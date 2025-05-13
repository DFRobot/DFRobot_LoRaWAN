/*!
 *@file DFRobot_LoRaRadio.h
 *@brief Define the basic structure of class DFRobot_LoRaRadio, the implementation of basic methods.
 *@copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 *@licence The MIT License (MIT)
 *@author [Martin](Martin@dfrobot.com)
 *@version V0.0.1
 *@date 2025-3-14
 *@get from https://www.dfrobot.com
 *@url https://gitee.com/dfrobotcd/lorawan-esp32-sdk
*/
#ifndef __DFROBOT_LORARADIO_H_
#define __DFROBOT_LORARADIO_H_

#include "external\DFRobot_GDL_LW\src\DFRobot_GDL_LW.h"
#include "radio/radio.h"
#include <string.h>
#include "radio/sx126x/sx126x.h"

#define LCD_OnBoard LoRaWAN::DFRobot_ST7735_80x160_HW_SPI ///< The type of screen on the development board
#define SPI_MUTEX LoRaWAN::spimutex

/**
 * @fn onTxDone
 * @brief Callback function for when data transmission is completed.
 */
typedef void onTxDone(void);

/**
 * @fn onRxDone
 * @brief Callback function for when data reception is completed.
 * @param payload The received payload.
 * @param size The size of the received payload.
 * @param rssi The received signal strength indicator.
 * @param snr The signal-to-noise ratio.
 */
typedef void onRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/**
 * @fn OnCadDone
 * @brief Callback function for when channel activity detection is completed.
 * @param cadResult Whether the channel is free or busy.
 */
typedef void OnCadDone(bool cadResult);

/**
 * @fn onRxTimeout
 * @brief Callback function for when data reception times out.
 */
typedef void onRxTimeout(void);

/**
 * @fn onRxError
 * @brief Callback function for when data reception encounters an error.
 */
typedef void onRxError(void);

/**
 * @brief Class for interfacing with a LoRa radio module using the Semtech SX126x chip.
 */
class DFRobot_LoRaRadio
{

public:

  /**
   * @enum eBandwidths_t
   * @brief LoRa communication uses specific bandwidths.
   * @details When the bandwidth is larger, the data transmission rate increases, 
   *          while a smaller bandwidth provides stronger anti-interference capabilities 
   *          and enables longer transmission distances.
   */
  typedef enum
  {
       BW_125 = 0,   /**<125k HZ>*/
       BW_250,       /**<250k HZ>*/
       BW_500,       /**<500k HZ>*/
       BW_062,       /**<62k HZ>*/
       BW_041,       /**<41k HZ>*/
       BW_031,       /**<31k HZ>*/
       BW_020,       /**<20k HZ>*/
       BW_015,       /**<15k HZ>*/
       BW_010,       /**<10k HZ>*/
       BW_007 = 9,   /**<7k HZ>*/
  } eBandwidths_t;

public:
    /**
     * @fn DFRobot_LoRaRadio
     * @brief Constructor for the DFRobot_LoRaRadio class.
     * @return LoRaRadio node object
     */
    DFRobot_LoRaRadio();

    /**
     * @fn init
     * @brief Initializes the LoRa radio module.
     * @return None
     */
    void init();

    /**
     * @fn setTxEirp
     * @brief Sets the transmission power of the LoRa radio module.
     * @param txeirp Equivalent Isotropically Radiated Power(dBm)
     * @return None
     */
    void setTxEirp(int8_t txeirp);

    /**
     * @fn setSpreadingFactor
     * @brief Set the spreading factor of the radio.
     * @param SF The spreading factor to set (unsigned 8-bit integer).
     * @return None
     */
    void setSpreadingFactor(uint8_t SF);

    /**
     * @fn setBandwidth
     * @brief Set the bandwidth of the radio.
     * @param bandwidth The bandwidth to set (enumeration type eBandwidths_t).
     * @return None
     */
    void setBandwidth(eBandwidths_t bandwidth);

    /**
     * @fn setSync
     * @brief Sets the synchronization word of the LoRa radio module.
     * @param sync The synchronization word.
     * @return None
     */
    void setSync(uint16_t sync);

    /**
     * @fn setFrequency
     * @brief Sets the frequency of the LoRa radio module.
     * @param freq The frequency, in Hz.
     * @return None
     */
    void setFrequency(uint32_t freq);

    /**
     * @fn setTxCallback
     * @brief Sets the callback function for when data transmission is completed.
     * @param cb The callback function.
     * @return None
     */
    void setTxCallback(onTxDone cb);

    /**
     * @fn sendData
     * @brief Sends data using the LoRa radio module.
     * @param data Pointer to the data to be sent.
     * @param size The length of the data, in bytes.
     * @return None
     */
    void sendData(const void *data, uint8_t size);

    /**
     * @fn setRxCallBack
     * @brief Sets the callback function for when data reception is completed.
     * @param cb The callback function.
     * @return None
     */
    void setRxCallBack(onRxDone cb);

    /**
     * @fn setRxTimeOutCallback
     * @brief Sets the callback function for when data reception times out.
     * @param cb The callback
     * @return None
     */
     void setRxTimeOutCallback(onRxTimeout cb);

     /**
      * @fn setRxErrorCallback
      * @brief Sets the callback function for when data reception encounters an error.
      * @param cb The callback function.
      * @return None
      */
     void setRxErrorCallback(onRxError cb);

     /**
      * @fn startRx
      * @brief Starts receiving data using the LoRa radio module.
      * @param timeout The timeout, in milliseconds.
      * @return None
      */
     void startRx(uint32_t timeout);

     /**
      * @fn setCadCallback
      * @brief Sets the callback function for when channel activity detection is completed.
      * @param cb The callback function.
      * @return None
      */
     void setCadCallback(OnCadDone cb);

     /**
      * @fn startCad
      * @brief Starts channel activity detection using the LoRa radio module.
      * @param dataRate The data rate, in bits per second.
      * @param cadSymbolNum the number of symbols to be used for channel activity detection operation.
      * @return None
      */
     void startCad(uint8_t dataRate, RadioLoRaCadSymbols_t cadSymbolNum);

     /**
      * @fn radioDeepSleep
      * @brief Puts the LoRa radio module and MCU in deep sleep mode.
      * @return None
      */
     void radioDeepSleep();
     
     /**
      * @fn setEncryptKey
      * @brief Set the encryption key for the radio.
      * @param key Pointer to the encryption key (unsigned 8-bit integer array).
      * @return None
      */
     void setEncryptKey(const uint8_t *key);

     /**
      * @fn dumpRegisters
      * @brief Dump the registers of the radio.
      * @n This function prints the current values of all the registers of the radio to the output (serial monitor)
      * @return None
      */
     void dumpRegisters();

     private:
     eBandwidths_t _bandwidth = BW_125; /**< The bandwidth of the LoRa radio module. */
     uint8_t _txeirp = 16;
     uint8_t _SF = 7;
     protected:
     };
     
     #endif // DFRobot_LoraRadio_h