/*!
 *@file setSync.ino
 *@brief LoRa communicates by specifying a sync word.
 *@details Two nodes set the same sync word and are configured as master and slave to communicate with each other.
 *@n 1.Sync is part of the LoRa PHY layer packet start, allowing the receiver to determine where the packet begins.
 *@n 2.In point-to-point communication, two LoRa nodes can communicate normally only when they are set to the same sync.
 *@n 3.Sync can enhance transmission security. By using a predefined sync word, unauthorized devices are prevented from 
       sending packets into the network. Only devices that know the correct sync word can communicate with the network.
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

bool isMaster = true;       // master
//bool isMaster = false;    // slave
DFRobot_LoRaRadio radio;

char buffer[]={0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

// Transmission complete callback function
void loraTxDone(void)
{
    printf("-------------------------LoRa Tx done-----------------------------\n");
    radio.startRx(10 * 1000);   // Start receiving and set the receive timeout to 10s
}

void loraRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    uint8_t i=0;
    printf("LoRa data received on channel %ld, SF=%d Rssi=%d Snr=%d \nData={",RF_FREQUENCY, LORA_SPREADING_FACTOR, rssi, snr);
    for(; i<size-1; i++ ){
        printf("0x%02x, ", payload[i]);
    }
    printf("0x%02x}\n\n", payload[i]);
    delay(5000);
    radio.sendData(buffer, /*size=*/7);     // Send data
}

void loraRxTimeout(void)
{  
    if(isMaster){
        printf("master rx timeout, send LoRa packet again\n");
        radio.sendData(buffer, /*size=*/7);     // Send data
    }else{
        printf("slave rx timeout, restart receive\n");
        radio.startRx(10 * 1000);
    }
}

void setup()
{
    Serial.begin(115200);   // Initialize serial communication with a baud rate of 115200
    delay(5000);            // Open the serial port within 5 seconds after uploading to view full print output
    printf("---------%s----------\n",isMaster?"Master":"Slave");
    radio.init();                                       // Initialize the LoRa node with a default bandwidth of 125 KHz
    radio.setTxCallback(loraTxDone);                    // Set the transmission complete callback function
    radio.setRxCallBack(loraRxDone);                    // Set the receive complete callback function
    radio.setRxTimeOutCallback(loraRxTimeout);          // Set the receive timeout callback function
    //radio.setSync(/*sync = */0x3121);                 // Set sync word
    radio.setFrequency(RF_FREQUENCY);                   // Set the communication frequency
    radio.setTxEirp(TX_EIRP);                           // Set the Tx Eirp
    radio.setSpreadingFactor(LORA_SPREADING_FACTOR);    // Set the spreading factor
      
    if(isMaster != true){
        radio.startRx(10 * 1000);   // Start receiving and set the receive timeout to 10s
    }else{
        radio.sendData(buffer, /*size=*/7);     // Send data
    }
}

void loop()
{
    delay(10 * 1000); 
}