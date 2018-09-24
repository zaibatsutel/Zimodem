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
#include "zstream.h"
#include "zcommand.h"
#include "../wifi/wifiservernode.h"
#include "../modem_state.h"
#include "../zimodem.h"

ZStream streamMode;

void ZStream::switchTo(WiFiClientNode *conn)
{
  current = conn;
  currentExpiresTimeMs = 0;
  lastNonPlusTimeMs = 0;
  plussesInARow=0;
  serial.setXON(true);
  #ifdef INCLUDE_PETSCII
  serial.setPetsciiMode(isPETSCII());
  #endif
  serial.setFlowControlType(getFlowControl());
  currMode=&streamMode;
  checkBaudChange();
  if(pinSupport[pinDTR])
    lastDTR = digitalRead(pinDTR);
}

bool ZStream::isPETSCII()
{
  #ifdef INCLUDE_PETSCII
  return (current != NULL) && (current->isPETSCII());
  #else
  return false;
  #endif
}

bool ZStream::isEcho()
{
  return (current != NULL) && (current->isEcho());
}

FlowControlType ZStream::getFlowControl()
{
  return (current != NULL) ? (current->getFlowControl()) : FCT_DISABLED;
}

bool ZStream::isTelnet()
{
  return (current != NULL) && (current->isTelnet());
}

bool ZStream::isDisconnectedOnStreamExit()
{
  return (current != NULL) && (current->isDisconnectedOnStreamExit());
}

void ZStream::serialIncoming()
{
  int bytesAvailable = HWSerial.available();
  if(bytesAvailable == 0)
    return;
  while(--bytesAvailable >= 0)
  {
    uint8_t c=HWSerial.read();
    logSerialIn(c);
    if((c==commandMode.EC)
    &&((plussesInARow>0)||((millis()-lastNonPlusTimeMs)>800)))
      plussesInARow++;
    else
    if(c!=commandMode.EC)
    {
      plussesInARow=0;
      lastNonPlusTimeMs=millis();
    }
    if((c==19)&&(getFlowControl()==FCT_NORMAL))
      serial.setXON(false);
    else
    if((c==17)&&(getFlowControl()==FCT_NORMAL))
      serial.setXON(true);
    else
    {
      if(isEcho())
        serial.printb(c);
      #ifdef INCLUDE_PETSCII
      if(isPETSCII())
        c = petToAsc(c);
      #endif
      socketWrite(c);
    }
  }
  
  currentExpiresTimeMs = 0;
  if(plussesInARow==3)
    currentExpiresTimeMs=millis()+800;
}

void ZStream::switchBackToCommandMode(bool logout)
{
  if(logout && (current != NULL) && isDisconnectedOnStreamExit())
  {
    if(!commandMode.suppressResponses)
    {
      if(commandMode.numericResponses)
      {
        serial.prints("3");
        serial.prints(commandMode.EOLN);
      }
      else
      if(current->isAnswered())
      {
        serial.prints("NO CARRIER");
        serial.prints(commandMode.EOLN);
      }
    }
    delete current;
  }
  current = NULL;
  currMode = &commandMode;
}

void ZStream::socketWrite(uint8_t c)
{
  if(current->isConnected())
  {
    current->write(c);
    logSocketOut(c);
    current->flush(); // rendered safe by available check
    delay(0);
    yield();
  }
}

void ZStream::loop()
{
  WiFiServerNode *serv = servs;
  while(serv != NULL)
  {
    if(serv->hasClient())
    {
      WiFiClient newClient = serv->server->available();
      if(newClient.connected())
      {
        int port=newClient.localPort();
        String remoteIPStr = newClient.remoteIP().toString();
        const char *remoteIP=remoteIPStr.c_str();
        bool found=false;
        WiFiClientNode *c=conns;
        while(c!=NULL)
        {
          if((c->isConnected())
          &&(c->port==port)
          &&(strcmp(remoteIP,c->host)==0))
            found=true;
          c=c->next;
        }
        if(!found)
        {
          newClient.write(busyMsg.c_str());
          newClient.flush();
          //can't confirm this even works...
          newClient.stop();
        }
      }
    }
    serv=serv->next;
  }
  
  WiFiClientNode *conn = conns;
  long now=millis();
  while(conn != NULL)
  {
    WiFiClientNode *nextConn = conn->next;
    if((!conn->isAnswered())&&(conn->isConnected())&&(conn!=current))
    {
      conn->write((uint8_t *)busyMsg.c_str(), busyMsg.length());
      conn->flush();
      //can't confirm this even works...
      delete conn;
    }
    conn = nextConn;
  }
  
  if(pinSupport[pinDTR])
  {
    if(lastDTR==dtrActive)
    {
      lastDTR = digitalRead(pinDTR);
      if((lastDTR==dtrInactive)
      &&(dtrInactive != dtrActive))
      {
        if(current != NULL)
          current->setDisconnectOnStreamExit(true);
        switchBackToCommandMode(true);
      }
    }
    lastDTR = digitalRead(pinDTR);
  }
  if((current==NULL)||(!current->isConnected()))
  {
    switchBackToCommandMode(true);
  }
  else
  if((currentExpiresTimeMs > 0) && (millis() > currentExpiresTimeMs))
  {
    currentExpiresTimeMs = 0;
    if(plussesInARow == 3)
    {
      plussesInARow=0;
      if(current != 0)
      {
        switchBackToCommandMode(false);
      }
    }
  }
  else
  if(serial.isSerialOut())
  {
    if(current->available()>0)
    //&&(current->isConnected()) // not a requirement to have available bytes to read
    {
      int bufferRemaining=serialOutBufferBytesRemaining();
      if(bufferRemaining > 0)
      {
        int bytesAvailable = current->available();
        if(bytesAvailable > bufferRemaining)
          bytesAvailable = bufferRemaining;
        if(bytesAvailable>0)
        {
          for(int i=0;(i<bytesAvailable) && (current->available()>0);i++)
          {
            if(serial.isSerialCancelled())
              break;
            uint8_t c=current->read();
            logSocketIn(c);
            if((!isTelnet() || handleAsciiIAC((char *)&c,current))
            && (!isPETSCII() || ascToPet((char *)&c,current)))
              serial.printb(c);
          }
        }
      }
    }
    if(serial.isSerialOut())
      serialOutDeque();
  }
  checkBaudChange();
}

