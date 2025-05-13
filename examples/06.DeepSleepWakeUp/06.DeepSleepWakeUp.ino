/*!
 *@file DeepSleepWakeUp.ino
 *@brief LoRaWAN low-power mode.
 *@details After the node joins the network or sends data, it will enter 
           sleep mode and wake up after the specified timer duration to 
           continue sending data before entering sleep again. During sleep, 
           pressing a button can wake up the node and print some network parameters.
 *@copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@licence The MIT License (MIT)
 *@author [Martin](Martin@dfrobot.com)
 *@version V0.0.1
 *@date 2025-2-21
 *@wiki en:https://wiki.dfrobot.com/lorawan
 *@wiki cn:https://wiki.dfrobot.com.cn/lorawan
 *@get from https://www.dfrobot.com
 *@url https://gitee.com/dfrobotcd/lorawan-esp32-sdk
 */

#include "DFRobot_LoRaWAN.h"

// Button pin
#define BTN_PIN 18  // GPIO2, 3, 11, 12, 13 can all trigger external wake-up

LCD_OnBoard screen;

#define BG_COLOR        COLOR_RGB565_BLACK      // Screen background color
#define TEXT_COLOR      COLOR_RGB565_GREEN      // Screen font color
/*
* Recommended font
* -------------------------------------------------------------------------------
* FreeMono9pt7b, FreeMono12pt7b, FreeMonoBold9pt7b, FreeSans9pt7b, FreeSerif9pt7b
* For more fonts, see the directory: src\external\DFRobot_GDL_LW\src\Frame\Fonts\
*/ 
#define TEXT_FONT       &FreeMono9pt7b          // font
#define TEXT_SIZE       1                       // Screen font size
#define LINE_HEIGHT     18                      // Line height
#define POX_X           0                       // Screen print position X coordinate
#define POX_Y           15                      // Screen print position Y coordinate
#define LINE_1          0                       // Line number
#define LINE_2          1
#define LINE_3          2
#define LINE_4          3

// Data packet transmission interval
#define APP_INTERVAL_MS 10000
// LoRaWAN DevEUI
const uint8_t DevEUI[8] = {0xDF, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
// LoRaWAN AppEUI/JoinEUI
const uint8_t AppEUI[8] = {0xDF, 0xB7, 0xB7, 0xB7, 0xB7, 0x00, 0x00, 0x00};
// LoRaWAN AppKEY
const uint8_t AppKey[16] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
  0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

// LoRaWAN application data buffer
uint8_t buffer[255];
// Application port number
uint8_t port = 2;
// LoRaWAN_Node node(DevEUI, AppEUI, AppKey, /*classType=*/CLASS_A);
LoRaWAN_Node node(DevEUI, AppEUI, AppKey);
TimerEvent_t appTimer;

// Join network callback function
void joinCb(bool isOk, int16_t rssi, int8_t snr)
{
    screen.fillScreen(BG_COLOR);    
    screen.setTextColor(TEXT_COLOR);
    screen.setFont(TEXT_FONT);
    screen.setTextSize(TEXT_SIZE);

    if(isOk){
        printf("JOIN SUCCESS\n");
        printf("JoinAccept Packet rssi = %d snr = %d\n", rssi, snr);
        printf("NetID = %06X\n", node.getNetID());
        printf("DevAddr = %08X\n", node.getDevAddr());
        uint8_t * NwkSKey = node.getNwkSKey();
        uint8_t * AppSKey = node.getAppSKey();
        printf("NwkSKey=0X");
        for(uint8_t i= 0; i < 16; i++){
            printf("%02X", NwkSKey[i]);
        }
        printf("\n");
        printf("AppSKey=0X");
        for(uint8_t i = 0; i < 16; i++){
            printf("%02X", AppSKey[i]);
        }
        printf("\n");

        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
        screen.printf("JOIN SUCCESS");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
        screen.printf("Accept Packet");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
        screen.printf("Rssi = %d", rssi);
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_4);
        screen.printf("Snr = %d", snr);
        delay(5000);

        node.TimerValue(&appTimer, APP_INTERVAL_MS);
        node.TimerStart(&appTimer);
    }else{
        printf("OTAA join error\n");
        printf("Check Whether the device has been registered on the gateway!\n");
        printf("deviceEUI and appKey are the same as the devices registered on the gateway\n");
        printf("Ensure that there is a gateway nearby\n");
        printf("Check whether the antenna is normal\n");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
        screen.printf("OTAA join Err!");
        delay(2000);    
        
        /*
        * If the network joining fails in sleep mode, 
        * it will automatically attempt to join the network again, 
        * so there is no need to call "joinCb" here again.        
        */
    }
  
}

void userSendConfirmedPacket(void)
{    
    const char * data = "DFRobot"; 
    uint32_t datalen = strlen(data);
    memcpy(buffer, data, datalen);    
    node.sendConfirmedPacket(port, buffer, /*size=*/datalen);
    
    screen.fillScreen(BG_COLOR);    
    screen.setTextColor(TEXT_COLOR);
    screen.setFont(TEXT_FONT);
    screen.setTextSize(TEXT_SIZE);
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
    screen.printf("Sending...");
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
    screen.printf("Confirmed");
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
    screen.printf("Packet");
}

// Send data callback function
void txCb(bool isconfirm, int8_t datarate, int8_t txeirp, uint8_t Channel)
{
    // Real transmission power and other information can be printed within the send data callback function
    // screen.fillScreen(BG_COLOR);    
    // screen.setTextColor(TEXT_COLOR);
    // screen.setFont(TEXT_FONT);
    // screen.setTextSize(TEXT_SIZE);

    // screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
    // screen.printf(isconfirm?"Confirm Packet":"Unconfirm Pack");
    // screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
    // screen.printf("datarate: %d", datarate);
    // screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
    // screen.printf("txeirp: %d", txeirp);
    // screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_4);
    // screen.printf("Channel: %d", Channel);
}

// Receive data callback function
void rxCb(void *buffer, uint16_t size, uint8_t port, int16_t rssi, int8_t snr, bool ackReceived, uint16_t uplinkCounter, uint16_t downlinkCounter)
{
    screen.fillScreen(BG_COLOR);    
    screen.setTextColor(TEXT_COLOR);
    screen.setFont(TEXT_FONT);
    screen.setTextSize(TEXT_SIZE);
    if(ackReceived == true){
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
        screen.printf("this is a ACK");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
        screen.printf("Rssi = %d", rssi);
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
        screen.printf("Snr = %d", snr);
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_4);
        screen.printf("DownCount = %d", downlinkCounter);
    }

    // You can print the received data here
    // if(size != 0){
    //     screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
    //     screen.printf("Data:%s\n", (uint8_t*)buffer);
    //     screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
    //     for(uint8_t i = 0; i < size; i++){
    //         screen.printf(",0x%x",((uint8_t*)buffer)[i]);
    //     }
    // }

    printf("enter deep sleep");
    delay(3000);

    node.TimerValue(&appTimer, APP_INTERVAL_MS);
    node.TimerStart(&appTimer);
}

// Button interrupt callback function
void buttonCB()
{    
    screen.fillScreen(BG_COLOR);    
    screen.setTextColor(TEXT_COLOR);
    screen.setFont(TEXT_FONT);
    screen.setTextSize(TEXT_SIZE);
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
    screen.printf("buttonCB");
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
    screen.printf("dataRate: %d\n", node.getDataRate());
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
    screen.printf("txEirp: %d\n", node.getTxEirp());
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_4);
    screen.printf("netID: %d\n", node.getNetID());
    
    delay(5000);
    node.sleepMs(APP_INTERVAL_MS);      // MCU sleep for a specified duration
}


void setup()
{
    Serial.begin(115200);
    screen.begin(); 
    /*
    * The screen backlight is turned on by default. 
    * You can use this method to control the backlight switch. 
    * When the backlight is turned off, the screen will not display any content.
    */
    // screen.Backlight(ON);
    screen.fillScreen(BG_COLOR);    
    screen.setTextColor(TEXT_COLOR);
    screen.setFont(TEXT_FONT);
    screen.setTextSize(TEXT_SIZE);
    screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
    screen.printf("WakeUp");    
    delay(2000); 

    /*
    * Region | Data Rate | Tx Eirp (Even Numbers Only)
    * ------------------------------------------------
    *  EU868 | DR0 ~ DR5 | 2, 4, 6 ~ 16 dBm
    *  US915 | DR0 ~ DR4 | 2, 4, 6 ~ 22 dBm
    */     
    if(!(node.init(/*dataRate=*/DR_4, /*txEirp=*/16))){     // Initialize the LoRaWAN node, set the data rate and Tx Eirp
        screen.fillScreen(BG_COLOR);        
        screen.setTextColor(TEXT_COLOR);
        screen.setFont(TEXT_FONT);
        screen.setTextSize(TEXT_SIZE);
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_1);
        screen.printf("LoRaWAN Init");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_2);
        screen.printf("Failed!");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_3);
        screen.printf("Please Check:");
        screen.setCursor(POX_X, POX_Y + LINE_HEIGHT * LINE_4);
        screen.printf("DR or Region");
        while(1);
    }
    //node.init(DR_5, 16, /*adr = */false, /*dutyCycle =*/LORAWAN_DUTYCYCLE_OFF);
    node.setSleepMode(MCU_DEEP_SLEEP);                      // Set MCU to low-power mode
    node.attachInterrupt(BTN_PIN, buttonCB, LOW);           // The button should be connected to an interrupt that can wake up the CPU
    node.setTxHander(txCb);                                 // Set the callback function for sending data
    node.setRxHander(rxCb);                                 // Set the callback function for receiving data
    node.TimerInit(&appTimer, userSendConfirmedPacket);     // Initialize timer event
    node.join(joinCb);                                      // Join the LoRaWAN network   
}

void loop()
{
    delay(1000);
}

