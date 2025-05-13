/*!
 *@file LoRaCad.ino
 *@brief LoRa channel activity detection.
 *@details In LoRa communication, the specified frequency, spreading factor,
           and bandwidth constitute a channel. This example uses the "startCad"
           function to detect whether the channel is occupied. The detection 
           result is returned through the callback function "loraCadDone".
           Within "loraCadDone", "startCad" is called again to enable continuous
           channel activity detection.
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

DFRobot_LoRaRadio radio;

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

void loraCadDone(bool busy)
{
    if(busy){        
        printf("channel busy, can NOT send data\n");
    }else{
        radio.sendData("hello", /*size=*/5);
        printf("channel free, sending 5 bytes data\n");
    }
    radio.startCad(LORA_SPREADING_FACTOR, LORA_CAD_16_SYMBOL);
}

// Transmission complete callback function
void loraTxDone(void)
{
    printf("-------------------------tran done-----------------------------\n");
}

void setup()
{
    Serial.begin(115200);
    delay(5000);                        // Open the serial port within 5 seconds after uploading to view full print output
    radio.init();
    radio.setCadCallback(loraCadDone);
    radio.setTxCallback(loraTxDone);    // Set the transmission complete callback function
    radio.setFrequency(RF_FREQUENCY);   // Set the communication frequency
    radio.setTxEirp(TX_EIRP);
    radio.setSpreadingFactor(LORA_SPREADING_FACTOR);
    radio.setTxCallback(loraTxDone);
    /*
    The CAD detection time, when SF=7 and BW=125kHz, is 1.024ms per symbol.
    LORA_CAD_01_SYMBOL =（2^SF）/BW     = 1.024ms
    LORA_CAD_02_SYMBOL = 2*（2^SF）/BW  = 2.048ms
    LORA_CAD_04_SYMBOL = 4*（2^SF）/BW  = 4.096ms
    LORA_CAD_08_SYMBOL = 8*（2^SF）/BW  = 8.192ms
    LORA_CAD_16_SYMBOL = 16*（2^SF）/BW = 16.384ms
    */
    radio.startCad(LORA_SPREADING_FACTOR, LORA_CAD_16_SYMBOL);
}

void loop()
{
    delay(10 * 1000);
}