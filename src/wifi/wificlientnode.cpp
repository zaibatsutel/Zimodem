/*
   Copyright 2016-2017 Bo Zimmerman

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#include "wificlientnode.h"
#include "wifiservernode.h"
#include "../mode/zcommand.h"
#include "../connSettings.h"
#include "../zimodem.h"

void WiFiClientNode::finishConnectionLink()
{
  wasConnected=true;
  if(conns == NULL)
    conns = this;
  else
  {
    WiFiClientNode *last = conns;
    while(last->next != NULL)
      last = last->next;
    last->next = this;
  }
  checkOpenConnections();
}

WiFiClientNode::WiFiClientNode(char *hostIp, int newport, int flagsBitmap)
{
  port=newport;
  host=new char[strlen(hostIp)+1];
  strcpy(host,hostIp);
  id=++WiFiNextClientId;
  this->flagsBitmap = flagsBitmap;
  clientPtr = createWiFiClient((flagsBitmap&FLAG_SECURE)==FLAG_SECURE);
  client = *clientPtr;
  client.setNoDelay(DEFAULT_NO_DELAY);
  setCharArray(&delimiters,"");
  setCharArray(&maskOuts,"");
  setCharArray(&stateMachine,"");
  machineState = stateMachine;
  if(!client.connect(hostIp, port))
  {
    // deleted when it returns and is deleted
  }
  else
  {
    finishConnectionLink();
  }
}
    
WiFiClientNode::WiFiClientNode(WiFiClient newClient, int flagsBitmap, int ringDelay)
{
  this->flagsBitmap = flagsBitmap;
  clientPtr=NULL;
  newClient.setNoDelay(DEFAULT_NO_DELAY);
  port=newClient.localPort();
  setCharArray(&delimiters,"");
  setCharArray(&maskOuts,"");
  setCharArray(&stateMachine,"");
  machineState = stateMachine;
  String remoteIPStr = newClient.remoteIP().toString();
  const char *remoteIP=remoteIPStr.c_str();
  host=new char[remoteIPStr.length()+1];
  strcpy(host,remoteIP);
  id=++WiFiNextClientId;
  client = newClient;
  answered=(ringDelay == 0);
  if(ringDelay > 0)
  {
    ringsRemain = ringDelay;
    nextRingMillis = millis() + 3000;
  }
  finishConnectionLink();
  serverClient=true;
}
    
WiFiClientNode::~WiFiClientNode()
{
  lastPacketLen=0;
  if(host!=NULL)
  {
    client.stop();
    delete host;
    host=NULL;
  }
  if(clientPtr != NULL)
  {
    delete clientPtr;
    clientPtr = NULL;
  }
  if(conns == this)
    conns = next;
  else
  {
    WiFiClientNode *last = conns;
    while((last != NULL) && (last->next != this)) // don't change this!
      last = last->next;
    if(last != NULL)
      last->next = next;
  }
  if(commandMode.current == this)
    commandMode.current = conns;
  if(commandMode.nextConn == this)
    commandMode.nextConn = conns;
  underflowBufLen = 0;
  freeCharArray(&delimiters);
  freeCharArray(&maskOuts);
  freeCharArray(&stateMachine);
  machineState = NULL;
  next=NULL;
  checkOpenConnections();
}

bool WiFiClientNode::isConnected()
{
  return (host != NULL) && client.connected();
}

bool WiFiClientNode::isPETSCII()
{
  return (flagsBitmap & FLAG_PETSCII) == FLAG_PETSCII;
}

bool WiFiClientNode::isEcho()
{
  return (flagsBitmap & FLAG_ECHO) == FLAG_ECHO;
}

FlowControlType WiFiClientNode::getFlowControl()
{
  if((flagsBitmap & FLAG_RTSCTS) == FLAG_RTSCTS)
    return FCT_RTSCTS;
  if((flagsBitmap & FLAG_XONXOFF) == FLAG_XONXOFF)
    return FCT_NORMAL;
  return FCT_DISABLED;
}

bool WiFiClientNode::isTelnet()
{
  return (flagsBitmap & FLAG_TELNET) == FLAG_TELNET;
}

bool WiFiClientNode::isDisconnectedOnStreamExit()
{
  return (flagsBitmap & FLAG_DISCONNECT_ON_EXIT) == FLAG_DISCONNECT_ON_EXIT;
}

void WiFiClientNode::setDisconnectOnStreamExit(bool tf)
{
  if(tf)
    flagsBitmap = flagsBitmap | FLAG_DISCONNECT_ON_EXIT;
  else
    flagsBitmap = flagsBitmap & ~FLAG_DISCONNECT_ON_EXIT;
}

int WiFiClientNode::read()
{
  if((host == NULL)||(!answered))
    return 0;
  if(underflowBufLen > 0)
  {
    int b = underflowBuf[0];
    memcpy(underflowBuf,underflowBuf+1,--underflowBufLen);
    return b;
  }
  return client.read();
}

int WiFiClientNode::peek()
{
  if((host == NULL)||(!answered))
    return 0;
  if(underflowBufLen > 0)
    return underflowBuf[0];
  return client.peek();
}

void WiFiClientNode::flush()
{
  if((host != NULL)&&(client.available()==0))
  {
    flushOverflowBuffer();
    client.flush();
  }
}

int WiFiClientNode::available()
{
  if((host == NULL)||(!answered))
    return 0;
  if(underflowBufLen > 0)
    return underflowBufLen;
  return client.available();
}

int WiFiClientNode::read(uint8_t *buf, size_t size)
{
  if((host == NULL)||(!answered))
    return 0;
  // this whole underflow buf len thing is to get around yet another
  // problem in the underlying library where a socket disconnection
  // eats away any stray available bytes in their buffers.
  if(underflowBufLen > 0)
  {
    if(underflowBufLen <= size)
    {
      memcpy(buf,underflowBuf,underflowBufLen);
      size = underflowBufLen;
      underflowBufLen = 0;
    }
    else
    {
      memcpy(buf,underflowBuf,size);
      underflowBufLen -= size;
      memcpy(underflowBuf,underflowBuf+size,underflowBufLen);
    }
    return size;
  }
  
  int bytesRead = client.read(buf,size);
  int newAvail = client.available(); 
  if((newAvail>0) && (newAvail<size) && (newAvail<UNDERFLOW_BUF_MAX_SIZE))
    underflowBufLen = client.read(underflowBuf,newAvail); 
  return bytesRead;
}

int WiFiClientNode::flushOverflowBuffer()
{
  /*
   * I've never gotten any of this to trigger, and could use those
   * extra 260 bytes per connection
  if(overflowBufLen > 0)
  {
    // because overflowBuf is not a const char* for some reason
    // we need to explicitly declare that we want one
    // The simplest thing to do is pin down the first character of the
    // array and call it a day.
    // This avoids client.write<T>(buffer, length) from being seen by the
    // compiler as a better way to poke at it. 
    const uint8_t* overflowbuf_ptr = &overflowBuf[0];
    int bufWrite=client.write(overflowbuf_ptr,overflowBufLen);
    if(bufWrite >= overflowBufLen)
    {
      overflowBufLen = 0;
      s_pinWrite(pinRTS,rtsActive);
      // fall-through
    }
    else
    {
      if(bufWrite > 0)
      {
        for(int i=bufWrite;i<overflowBufLen;i++)
          overflowBuf[i-bufWrite]=overflowBuf[i];
        overflowBufLen -= bufWrite;
      }
      s_pinWrite(pinRTS,rtsInactive);
      return bufWrite;
    }
  }
   */
  return 0;
}

size_t WiFiClientNode::write(const uint8_t *buf, size_t size)
{
  int written = 0;
  /* overflow buf is pretty much useless now
  if(host == NULL)
  {
    if(overflowBufLen>0)
    {
      s_pinWrite(pinRTS,rtsActive);
    }
    overflowBufLen=0;
    return 0;
  }
  written += flushOverflowBuffer();
  if(written > 0)
  {
    for(int i=0;i<size && overflowBufLen<OVERFLOW_BUF_SIZE;i++,overflowBufLen++)
      overflowBuf[overflowBufLen]=buf[i];
    return written;
  }
  */
  written += client.write(buf, size);
  /*
  if(written < size)
  {
    for(int i=written;i<size && overflowBufLen<OVERFLOW_BUF_SIZE;i++,overflowBufLen++)
      overflowBuf[overflowBufLen]=buf[i];
    s_pinWrite(pinRTS,rtsInactive);
  }
  */
  return written;
}

String WiFiClientNode::readLine(int timeout)
{
  long now=millis();
  String line = "";
  while(((millis()-now < timeout) || (available()>0)))
  {
    yield();
    if(available()>0)
    {
      char c=read();
      if((c=='\n')||(c=='\r'))
      {
          if(line.length()>0)
            return line;
      }
      else
      if((c >= 32 ) && (c <= 127))
        line += c;
    }
  }
  return line;
}

void WiFiClientNode::answer()
{
  answered=true;
  ringsRemain=0;
  nextRingMillis=0;
}

bool WiFiClientNode::isAnswered()
{
  return answered;
}

int WiFiClientNode::ringsRemaining(int delta)
{
  ringsRemain += delta;
  return ringsRemain;
}

long WiFiClientNode::nextRingTime(long delta)
{
  nextRingMillis += delta;
  return nextRingMillis;
}

size_t WiFiClientNode::write(uint8_t c)
{
  const uint8_t one[] = {c};
  write(one,1);
}

int WiFiClientNode::getNumOpenWiFiConnections()
{
  int num = 0;
  WiFiClientNode *conn = conns;
  while(conn != NULL)
  {
    if((conn->isConnected()
     ||(conn->available()>0)
     ||((conn == conns)
       &&((serialOutBufferBytesRemaining() <SER_WRITE_BUFSIZE-1)
         ||(HWSerial.availableForWrite()<SER_BUFSIZE))))
    && conn->isAnswered())
      num++;
    conn = conn->next;
  }
  return num;
}

