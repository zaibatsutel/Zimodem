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

#ifndef __WIFICLIENTNODE_H__
#define __WIFICLIENTNODE_H__

//#include <ESP8266WiFi.h>
#include "../board/board.h"
#include "../serout.h"

#define LAST_PACKET_BUF_SIZE 256
#define OVERFLOW_BUF_SIZE 256
#define UNDERFLOW_BUF_MAX_SIZE 256

static WiFiClient *createWiFiClient(bool SSL)
{
  /*** adds like 8% to the total!
  if(SSL)
    return new WiFiClientSecure();
  else
  */
  return new WiFiClient();
}

class WiFiClientNode : public Stream
{
  private:
    void finishConnectionLink();
    int flushOverflowBuffer();
    WiFiClient client;
    WiFiClient *clientPtr;
    bool answered=true;
    int ringsRemain=0;
    long nextRingMillis = 0;

  public:
    int id=0;
    char *host;
    int port;
    bool wasConnected=false;
    bool serverClient=false;
    int flagsBitmap = 0;
    char *delimiters = NULL;
    char *maskOuts = NULL;
    char *stateMachine = NULL;
    char *machineState = NULL;
    String machineQue = "";

    uint8_t lastPacketBuf[LAST_PACKET_BUF_SIZE];
    int lastPacketLen=0;
    //uint8_t overflowBuf[OVERFLOW_BUF_SIZE];
    //int overflowBufLen = 0;
    uint8_t underflowBuf[UNDERFLOW_BUF_MAX_SIZE];
    size_t underflowBufLen = 0;
    WiFiClientNode *next = NULL;

    WiFiClientNode(char *hostIp, int newport, int flagsBitmap);
    WiFiClientNode(WiFiClient newClient, int flagsBitmap, int ringDelay);
    virtual ~WiFiClientNode();
    bool isConnected();

    FlowControlType getFlowControl();
    bool isPETSCII();
    bool isEcho();
    bool isTelnet();

    bool isAnswered();
    void answer();
    int ringsRemaining(int delta);
    long nextRingTime(long delta);

    bool isDisconnectedOnStreamExit();
    void setDisconnectOnStreamExit(bool tf);

    size_t write(uint8_t c);
    size_t write(const uint8_t *buf, size_t size);
    int read();
    int peek();
    void flush();
    int available();
    int read(uint8_t *buf, size_t size);
    String readLine(int timeout);

    static int getNumOpenWiFiConnections();
};

extern WiFiClientNode *conns;

#endif
