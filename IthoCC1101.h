/*
   Author: Klusjesman, supersjimmie, modified and reworked by arjenhiemstra
*/

#pragma once

#include <stdio.h>
#include "CC1101.h"
#include "IthoPacket.h"

#define MAX_NUMBER_OF_REMOTES 10

//struct ithoRFCapab {
//  std::string name;
//  int32_t value;
//};
//
//struct ithoRFDevices {
//  uint32_t deviceId;
//  std::vector<ithoRFCapab> Capabilities;
//
//  ithoRFDevices(): deviceId(0), Capabilities()
//  {
//  }
//
//};

struct ithoRFDevice {
  uint32_t deviceId {0};
//  char name[16];
  IthoCommand lastCommand {IthoUnknown};
  int32_t co2 {0xEFFF};
  int32_t temp {0xEFFF};
  int32_t hum {0xEFFF};
  int32_t dewpoint {0xEFFF};
  int32_t battery {0xEFFF};
};

struct ithoRFDevices {
  uint8_t count {0};
  ithoRFDevice device[MAX_NUMBER_OF_REMOTES];
};

//pa table settings
const uint8_t ithoPaTableSend[8] = {0x6F, 0x26, 0x2E, 0x8C, 0x87, 0xCD, 0xC7, 0xC0};
const uint8_t ithoPaTableReceive[8] = {0x6F, 0x26, 0x2E, 0x7F, 0x8A, 0x84, 0xCA, 0xC4};

const uint8_t messageOpcodeRemote[] =        { 0x22, 0xF1 };
const uint8_t messageOpcodeRemoteAutoCO2[] = { 0x22, 0xF8 };
const uint8_t messageOpcodeTimer[] =         { 0x22, 0xF3 };
const uint8_t messageOpcodeRFTRV[] =         { 0x31, 0xE0 };
const uint8_t messageOpcodeRFBind[] =        { 0x1F, 0xC9 };

//message command bytes
const uint8_t ithoMessageRVHighCommandBytes[] =   {49, 224, 4, 0, 0, 200, 0};
const uint8_t ithoMessageHighCommandBytes[] =     {34, 241, 3, 0, 4, 4};
const uint8_t ithoMessageFullCommandBytes[] =     {34, 241, 3, 0, 4, 4};
const uint8_t ithoMessageMediumCommandBytes[] =   {34, 241, 3, 0, 3, 4};
const uint8_t ithoMessageRVMediumCommandBytes[] = {34, 241, 3, 0, 3, 7};
const uint8_t ithoMessageLowCommandBytes[] =      {34, 241, 3, 0, 2, 4};
const uint8_t ithoMessageRVLowCommandBytes[] =    {49, 224, 4, 0, 0, 1, 0};
const uint8_t ithoMessageRVAutoCommandBytes[] =   {34, 241, 3, 0, 5, 7};
const uint8_t ithoMessageStandByCommandBytes[] =  {0, 0, 0, 0, 0, 0};    //unkown, tbd
const uint8_t ithoMessageTimer1CommandBytes[] =   {34, 243, 3, 0, 0, 10}; //10 minutes full speed
const uint8_t ithoMessageTimer2CommandBytes[] =   {34, 243, 3, 0, 0, 20}; //20 minutes full speed
const uint8_t ithoMessageTimer3CommandBytes[] =   {34, 243, 3, 0, 0, 30}; //30 minutes full speed
const uint8_t ithoMessageJoinCommandBytes[] =     {31, 201, 12, 0, 34, 241};
const uint8_t ithoMessageJoin2CommandBytes[] =    {31, 201, 12, 99, 34, 248}; //join command of RFT AUTO Co2 remote
const uint8_t ithoMessageRVJoinCommandBytes[] =   {31, 201, 24, 0, 49, 224}; //join command of RFT-RV
const uint8_t ithoMessageLeaveCommandBytes[] =    {31, 201, 6, 0, 31, 201};
//itho rft-rv
//unknown, high
//148,216,43,49,224,4,0,0,200,0,3,127,244,78,11,155,154,225,11,96,138
//148,216,43,49,224,4,0,0,200,0,3,127,51,80,47,233,94,6,189,114,73

//low
//148,216,43,49,224,4,0,0,1,0,202,127,242,212,160,123,15,64,7,129,33
//148,216,43,34,241,3,0,4,4,194,127,255,189,90,107,88,72,115,49,192,105

//join
//151,149,65,31,201,24,0,49,224,151,149,65,0,18,160,151,149,65,1,16,224

class IthoPacket;

class IthoCC1101 : protected CC1101
{
  private:
    //receive
    CC1101Packet inMessage;                       //temp storage message2
    IthoPacket inIthoPacket;                        //stores last received message data

    //send
    IthoPacket outIthoPacket;                       //stores state of "remote"

    //settings
    uint8_t sendTries;                            //number of times a command is send at one button press

    uint8_t cc_freq[3]; //FREQ0, FREQ1, FREQ2


    //Itho remotes
//    std::vector<ithoRFDevices> IthoRFDevices;
    bool bindAllowed;
    bool allowAll;
    ithoRFDevices ithoRF;

    //functions
  public:
    IthoCC1101(uint8_t counter = 0, uint8_t sendTries = 3);   //set initial counter value
    ~IthoCC1101();



    //init
    void init() {
      CC1101::init();  //init,reset CC1101
      initReceive();
    }
    void initReceive();
    uint8_t getLastCounter() {
      return outIthoPacket.counter;  //counter is increased before sending a command
    }
    void setSendTries(uint8_t sendTries) {
      this->sendTries = sendTries;
    }
    void setDeviceID(uint8_t byte0, uint8_t byte1, uint8_t byte2) {
      this->outIthoPacket.deviceId[0] = byte0;
      this->outIthoPacket.deviceId[1] = byte1;
      this->outIthoPacket.deviceId[2] = byte2;
    }

    bool addRFDevice(uint8_t byte0, uint8_t byte1, uint8_t byte2);
    bool addRFDevice(uint32_t ID);
    bool removeRFDevice(uint8_t byte0, uint8_t byte1, uint8_t byte2);
    bool removeRFDevice(uint32_t ID);
    bool checkRFDevice(uint8_t byte0, uint8_t byte1, uint8_t byte2);
    bool checkRFDevice(uint32_t ID);
    void setBindAllowed(bool input) {
      bindAllowed = input;
    }
    bool getBindAllowed() {
      return bindAllowed;
    }
    void setAllowAll(bool input) {
      allowAll = input;
    }
    bool getAllowAll() {
      return allowAll;
    }
    const struct ithoRFDevices &getRFdevices() const {
      return ithoRF;
    }    
    //receive
    uint8_t receivePacket();  //read RX fifo
    bool checkForNewPacket();
    IthoPacket getLastPacket() {
      return inIthoPacket;  //retrieve last received/parsed packet from remote
    }
    IthoCommand getLastCommand() {
      return inIthoPacket.command;  //retrieve last received/parsed command from remote
    }
    uint8_t getLastInCounter() {
      return inIthoPacket.counter;  //retrieve last received/parsed command from remote
    }
    uint8_t ReadRSSI();
    bool checkID(const uint8_t *id);
    int * getLastID();
    String getLastIDstr(bool ashex = true);
    String getLastMessagestr(bool ashex = true);
    String LastMessageDecoded();

    //send
    void sendCommand(IthoCommand command);


    void handleBind();
    void handleLevel();
    void handleTimer();
    void handleStatus();
    void handleRemotestatus();
    void handleTemphum();
    void handleCo2();
    void handleBattery();

  protected:
  private:
    IthoCC1101( const IthoCC1101 &c);
    IthoCC1101& operator=( const IthoCC1101 &c);

    //init CC1101 for receiving
    void initReceiveMessage();

    //init CC1101 for sending
    void initSendMessage(uint8_t len);
    void finishTransfer();

    //parse received message
    bool parseMessageCommand();
    bool checkIthoCommand(IthoPacket *itho, const uint8_t commandBytes[]);

    //send
    void createMessageStart(IthoPacket *itho, CC1101Packet *packet);
    void createMessageCommand(IthoPacket *itho, CC1101Packet *packet);
    void createMessageJoin(IthoPacket *itho, CC1101Packet *packet);
    void createMessageLeave(IthoPacket *itho, CC1101Packet *packet);
    uint8_t* getMessageCommandBytes(IthoCommand command);
    uint8_t getCounter2(IthoPacket *itho, uint8_t len);

    uint8_t messageEncode(IthoPacket *itho, CC1101Packet *packet);
    void messageDecode(CC1101Packet *packet, IthoPacket *itho);


}; //IthoCC1101