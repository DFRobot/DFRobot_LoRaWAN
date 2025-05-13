/*!
 * \file      radio.h
 *
 * \brief     Radio driver API definition
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __RADIO_H__
#define __RADIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/*!
 * Radio driver supported modems
 */
typedef enum
{
    MODEM_FSK = 0,
    MODEM_LORA,
}RadioModems_t;

/*!
 * Radio driver internal state machine states definition    天线驱动程序内部状态机状态定义
 */
typedef enum
{
    RF_IDLE = 0,   //!< The radio is idle                               天线空闲
    RF_RX_RUNNING, //!< The radio is in reception state                 接收状态
    RF_TX_RUNNING, //!< The radio is in transmission state              发送状态
    RF_CAD,        //!< The radio is doing channel activity detection   正在进行信道活动探测
}RadioState_t;

/*!
 * \brief Radio driver callback functions   天线驱动回调函数
 */
typedef struct
{
    /*!
     * \brief  Tx Done callback prototype.  发送完成回调原型
     */
    void    ( *TxDone )( void );
    /*!
     * \brief  Tx Timeout callback prototype.   发送超时回调原型
     */
    void    ( *TxTimeout )( void );
    /*!
     * \brief Rx Done callback prototype.       接收完成回调原型
     *
     * \param [IN] payload Received buffer pointer    接收数据buff头指针
     * \param [IN] size    Received buffer size       接收数据尺寸
     * \param [IN] rssi    RSSI value computed while receiving the frame [dBm]      信号强度
     * \param [IN] snr     SNR value computed while receiving the frame [dB]        信噪比
     *                     FSK : N/A ( set to 0 )
     *                     LoRa: SNR value in dB
     */
    void    ( *RxDone )( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
    /*!
     * \brief  Rx Timeout callback prototype.       接收超时回调原型
     */
    void    ( *RxTimeout )( void );
    /*!
     * \brief Rx Error callback prototype.          接收错误回调原型
     */
    void    ( *RxError )( void );
    /*!
     * \brief  FHSS Change Channel callback prototype.      FHSS更改频道回调原型
     *
     * \param [IN] currentChannel   Index number of the current channel     参数为当前频道索引
     */
    void ( *FhssChangeChannel )( uint8_t currentChannel );

    /*!
     * \brief CAD Done callback prototype.      CAD回调原型
     *
     * \param [IN] channelDetected    Channel Activity detected during the CAD
     */
    void ( *CadDone ) ( bool channelActivityDetected );
    
    /*!
     * \brief  Gnss Done Done callback prototype.       卫星操作完成回调
    */
    void    ( *GnssDone )( void );
    
    /*!
     * \brief  Gnss Done Done callback prototype.       wifi操作完成回调
    */
    void    ( *WifiDone )( void );

    /*!
     * \brief Preamble detected callback prototype.     前导码检测回调原型
     */
	void (*PreAmpDetect)(void);
}RadioEvents_t;

/*!
 * \brief Radio driver definition       天线驱动定义
 */
struct Radio_s
{
    /*!
     * \brief Initializes the radio     天线初始化  参数为天线驱动回调
     *
     * \param [IN] events Structure containing the driver callback functions
     */
    void    ( *Init )( RadioEvents_t *events );
    /*!
     * Return current radio status      获取当前天线状态
     *
     * \param status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
     */
    RadioState_t ( *GetStatus )( void );
    /*!
     * \brief Configures the radio with the given modem     设置天线调制模式：0为FSK 1为LORA
     *
     * \param [IN] modem Modem to be used [0: FSK, 1: LoRa]
     */
    void    ( *SetModem )( RadioModems_t modem );
    /*!
     * \brief Sets the channel frequency        设置频道频率
     *
     * \param [IN] freq         Channel RF frequency
     */
    void    ( *SetChannel )( uint32_t freq );
    /*!
     * \brief Checks if the channel is free for the given time      检测该频道在给定时间内是否空闲
     *
     * \remark The FSK modem is always used for this task as we can select the Rx bandwidth at will.
     *
     * \param [IN] freq                Channel RF frequency in Hertz    射频频率
     * \param [IN] rxBandwidth         Rx bandwidth in Hertz            Rx带宽
     * \param [IN] rssiThresh          RSSI threshold in dBm            RSSI阈值        
     * \param [IN] maxCarrierSenseTime Max time in milliseconds while the RSSI is measured  测量RSSI时的最大时间（毫秒）
     *
     * \retval isFree         [true: Channel is free, false: Channel is not free]
     */
    bool    ( *IsChannelFree )( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime );
    /*!
     * \brief Generates a 32 bits random value based on the RSSI readings       根据RSSI读数生成32位随机值
     *
     * \remark This function sets the radio in LoRa modem mode and disables
     *         all interrupts.
     *         After calling this function either Radio.SetRxConfig or
     *         Radio.SetTxConfig functions must be called.
     * 
     *          该功能将射频设置为LoRa modem模式，并禁用所有中断。调用此函数后，必须调用Radio.SetRxConfig函数或Radio.SetTxConfig
     *
     * \retval randomValue    32 bits random value
     */
    uint32_t ( *Random )( void );
    /*!
     * \brief Sets the reception parameters     设置接收参数
     *
     * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]                        参数1 modem 天线调制模式
     * \param [IN] bandwidth    Sets the bandwidth                                              参数2 bandwidth 带宽
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [IN] datarate     Sets the Datarate                                               参数3 datarate 速率 实际是扩频因子SF
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [IN] coderate     Sets the coding rate (LoRa only)                                参数4 coderate 码率 (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [IN] bandwidthAfc Sets the AFC Bandwidth (FSK only)                               参数5 bandwidthAfc AFC带宽 (FSK only) ( lora set to 0 )
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: N/A ( set to 0 )
     * \param [IN] preambleLen  Sets the Preamble length                                        参数6 preambleLen 前导码长度 
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [IN] symbTimeout  Sets the RxSingle timeout value                                 参数7 symbTimeout 码元超时时间
     *                          FSK : timeout in number of bytes
     *                          LoRa: timeout in symbols
     * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]                    参数8 fixlen 固定长度报文 (0:可变  1:固定)
     * \param [IN] payloadLen   Sets payload length when fixed length is used                   参数9 payloadLen 固定报文长度时，报文的长度
     * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]                        参数10 crcOn 是否启用CRC校验        
     * \param [IN] freqHopOn    Enables disables the intra-packet frequency hopping             参数11 freqHopOn 是否启用报文内跳频功能
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [IN] hopPeriod    Number of symbols between each hop                              参数12 hopPeriod 每一跳之间的码元数
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [IN] iqInverted   Inverts IQ signals (LoRa only)                                  参数13 iqInverted 是否反转中断信号
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [IN] rxContinuous Sets the reception in continuous mode                           参数14 rxContinuous 设置是否为连续接收模式
     *                          [false: single mode, true: continuous mode]
     */
    void    ( *SetRxConfig )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                              bool iqInverted, bool rxContinuous );
    /*!
     * \brief Sets the transmission parameters
     *
     * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]                        参数1 modem 天线调制模式
     * \param [IN] power        Sets the output power [dBm]                                     参数2 power 物理发射功率
     * \param [IN] fdev         Sets the frequency deviation (FSK only)                         参数3 fdev 设置频率偏差 (FSK only) ( lora set to 0 )
     *                          FSK : [Hz]
     *                          LoRa: 0
     * \param [IN] bandwidth    Sets the bandwidth (LoRa only)                                  参数4 bandwidth 带宽
     *                          FSK : 0
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [IN] datarate     Sets the Datarate                                               参数5 datarate 速率 实际是扩频因子SF
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [IN] coderate     Sets the coding rate (LoRa only)                                参数6 coderate  (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [IN] preambleLen  Sets the preamble length                                        参数7 preambleLen 前导码长度
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]                    参数8 fixlen 固定长度报文 (0:可变  1:固定)
     * \param [IN] crcOn        Enables disables the CRC [0: OFF, 1: ON]                        参数9 crcOn 是否启用CRC校验
     * \param [IN] freqHopOn    Enables disables the intra-packet frequency hopping             参数10 freqHopOn 是否启用报文内跳频功能
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [IN] hopPeriod    Number of symbols between each hop                              参数11 hopPeriod 每一跳之间的码元数                      
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [IN] iqInverted   Inverts IQ signals (LoRa only)                                  参数12 iqInverted 是否反转中断信号
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [IN] timeout      Transmission timeout [ms]                                       参数13 timeout 发送超时时长
     */
    void    ( *SetTxConfig )( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool freqHopOn,
                              uint8_t hopPeriod, bool iqInverted, uint32_t timeout );
    /*!
     * \brief Checks if the given RF frequency is supported by the hardware                     检查硬件是否支持给定的射频频率                
     *
     * \param [IN] frequency RF frequency to be checked
     * \retval isSupported [true: supported, false: unsupported]
     */
    bool    ( *CheckRfFrequency )( uint32_t frequency );
    /*!
     * \brief Computes the packet time on air in ms for the given payload                       计算给定负载的空中数据包时间（以毫秒为单位）
     *
     * \Remark Can only be called once SetRxConfig or SetTxConfig have been called              只能在SetRxConfig或SetTxConfig被调用后调用            
     *
     * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]                          参数1 modem 天线调制模式
     * \param [IN] bandwidth    Sets the bandwidth                                              参数2 bandwidth 带宽
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [IN] datarate     Sets the Datarate                                               参数3 datarate 速率 实际是扩频因子SF
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [IN] coderate     Sets the coding rate (LoRa only)                                参数4 coderate 码率 (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [IN] preambleLen  Sets the Preamble length                                        参数5 preambleLen 前导码长度
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]                    参数6 fixlen 固定长度报文 (0:可变  1:固定)
     * \param [IN] payloadLen   Sets payload length when fixed length is used                   参数7 payloadLen 固定报文长度时，报文的长度
     * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]                        参数8 crcOn 是否启用CRC校验
     *
     * \retval airTime        Computed airTime (ms) for the given packet payload length
     */
    uint32_t  ( *TimeOnAir )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn );
    /*!
     * \brief Sends the buffer of size. Prepares the packet to be sent and sets                 发送指定大小数据，准备要发送的数据包并在传输中设置无线电
     *        the radio in transmission
     *
     * \param [IN]: buffer     Buffer pointer
     * \param [IN]: size       Buffer size
     */
    void    ( *Send )( uint8_t *buffer, uint8_t size );
    /*!
     * \brief Sets the radio in sleep mode                                                      设置天线进入睡眠状态   
     */
    void    ( *Sleep )( void );
    /*!
     * \brief Sets the radio in standby mode                                                    设置天线进入预备状态
     */
    void    ( *Standby )( void );
    /*!
     * \brief Sets the radio in reception mode for the given time                               在给定时间将天线设置为接收模式
     * \param [IN] timeout Reception timeout [ms]
     *                     [0: continuous, others timeout]
     */
    void    ( *Rx )( uint32_t timeout );
    /*!
     * \brief Start a Channel Activity Detection                                                启动通道活动检测                                               
     */
    void    ( *StartCad )( void );
    /*!
     * \brief Sets the radio in continuous wave transmission mode                               设置为连续传输模式
     *
     * \param [IN]: freq       Channel RF frequency
     * \param [IN]: power      Sets the output power [dBm]
     * \param [IN]: time       Transmission mode timeout [s]
     */
    void    ( *SetTxContinuousWave )( uint32_t freq, int8_t power, uint16_t time );
    /*!
     * \brief Reads the current RSSI value                                                      获取当前RSSI值
     *
     * \retval rssiValue Current RSSI value in [dBm]
     */
    int16_t ( *Rssi )( RadioModems_t modem );
    /*!
     * \brief Writes the radio register at the specified address                                向指定寄存器写数据
     *
     * \param [IN]: addr Register address
     * \param [IN]: data New register value
     */
    void    ( *Write )( uint32_t addr, uint8_t data );
    /*!
     * \brief Reads the radio register at the specified address                                 从指定寄存器地址读数据
     *
     * \param [IN]: addr Register address
     * \retval data Register value
     */
    uint8_t ( *Read )( uint32_t addr );
    /*!
     * \brief Writes multiple radio registers starting at address                               写多个寄存器
     *
     * \param [IN] addr   First Radio register address
     * \param [IN] buffer Buffer containing the new register's values
     * \param [IN] size   Number of registers to be written
     */
    void    ( *WriteBuffer )( uint32_t addr, uint8_t *buffer, uint8_t size );
    /*!
     * \brief Reads multiple radio registers starting at address
     *
     * \param [IN] addr First Radio register address
     * \param [OUT] buffer Buffer where to copy the registers data
     * \param [IN] size Number of registers to be read
     */
    void    ( *ReadBuffer )( uint32_t addr, uint8_t *buffer, uint8_t size );
    /*!
     * \brief Sets the maximum payload length.                                                  设置最大有效载荷长度
     *
     * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
     * \param [IN] max        Maximum payload length in bytes
     */
    void    ( *SetMaxPayloadLength )( RadioModems_t modem, uint8_t max );
    /*!
     * \brief Sets the network to public or private. Updates the sync byte.                     设置是否为公共网络，同时更新同步字
     *
     * \remark Applies to LoRa modem only
     *
     * \param [IN] enable if true, it enables a public network
     */
    void    ( *SetPublicNetwork )( bool enable );
    /*!
     * \brief Gets the time required for the board plus radio to get out of sleep.[ms]          获得唤醒需要的实际
     *
     * \retval time Radio plus board wakeup time in ms.
     */
    uint32_t  ( *GetWakeupTime )( void );
    /*!
     * \brief Process radio irq                                                                 处理IRQ中断
     */
    void ( *IrqProcess )( void );
    /*
     * The next functions are available only on SX126x radios.
     */
    /*!
     * \brief Sets the radio in reception mode with Max LNA gain for the given time
     设置无线电在接收模式与最大LNA增益给定时间
     *
     * \remark Available on SX126x radios only.
     *
     * \param [IN] timeout Reception timeout [ms]
     *                     [0: continuous, others timeout]
     */
    void    ( *RxBoosted )( uint32_t timeout );
    /*!
     * \brief Sets the Rx duty cycle management parameters                                  设置接收占空比相关参数
     *
     * \remark Available on SX126x radios only.
     *
     * \param [in]  rxTime        Structure describing reception timeout value
     * \param [in]  sleepTime     Structure describing sleep timeout value
     */
    void ( *SetRxDutyCycle ) ( uint32_t rxTime, uint32_t sleepTime );
	/*!
     * \brief Process radio irq in background task (nRF52 & ESP32)
     */
	void (*BgIrqProcess)(void);

    // 以下函数为兼容DFRobot_LoRaRadio增加      mating

    /*!
     * \brief Re-Initializes the radio after CPU wakeup from deep sleep
     *
     * \param  events Structure containing the driver callback functions
     */
	void (*ReInit)(RadioEvents_t *events);
    /*!
     * \brief Set Channel Activity Detection parameters 设置通道活动检测参数
     */
	void (*SetCadParams)(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, uint8_t cadExitMode, uint32_t cadTimeout);
	/*
      * \brief Process radio irq after CPU wakeup from deep sleep
     */
	void (*IrqProcessAfterDeepSleep)(void);
	
};

/*!
 * \brief Radio driver
 *
 * \remark This variable is defined and initialized in the specific radio
 *         board implementation
 */
extern const struct Radio_s Radio;

extern const struct Radio_s Radio2;

void reInitEvent(RadioEvents_t *events);

#ifdef __cplusplus
}
#endif

#endif // __RADIO_H__
