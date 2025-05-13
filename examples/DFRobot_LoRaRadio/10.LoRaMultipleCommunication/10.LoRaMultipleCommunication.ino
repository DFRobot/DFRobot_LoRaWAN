/*!
 *@file LoRaMultipleCommunication.ino
 *@brief LoRa multi-device communication.
 *@details In multi-node communication, include target address and source address information. 
           Each device can only parse messages with its own address as the target or messages 
           from a broadcast.
 *@copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@licence The MIT License (MIT)
 *@author [Martin](Martin@dfrobot.com)
 *@version V0.0.1
 *@date 2025-3-12
 *@wiki en:https://wiki.dfrobot.com/lorawan
 *@wiki cn:https://wiki.dfrobot.com.cn/lorawan
 *@get from https://www.dfrobot.com
 *@url https://gitee.com/dfrobotcd/lorawan-esp32-sdk
 */
#include "DFRobot_LoRaRadio.h"

/*
* Region | Spreading Factor | Tx Eirp (Even Numbers Only)
* -------------------------------------------------------
*  EU868 |    SF7 ~ SF12    | 2, 4, 6 ~ 16 dBm
*  US915 |    SF8 ~ SF12    | 2, 4, 6 ~ 22 dBm
*/  
#ifdef REGION_EU868
#define RF_FREQUENCY 868000000  // Hz
#define TX_EIRP 16    // dBm 
#endif
#ifdef REGION_US915
#define RF_FREQUENCY 915000000  // Hz
#define TX_EIRP 22    // dBm 
#endif
#define LORA_SPREADING_FACTOR 7

DFRobot_LoRaRadio radio;

char message[] = "Hello";
uint8_t buffer[36];
byte localAddr = 0xFD;        // Current device address
byte destAddr  = 0xBB;        // Target device address
byte msgCount = 0;            // Send message count
long prevTimeStamp = 0;       // Message timestamp
int interval = 15000;         // Send interval

// Transmission complete callback function
void loraTxDone(void)
{
    printf("-------------------------tran done-----------------------------\n");
    radio.startRx(10*1000); // Start receiving and set the receive timeout to 10s
}

void loraRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    // Parse packet header data.
    uint8_t to = payload[0];                // Recipient address
    uint8_t from = payload[1];              // Sender address
    uint8_t incomingMsgId = payload[2];     // MsgID
    uint8_t incomingLength = payload[3];    // Message length
  
    // If the recipient address is neither this device's address nor the broadcast address.
    if(to != localAddr && to != 0xFF){
        printf("This message is not for me. to=0x%X localAddr=0x%X\n", to, localAddr);
        return;
    }

    // If the message is intended for this device or is a broadcast message, print it.
    printf("Received from: 0x%x\n" , from);
    printf("Sent to: 0x%x\n" , to);
    printf("Message ID: %d\n" , incomingMsgId);
    printf("Message length: %d\n" , incomingLength);
    printf("Message: %s\n" ,(char*)(payload + 4));
    printf("RSSI: %d\n" , rssi);
    printf("Snr: %d\n" , snr);
}

void setup()
{
    Serial.begin(115200);   // Initialize serial communication with a baud rate of 115200
    delay(5000);
    printf("localAddr = 0x%02x\n", localAddr);
    printf("destAddr = 0x%02x\n", destAddr);
    
    radio.init();           // Initialize the LoRa node with a default bandwidth of 125 KHz
    radio.setTxCallback(loraTxDone);                    // Set the transmission complete callback function
    radio.setRxCallBack(loraRxDone);                    // Set the receive complete callback function
    radio.setFrequency(RF_FREQUENCY);                   // Set the communication frequency
    radio.setTxEirp(TX_EIRP);
    radio.setSpreadingFactor(LORA_SPREADING_FACTOR);
}

void loop()
{
    if(millis() - prevTimeStamp > interval){
        buffer[0] = destAddr;
        buffer[1] = localAddr;
        buffer[2] = msgCount;
        buffer[3] = sizeof(message);
        memcpy(buffer+4, message, sizeof(message)+1);
        radio.sendData(buffer, sizeof(message)+4);
        printf("Sending %s to Addr 0x%x\n" ,message,destAddr);
        prevTimeStamp = millis();            // Message timestamp
        msgCount++;
    }
}