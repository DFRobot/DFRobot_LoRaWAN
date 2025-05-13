/*!
 *@file LoRaEncryptReceive.ino
 *@brief Receive encrypted LoRa data from the air and decrypt it.
 *@details Receive encrypted LoRa data from the air and set a 10-second timeout. 
           If no data is received within the timeout, the interrupt service function 
           will trigger to receive again for another 10 seconds. If data is received, 
           decrypt it using the agreed key.
 *@copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@licence The MIT License (MIT)
 *@author [Martin](Martin@dfrobot.com)
 *@version V0.0.1
 *@date 2025-3-14
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
const uint8_t decryptKey[16] = {0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
                                0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12};

void loraRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    uint8_t i = 0;
    printf("LoRa data received on channel %ld, SF=%d Rssi=%d Snr=%d \nDecrypt Data={",RF_FREQUENCY, LORA_SPREADING_FACTOR, rssi, snr);
    for(; i<size-1; i++ ){
        printf("0x%02x, ", payload[i]);
    }
    printf("0x%02x}\n\n", payload[i]);
}

void loraRxTimeout(void)
{
    printf("loraRxTimeout\n");
    radio.startRx(10 * 1000);   // Start receiving and set the receive timeout to 10s
}

void setup()
{
    Serial.begin(115200);   // Initialize serial communication with a baud rate of 115200
    delay(5000);            // Open the serial port within 5 seconds after uploading to view full print output
    radio.init();           // Initialize the LoRa node with a default bandwidth of 125 KHz
    radio.setEncryptKey(decryptKey);                    // Set the communication key
    radio.setRxCallBack(loraRxDone);                    // Set the receive complete callback function
    radio.setRxTimeOutCallback(loraRxTimeout);          // Set the receive timeout callback function
    radio.setFrequency(RF_FREQUENCY);                   // Set the communication frequency
    radio.setSpreadingFactor(LORA_SPREADING_FACTOR);    // Set the spreading factor
    radio.startRx(10 * 1000);                           // Start receiving and set the receive timeout to 10s
}

void loop()
{
    delay(3000);
}