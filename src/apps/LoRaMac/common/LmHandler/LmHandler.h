/*!
 * \file      LmHandler.h
 *
 * \brief     Implements the LoRaMac layer handling.        实现了Loramac层的处理
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#ifndef __LORAMAC_HANDLER_H__
#define __LORAMAC_HANDLER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LmHandlerTypes.h"
#include "packages/LmhpCompliance.h"

extern const uint8_t txpowerEirpEU868[][2];
extern const uint8_t txpowerEirpUS915[][2];

typedef struct LmHandlerJoinParams_s
{
    CommissioningParams_t *CommissioningParams;
    int8_t Datarate;
    LmHandlerErrorStatus_t Status;
}LmHandlerJoinParams_t;

typedef struct LmHandlerTxParams_s
{
    uint8_t IsMcpsConfirm;
    LoRaMacEventInfoStatus_t Status;
    CommissioningParams_t *CommissioningParams;
    LmHandlerMsgTypes_t MsgType;
    uint8_t AckReceived;
    int8_t Datarate;
    uint32_t UplinkCounter;
    LmHandlerAppData_t AppData;
    int8_t TxPower;
    uint8_t Channel;
}LmHandlerTxParams_t;

typedef struct LmHandlerRxParams_s
{
    uint8_t IsMcpsIndication;
    LoRaMacEventInfoStatus_t Status;
    CommissioningParams_t *CommissioningParams;
    int8_t Datarate;
    int8_t Rssi;
    int8_t Snr;
    uint32_t DownlinkCounter;
    int8_t RxSlot;
    bool IsRevACK;
}LmHandlerRxParams_t;

typedef struct LoRaMAcHandlerBeaconParams_s
{
    LoRaMacEventInfoStatus_t Status;
    LmHandlerBeaconState_t State;
    BeaconInfo_t Info;
}LoRaMAcHandlerBeaconParams_t;

typedef struct LmHandlerParams_s
{
    /*!
     * Region
     */
    LoRaMacRegion_t Region;
    /*!
     * Holds the ADR state
     */
    bool AdrEnable;
    /*!
     * Uplink datarate, when \ref AdrEnable is OFF
     */
    int8_t TxDatarate;
    /*!
     * Enables/Disables a public network usage
     */
    bool PublicNetworkEnable;
    /*!
    * LoRaWAN ETSI duty cycle control enable/disable
    *
    * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
    */
    bool DutyCycleEnabled;
    /*!
     * Application data buffer maximum size
     */
    uint8_t DataBufferMaxSize;
    /*!
     * Application data buffer pointer
     */
    uint8_t *DataBuffer;

    // 发射功率
    uint8_t TxEirp;

    // 入网方式/状态
    ActivationType_t joinType;

    // DEVEUI
    uint8_t *DevEui;

    // JOINEUI
    uint8_t *JoinEui;

    // APPKEY
    uint8_t *AppKey;

    // DEVADDR
    uint32_t DevAddr;

    // APPSKEY
    uint8_t *AppSKey;

    // NWKSKEY
    uint8_t *NwkSKey;

    // 重传次数
    uint8_t NbTrials;

    // 设备类型/工作模式
    DeviceClass_t Class;

}LmHandlerParams_t;

typedef struct LmHandlerCallbacks_s
{
    /*!
     * Get the current battery level        获得当前电池电量
     *
     * \retval value  Battery level ( 0: very low, 254: fully charged )
     */
    uint8_t ( *GetBatteryLevel )( void );
    /*!
     * Get the current temperature          获取当前摄氏度
     *
     * \retval value  Temperature in degree Celsius
     */
    float ( *GetTemperature )( void );
    /*!
     * Returns a pseudo random seed generated using the MCU Unique ID   获取随机种子
     *
     * \retval seed Generated pseudo random seed
     */
    uint32_t ( *GetRandomSeed )( void );
    /*!
     *\brief    Will be called each time a Radio IRQ is handled by the MAC layer.
     *          每次通过MAC层处理无线电IRQ时，都会被调用。
     *
     *\warning  Runs in a IRQ context. Should only change variables state.  在IRQ上下文中运行。只能更改变量状态。
     */
    void ( *OnMacProcess )( void );
    /*!
     * Notifies the upper layer that the NVM context has changed
     *
     * \param [IN] state Indicates if we are storing (true) or
     *                   restoring (false) the NVM context
     *
     * \param [IN] size Number of data bytes which were stored or restored.
     */
    void ( *OnNvmDataChange )( LmHandlerNvmContextStates_t state, uint16_t size );
    /*!
     * Notifies the upper layer that a network parameters have been set     通知上层网络参数设置完成
     *
     * \param [IN] params notification parameters
     */
    void ( *OnNetworkParametersChange )( CommissioningParams_t *params );
    /*!
     * Notifies the upper layer that a MCPS request has been made to the MAC layer
     *
     * \param   [IN] status      - Request returned status
     * \param   [IN] mcpsRequest - Performed MCPS-Request. Refer to \ref McpsReq_t.
     * \param   [IN] nextTxDelay - Time to wait until another TX is possible.
     */
    void ( *OnMacMcpsRequest )( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxDelay );
    /*!
     * Notifies the upper layer that a MLME request has been made to the MAC layer
     *
     * \param   [IN] status      - Request returned status
     * \param   [IN] mlmeRequest - Performed MLME-Request. Refer to \ref MlmeReq_t.
     * \param   [IN] nextTxDelay - Time to wait until another TX is possible.
     */
    void ( *OnMacMlmeRequest )( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxDelay );
    /*!
     * Notifies the upper layer that a network has been joined
     *
     * \param [IN] params notification parameters
     */
    void ( *OnJoinRequest )( LmHandlerJoinParams_t *params );
    /*!
     * Notifies upper layer that a frame has been transmitted                   通知上层一个帧已经传输
     *
     * \param [IN] params notification parameters
     */
    void ( *OnTxData )( LmHandlerTxParams_t *params );
    /*!
     * Notifies the upper layer that an applicative frame has been received     通知上层接收到一个应用帧
     *
     * \param [IN] appData Received applicative data
     * \param [IN] params notification parameters
     */
    void ( *OnRxData )( LmHandlerAppData_t *appData, LmHandlerRxParams_t *params );
    /*!
     * Confirms the LoRaWAN device class change     确认Lorawan设备工作模式更改
     *
     * \param [IN] deviceClass New end-device class     参数为新模式
     */
    void ( *OnClassChange )( DeviceClass_t deviceClass );
    /*!
     * Notifies the upper layer that the beacon status has changed
     *
     * \param [IN] params notification parameters
     */
    void ( *OnBeaconStatusChange )( LoRaMAcHandlerBeaconParams_t *params );
#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
    /*!
     * Notifies the upper layer that the system time has been updated.
     *
     * \param [in] isSynchronized Indicates if the system time is synchronized in the range +/-1 second
     * \param [in] timeCorrection Received time correction value
     */
    void ( *OnSysTimeUpdate )( bool isSynchronized, int32_t timeCorrection );
#else
    /*!
     * Notifies the upper layer that the system time has been updated.
     */
    void ( *OnSysTimeUpdate )( void );
#endif
}LmHandlerCallbacks_t;

/*!
 * LoRaMac handler initialisation   mac层初始化
 *
 * \param [IN] callbacks     LoRaMac handler callbacks
 * \param [IN] handlerParams LoRaMac handler parameters
 *
 * \retval none
 */
LmHandlerErrorStatus_t LmHandlerInit( LmHandlerCallbacks_t *callbacks,
                                      LmHandlerParams_t *handlerParams );

/*!
 * Indicates if the LoRaMacHandler is busy      检测mac层是否繁忙
 * 
 * \retval status [true] Busy, [false] free
 */
bool LmHandlerIsBusy( void );

/*!
 * Processes the LoRaMac and Radio events.      处理Loramac和无线电事件 
 * When no pendig operation asks to go in low power mode.   当没有未完成的操作要求进入低功耗模式
 *
 * \remark This function must be called in the main loop.   这个函数必须在主循环中调用
 */
void LmHandlerProcess( void );

/*!
 * Instructs the MAC layer to send a ClassA uplink      指示MAC层发送CLASS A模式上行数据
 *
 * \param [IN] appData Data to be sent
 * \param [IN] isTxConfirmed Indicates if the uplink requires an acknowledgement
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerSend( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed );

/*!
 * Join a LoRa Network in classA    CLASS A 入网
 *
 * \Note if the device is ABP, this is a pass through function
 */
void LmHandlerJoin( void );

/*!
 * Check whether the Device is joined to the network    检测是否链接到网络
 *
 * \param [IN] none
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
LmHandlerFlagStatus_t LmHandlerJoinStatus( void );

/*!
 * Informs the server on the ping-slot periodicity to use       通知服务器在ping-slot周期性上使用
 *
 * \param [IN] periodicity Is equal to 2^periodicity seconds.
 *                         Example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerPingSlotReq( uint8_t periodicity );

/*!
 * Request the MAC layer to change LoRaWAN class        请求Mac层更改设备工作模式
 *
 * \Note Callback \ref LmHandlerConfirmClass informs upper layer that the change has occurred
 * \Note Only switch from class A to class B/C OR from class B/C to class A is allowed  只允许从A级切换到B/C级或从B/C级切换到A级
 *
 * \param [IN] newClass New class to be requested
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerRequestClass( DeviceClass_t newClass );

/*!
 * Gets the current LoRaWAN class                       获取当前工作模式
 *
 * \retval currentClass Current LoRaWAN class
 */
DeviceClass_t LmHandlerGetCurrentClass( void );

/*!
 * Gets the current datarate                            获取当前设备速率
 *
 * \retval currentDatarate Current datarate
 */
int8_t LmHandlerGetCurrentDatarate( void );

/*!
 * Gets the current active region                       获取当前活动地区
 *
 * \retval currentRegion Current active region
 */
LoRaMacRegion_t LmHandlerGetActiveRegion( void );

/*!
 * Set system maximum tolerated rx error in milliseconds        设置系统最大耐受性RX错误中的毫秒误差
 *
 * \param [IN] maxErrorInMs Maximum tolerated error in milliseconds
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs );

/*
 *=============================================================================
 * PACKAGES HANDLING        包装处理
 *=============================================================================
 */
LmHandlerErrorStatus_t LmHandlerPackageRegister( uint8_t id, void *params );
bool LmHandlerPackageIsInitialized( uint8_t id );
bool LmHandlerPackageIsRunning( uint8_t id );

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_HANDLER_H__
