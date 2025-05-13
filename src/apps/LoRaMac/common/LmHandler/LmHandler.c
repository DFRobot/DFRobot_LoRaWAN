/*!
 * \file      LmHandler.c
 *
 * \brief     Implements the LoRaMac layer handling.
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system/utilities.h"
#include "boards/mcu/timer.h"
#include "apps/LoRaMac/common/Commissioning.h"
#include "apps/LoRaMac/common/NvmDataMgmt.h"
#include "radio/radio.h"
#include "LmHandler.h"
#include "packages/LmhPackage.h"
#include "packages/LmhpCompliance.h"
#include "packages/LmhpClockSync.h"
#include "packages/LmhpRemoteMcastSetup.h"
#include "packages/LmhpFragmentation.h"

// #ifndef ACTIVE_REGION

// #warning "No active region defined, LORAMAC_REGION_EU868 will be used as default."

// #define ACTIVE_REGION LORAMAC_REGION_EU868

// #endif

#include "mac/LoRaMacTest.h"
#include <Arduino.h>

RTC_DATA_ATTR static CommissioningParams_t CommissioningParams =
    {
        .IsOtaaActivation = false,      // 默认是ABP，OTAA激活后会置为true
        .DevEui = {0},  // Automatically filed from secure-element  自动从安全元素提交
        .JoinEui = {0}, // Automatically filed from secure-element
        .SePin = {0},   // Automatically filed from secure-element
        .NetworkId = LORAWAN_NETWORK_ID,
        .DevAddr = LORAWAN_DEVICE_ADDRESS,
};

RTC_DATA_ATTR static LmhPackage_t *LmHandlerPackages[PKG_MAX_NUMBER];

/*!
 * Upper layer LoRaMac parameters
 */
RTC_DATA_ATTR static LmHandlerParams_t *LmHandlerParams;

/*!
 * Upper layer callbacks
 */
RTC_DATA_ATTR static LmHandlerCallbacks_t *LmHandlerCallbacks;

/*!
 * Used to notify LmHandler of LoRaMac events
 */
RTC_DATA_ATTR static LoRaMacPrimitives_t LoRaMacPrimitives;

/*!
 * LoRaMac callbacks
 */
RTC_DATA_ATTR static LoRaMacCallback_t LoRaMacCallbacks;

RTC_DATA_ATTR static LmHandlerJoinParams_t JoinParams =
    {
        .CommissioningParams = &CommissioningParams,
        .Datarate = DR_0,
        .Status = LORAMAC_HANDLER_ERROR};

RTC_DATA_ATTR static LmHandlerTxParams_t TxParams =
    {
        .CommissioningParams = &CommissioningParams,
        .MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG,
        .AckReceived = 0,
        .Datarate = DR_0,
        .UplinkCounter = 0,
        .AppData =
            {
                .Port = 0,
                .BufferSize = 0,
                .Buffer = NULL},
        .TxPower = TX_POWER_0,
        .Channel = 0};

RTC_DATA_ATTR static LmHandlerRxParams_t RxParams =
    {
        .CommissioningParams = &CommissioningParams,
        .Rssi = 0,
        .Snr = 0,
        .DownlinkCounter = 0,
        .RxSlot = -1,
        .IsRevACK = false};

RTC_DATA_ATTR static LoRaMAcHandlerBeaconParams_t BeaconParams =
    {
        .State = LORAMAC_HANDLER_BEACON_ACQUIRING,
        .Info =
            {
                .Time = {.Seconds = 0, .SubSeconds = 0},
                .Frequency = 0,
                .Datarate = 0,
                .Rssi = 0,
                .Snr = 0,
                .GwSpecific =
                    {
                        .InfoDesc = 0,
                        .Info = {0}}}};

/*!
 * Indicates if a switch to Class B operation is pending or not.
 *
 * TODO: Create a new structure to store the current handler states/status
 *       and add the below variable to it.
 */
RTC_DATA_ATTR static bool IsClassBSwitchPending = false;


// 协议栈输出功率下标对应设置功率
const uint8_t txpowerEirpEU868[][2] = {
        {TX_POWER_0, 16},
        {TX_POWER_1, 14},
        {TX_POWER_2, 12},
        {TX_POWER_3, 10},
        {TX_POWER_4, 8},
        {TX_POWER_5, 6},
        {TX_POWER_6, 4},
        {TX_POWER_7, 2}
};

const uint8_t txpowerEirpUS915[][2] = {
        {TX_POWER_4, 22},
        {TX_POWER_5, 20},
        {TX_POWER_6, 18},
        {TX_POWER_7, 16},
        {TX_POWER_8, 14},
        {TX_POWER_9, 12},
        {TX_POWER_10, 10},
        {TX_POWER_11, 8},
        {TX_POWER_12, 6},
        {TX_POWER_13, 4},
        {TX_POWER_14, 2}
};

// 获取地区索引数组的行数
static uint8_t getTxpowerEirpRowNum(LoRaMacRegion_t region)
{
    switch (region)
    {
    case LORAMAC_REGION_EU868:
        return (sizeof(txpowerEirpEU868) / sizeof(txpowerEirpEU868[0]));
        break;

    case LORAMAC_REGION_US915:
        return (sizeof(txpowerEirpUS915) / sizeof(txpowerEirpUS915[0]));
        break;
    default:
        return 0;
        break;
    }
}

// 检查输入发射功率是否合法并获取TXpower
static bool isEirpValid(const uint8_t (*txpowerEirp)[2], uint8_t row, uint8_t eripS, uint8_t *index)
{
    for (int i = 0; i < row; i++)
    {
        if (txpowerEirp[i][1] == eripS)
        {
            // printf("txpowerEirp[i][0] = %d, txpowerEirp[i][1] = %d", txpowerEirp[i][0], txpowerEirp[i][1]);
            *index = txpowerEirp[i][0];
            return true;
        }
    }
    *index = TX_POWER_0;
    return false;
}



/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *                             containing confirm attributes.
 */
static void McpsConfirm(McpsConfirm_t *mcpsConfirm);

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication(McpsIndication_t *mcpsIndication);

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm);

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void MlmeIndication(MlmeIndication_t *mlmeIndication);

/*!
 * Requests network server time update
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
static LmHandlerErrorStatus_t LmHandlerDeviceTimeReq(void);

/*!
 * Starts the beacon search
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
static LmHandlerErrorStatus_t LmHandlerBeaconReq(void);

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */
typedef enum PackageNotifyTypes_e
{
    PACKAGE_MCPS_CONFIRM,
    PACKAGE_MCPS_INDICATION,
    PACKAGE_MLME_CONFIRM,
    PACKAGE_MLME_INDICATION,
} PackageNotifyTypes_t;

/*!
 * Notifies the package to process the LoRaMac callbacks.
 *
 * \param [IN] notifyType MAC notification type [PACKAGE_MCPS_CONFIRM,
 *                                               PACKAGE_MCPS_INDICATION,
 *                                               PACKAGE_MLME_CONFIRM,
 *                                               PACKAGE_MLME_INDICATION]
 * \param[IN] params      Notification parameters. The params type can be
 *                        [McpsConfirm_t, McpsIndication_t, MlmeConfirm_t, MlmeIndication_t]
 */
static void LmHandlerPackagesNotify(PackageNotifyTypes_t notifyType, void *params);

static void LmHandlerPackagesProcess(void);

LmHandlerErrorStatus_t LmHandlerInit(LmHandlerCallbacks_t *handlerCallbacks, LmHandlerParams_t *handlerParams)
{

    MibRequestConfirm_t mibReq;
    LmHandlerParams = handlerParams;
    LmHandlerCallbacks = handlerCallbacks;

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
    LoRaMacCallbacks.GetBatteryLevel = LmHandlerCallbacks->GetBatteryLevel;
    LoRaMacCallbacks.GetTemperatureLevel = LmHandlerCallbacks->GetTemperature;
    LoRaMacCallbacks.NvmDataChange = NvmDataMgmtEvent;
    LoRaMacCallbacks.MacProcessNotify = LmHandlerCallbacks->OnMacProcess;

    IsClassBSwitchPending = false;

    if (LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LmHandlerParams->Region) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    LoRaMacMibGetRequestConfirm(&mibReq);
    if (mibReq.Param.NetworkActivation != ACTIVATION_TYPE_NONE)     // 如果已经入网就不必设置以下参数
    {
        return LORAMAC_HANDLER_SUCCESS;
    }


    // set
    // 公共网络
    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = LmHandlerParams->PublicNetworkEnable;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // ADR
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LmHandlerParams->AdrEnable;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 入网方式
    // mibReq.Type = MIB_NETWORK_ACTIVATION;
    // if (LmHandlerParams->joinType == ACTIVATION_TYPE_OTAA)
    // {
    //     mibReq.Param.NetworkActivation = ACTIVATION_TYPE_NONE;      // 不能在这设为OTAA，因为入网成功后会自动设为OTAA
    // }
    // else
    // {
    //     mibReq.Param.NetworkActivation = LmHandlerParams->joinType;
    // }
    // LoRaMacMibSetRequestConfirm(&mibReq);

    // devEui
    mibReq.Type = MIB_DEV_EUI;
    mibReq.Param.DevEui = LmHandlerParams->DevEui;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // joinEui
    mibReq.Type = MIB_JOIN_EUI;
    mibReq.Param.JoinEui = LmHandlerParams->JoinEui;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // appkey
    mibReq.Type = MIB_NWK_KEY;
    mibReq.Param.AppKey = LmHandlerParams->AppKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // netid
    // mibReq.Type = MIB_NET_ID;
    // mibReq.Param.NetID = LORAWAN_NETWORK_ID;
    // LoRaMacMibSetRequestConfirm(&mibReq);

    // devaddr
    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = LmHandlerParams->DevAddr;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // appskey
    mibReq.Type = MIB_APP_S_KEY;
    mibReq.Param.AppSKey = LmHandlerParams->AppSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // nwkskey
    mibReq.Type = MIB_F_NWK_S_INT_KEY;
    mibReq.Param.FNwkSIntKey = LmHandlerParams->NwkSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mibReq.Type = MIB_S_NWK_S_INT_KEY;
    mibReq.Param.SNwkSIntKey = LmHandlerParams->NwkSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);
    mibReq.Type = MIB_NWK_S_ENC_KEY;
    mibReq.Param.NwkSEncKey = LmHandlerParams->NwkSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 速率
    mibReq.Type = MIB_CHANNELS_DATARATE;
    mibReq.Param.ChannelsDatarate = LmHandlerParams->TxDatarate;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 天线增益
    mibReq.Type = MIB_ANTENNA_GAIN;
    mibReq.Param.AntennaGain = 3;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 发射功率
    mibReq.Type = MIB_CHANNELS_TX_POWER;
    uint8_t index = 0;
    switch(LmHandlerParams->Region)
    {
    case LORAMAC_REGION_US915:
        if(isEirpValid(txpowerEirpUS915, getTxpowerEirpRowNum(LORAMAC_REGION_US915), LmHandlerParams->TxEirp, &index))
        {
            mibReq.Param.ChannelsTxPower = index;   //(maxEirp - dtuConfig.loraWanPara.eirp - antennaGain)/2;
        } 
        else 
        {
            mibReq.Param.ChannelsTxPower = TX_POWER_0;
        }
        break;
    case LORAMAC_REGION_EU868:
        if(isEirpValid(txpowerEirpEU868, getTxpowerEirpRowNum(LORAMAC_REGION_EU868), LmHandlerParams->TxEirp, &index)) 
        {
            mibReq.Param.ChannelsTxPower = index;   //(maxEirp - dtuConfig.loraWanPara.eirp - antennaGain)/2;
        } 
        else 
        {
            mibReq.Param.ChannelsTxPower = TX_POWER_0;
        }
        break;
    default:
        break;
    }
    LoRaMacMibSetRequestConfirm(&mibReq);
    mibReq.Type = MIB_CHANNELS_DEFAULT_TX_POWER;
    mibReq.Param.ChannelsDefaultTxPower = index;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 重传次数
    mibReq.Type = MIB_CHANNELS_NB_TRANS;
    mibReq.Param.ChannelsNbTrans = LmHandlerParams->NbTrials;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // 设备类型
    mibReq.Type = MIB_DEVICE_CLASS;
    mibReq.Param.Class = LmHandlerParams->Class;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // get
    // Read secure-element DEV_EUI, JOI_EUI and SE_PIN values.      获取安全元素：DEVEUI、JOINEUI和硬件加密引脚
    mibReq.Type = MIB_DEV_EUI;
    LoRaMacMibGetRequestConfirm(&mibReq);
    memcpy1(CommissioningParams.DevEui, mibReq.Param.DevEui, 8);

    mibReq.Type = MIB_JOIN_EUI;
    LoRaMacMibGetRequestConfirm(&mibReq);
    memcpy1(CommissioningParams.JoinEui, mibReq.Param.JoinEui, 8);

    mibReq.Type = MIB_SE_PIN;
    LoRaMacMibGetRequestConfirm(&mibReq);
    memcpy1(CommissioningParams.SePin, mibReq.Param.SePin, 4);

    LoRaMacTestSetDutyCycleOn(LmHandlerParams->DutyCycleEnabled);
    LoRaMacStart();

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    if (LoRaMacMibGetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
    {
        if (mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)     // 如果没入网，则通知应用层网络参数设置完成（初始化完成）
        {
            LmHandlerCallbacks->OnNetworkParametersChange(&CommissioningParams);
        }
    }
    return LORAMAC_HANDLER_SUCCESS;
}

bool LmHandlerIsBusy(void)
{
    if (LoRaMacIsBusy() == true)
    {
        return true;
    }
    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        // The network isn't yet joined, try again later.
        LmHandlerJoin();
        return true;
    }

    if (LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning() == true)
    {
        return true;
    }
    return false;
}

void LmHandlerProcess(void)
{
    // uint16_t size = 0;

    // Process Radio IRQ
    // if (Radio.IrqProcess != NULL)
    // {
    //     Radio.IrqProcess();
    // }
    
    if(Radio.BgIrqProcess != NULL)
    {
        Radio.BgIrqProcess();
    }    

    // Processes the LoRaMac events
    LoRaMacProcess();

    // Call all packages process functions
    LmHandlerPackagesProcess();

    // Store to NVM if required
    // size = NvmDataMgmtStore();

    // if (size > 0)
    // {
    //     LmHandlerCallbacks->OnNvmDataChange(LORAMAC_HANDLER_NVM_STORE, size);
    // }
}

/*!
 * Join a LoRa Network in classA
 *
 * \Note if the device is ABP, this is a pass through function
 *
 * \param [IN] isOtaa Indicates which activation mode must be used
 */
static void LmHandlerJoinRequest(bool isOtaa)
{
    // printf("\n\n\n------------LmHandlerJoinRequest-------------\n\n");
    if (isOtaa == true)     // OTAA入网激活流程
    {
        MlmeReq_t mlmeReq;

        mlmeReq.Type = MLME_JOIN;
        mlmeReq.Req.Join.Datarate = DR_0;   // 入网包默认DR0发送
        // Update commissioning parameters activation type variable.
        CommissioningParams.IsOtaaActivation = true;

        // 启动OTAA连接过程
        LmHandlerCallbacks->OnMacMlmeRequest(LoRaMacMlmeRequest(&mlmeReq), &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime);
    }
    else                    // ABP入网，不需要调用MAC层函数，直接通知应用层入网成功
    {
        MibRequestConfirm_t mibReq;
        CommissioningParams.IsOtaaActivation = false;
        LmHandlerJoinParams_t joinParams =
            {
                .CommissioningParams = &CommissioningParams,
                .Datarate = LmHandlerParams->TxDatarate,
                .Status = LORAMAC_HANDLER_SUCCESS
            };

        mibReq.Type = MIB_NETWORK_ACTIVATION;
        mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
        LoRaMacMibSetRequestConfirm(&mibReq);

        // Notify upper layer
        LmHandlerCallbacks->OnJoinRequest(&joinParams);
    }
}

void LmHandlerJoin(void)
{
    if(LmHandlerParams->joinType == ACTIVATION_TYPE_OTAA)
    {
        LmHandlerJoinRequest(true);
    }
    else if(LmHandlerParams->joinType == ACTIVATION_TYPE_ABP)
    {
        LmHandlerJoinRequest(false);
    }    
}

LmHandlerFlagStatus_t LmHandlerJoinStatus(void)
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm(&mibReq);

    if (status == LORAMAC_STATUS_OK)
    {
        if (mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)
        {
            return LORAMAC_HANDLER_RESET;
        }
        else
        {
            return LORAMAC_HANDLER_SET;
        }
    }
    else
    {
        return LORAMAC_HANDLER_RESET;
    }
}

LmHandlerErrorStatus_t LmHandlerSend(LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed)
{
    LoRaMacStatus_t status;
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
    {
        // The network isn't joined, try again.
        LmHandlerJoinRequest(CommissioningParams.IsOtaaActivation);
        return LORAMAC_HANDLER_ERROR;
    }

    if ((LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning() == true) && (appData->Port != LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->Port) && (appData->Port != 0))
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mcpsReq.Req.Unconfirmed.Datarate = LmHandlerParams->TxDatarate;
    if (LoRaMacQueryTxPossible(appData->BufferSize, &txInfo) != LORAMAC_STATUS_OK)
    {
        // Send empty frame in order to flush MAC commands
        TxParams.MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG;
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    }
    else
    {
        TxParams.MsgType = isTxConfirmed;
        mcpsReq.Req.Unconfirmed.fPort = appData->Port;
        mcpsReq.Req.Unconfirmed.fBufferSize = appData->BufferSize;
        mcpsReq.Req.Unconfirmed.fBuffer = appData->Buffer;
        if (isTxConfirmed == LORAMAC_HANDLER_UNCONFIRMED_MSG)
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.NbTrials = 8;
        }
    }

    TxParams.AppData = *appData;
    TxParams.Datarate = LmHandlerParams->TxDatarate;

    status = LoRaMacMcpsRequest(&mcpsReq);
    LmHandlerCallbacks->OnMacMcpsRequest(status, &mcpsReq, mcpsReq.ReqReturn.DutyCycleWaitTime);

    if (status == LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static LmHandlerErrorStatus_t LmHandlerDeviceTimeReq(void)
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_DEVICE_TIME;

    status = LoRaMacMlmeRequest(&mlmeReq);
    LmHandlerCallbacks->OnMacMlmeRequest(status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime);

    if (status == LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static LmHandlerErrorStatus_t LmHandlerBeaconReq(void)
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_BEACON_ACQUISITION;

    status = LoRaMacMlmeRequest(&mlmeReq);
    LmHandlerCallbacks->OnMacMlmeRequest(status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime);

    if (status == LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerPingSlotReq(uint8_t periodicity)
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_PING_SLOT_INFO;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = periodicity;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

    status = LoRaMacMlmeRequest(&mlmeReq);
    LmHandlerCallbacks->OnMacMlmeRequest(status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime);

    if (status == LORAMAC_STATUS_OK)
    {
        // Send an empty message
        LmHandlerAppData_t appData =
            {
                .Buffer = NULL,
                .BufferSize = 0,
                .Port = 0};
        return LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG);
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerRequestClass(DeviceClass_t newClass)
{
    MibRequestConfirm_t mibReq;
    DeviceClass_t currentClass;
    LmHandlerErrorStatus_t errorStatus = LORAMAC_HANDLER_SUCCESS;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm(&mibReq);
    currentClass = mibReq.Param.Class;

    // Attempt to switch only if class update
    if (currentClass != newClass)
    {
        switch (newClass)
        {
        case CLASS_A:
        {
            if (currentClass != CLASS_A)
            {
                mibReq.Param.Class = CLASS_A;
                if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
                {
                    // Switch is instantaneous
                    LmHandlerCallbacks->OnClassChange(CLASS_A);
                }
                else
                {
                    errorStatus = LORAMAC_HANDLER_ERROR;
                }
            }
        }
        break;
        case CLASS_B:
        {
            if (currentClass != CLASS_A)
            {
                errorStatus = LORAMAC_HANDLER_ERROR;
            }
            // Beacon must first be acquired
            errorStatus = LmHandlerDeviceTimeReq();
            IsClassBSwitchPending = true;
        }
        break;
        case CLASS_C:
        {
            if (currentClass != CLASS_A)
            {
                errorStatus = LORAMAC_HANDLER_ERROR;
            }
            // Switch is instantaneous
            mibReq.Param.Class = CLASS_C;
            if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
            {
                LmHandlerCallbacks->OnClassChange(CLASS_C);
            }
            else
            {
                errorStatus = LORAMAC_HANDLER_ERROR;
            }
        }
        break;
        default:
            break;
        }
    }
    return errorStatus;
}

DeviceClass_t LmHandlerGetCurrentClass(void)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm(&mibReq);

    return mibReq.Param.Class;
}

int8_t LmHandlerGetCurrentDatarate(void)
{
    MibRequestConfirm_t mibGet;

    mibGet.Type = MIB_CHANNELS_DATARATE;
    LoRaMacMibGetRequestConfirm(&mibGet);

    return mibGet.Param.ChannelsDatarate;
}

LoRaMacRegion_t LmHandlerGetActiveRegion(void)
{
    return LmHandlerParams->Region;
}

LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError(uint32_t maxErrorInMs)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
    mibReq.Param.SystemMaxRxError = maxErrorInMs;
    if (LoRaMacMibSetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

/*
 *=============================================================================
 * LORAMAC NOTIFICATIONS HANDLING   Loramac通知处理
 *=============================================================================
 */

static void McpsConfirm(McpsConfirm_t *mcpsConfirm)
{
    TxParams.IsMcpsConfirm = 1;
    TxParams.Status = mcpsConfirm->Status;
    TxParams.Datarate = mcpsConfirm->Datarate;
    TxParams.UplinkCounter = mcpsConfirm->UpLinkCounter;
    TxParams.TxPower = mcpsConfirm->TxPower;
    TxParams.Channel = mcpsConfirm->Channel;
    TxParams.AckReceived = mcpsConfirm->AckReceived;

    LmHandlerCallbacks->OnTxData(&TxParams);

    LmHandlerPackagesNotify(PACKAGE_MCPS_CONFIRM, mcpsConfirm);
}

static void McpsIndication(McpsIndication_t *mcpsIndication)
{
    LmHandlerAppData_t appData;

    RxParams.IsMcpsIndication = 1;
    RxParams.Status = mcpsIndication->Status;
    // printf("\n\n--------McpsIndication step1------------\n\n");
    if (RxParams.Status != LORAMAC_EVENT_INFO_STATUS_OK)
    {
        // printf("\n\n------RxParams.Status = %d--------------\n\n", RxParams.Status);
        return;
    }

    RxParams.Datarate = mcpsIndication->RxDatarate;
    RxParams.Rssi = mcpsIndication->Rssi;
    RxParams.Snr = mcpsIndication->Snr;
    RxParams.DownlinkCounter = mcpsIndication->DownLinkCounter;
    RxParams.RxSlot = mcpsIndication->RxSlot;
    RxParams.IsRevACK = mcpsIndication->AckReceived;

    appData.Port = mcpsIndication->Port;
    appData.BufferSize = mcpsIndication->BufferSize;
    appData.Buffer = mcpsIndication->Buffer;

    LmHandlerCallbacks->OnRxData(&appData, &RxParams);
    // printf("\n\n--------McpsIndication step2------------\n\n");
    if (mcpsIndication->DeviceTimeAnsReceived == true)
    {
#if (LMH_SYS_TIME_UPDATE_NEW_API == 1)
        // Provide fix values. DeviceTimeAns is accurate
        LmHandlerCallbacks->OnSysTimeUpdate(true, 0);
#else
        LmHandlerCallbacks->OnSysTimeUpdate();
#endif
    }
    // Call packages RxProcess function
    LmHandlerPackagesNotify(PACKAGE_MCPS_INDICATION, mcpsIndication);

    if ((mcpsIndication->FramePending == true) && (LmHandlerGetCurrentClass() == CLASS_A))
    {
        // The server signals that it has pending data to be sent.
        // We schedule an uplink as soon as possible to flush the server.

        // Send an empty message
        LmHandlerAppData_t appData =
            {
                .Buffer = NULL,
                .BufferSize = 0,
                .Port = 0};
        LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG);
    }
}

static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
    TxParams.IsMcpsConfirm = 0;
    TxParams.Status = mlmeConfirm->Status;
    LmHandlerCallbacks->OnTxData(&TxParams);

    LmHandlerPackagesNotify(PACKAGE_MLME_CONFIRM, mlmeConfirm);

    switch (mlmeConfirm->MlmeRequest)
    {
    case MLME_JOIN:
    {
        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_DEV_ADDR;
        LoRaMacMibGetRequestConfirm(&mibReq);
        JoinParams.CommissioningParams->DevAddr = mibReq.Param.DevAddr;
        JoinParams.Datarate = LmHandlerGetCurrentDatarate();

        // printf("\n\n------mlmeConfirm->Status = %d-----\n\n", mlmeConfirm->Status);

        if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
        {
            // Status is OK, node has joined the network
            JoinParams.Status = LORAMAC_HANDLER_SUCCESS;
        }
        else
        {
            // Join was not successful. Try to join again
            JoinParams.Status = LORAMAC_HANDLER_ERROR;
        }

        // printf("\n\n\n------------MlmeConfirm-------------\n\n");
        // Notify upper layer
        LmHandlerCallbacks->OnJoinRequest(&JoinParams);
    }
    break;
    case MLME_LINK_CHECK:
    {
        // Check DemodMargin
        // Check NbGateways
    }
    break;
    case MLME_DEVICE_TIME:
    {
        if (IsClassBSwitchPending == true)
        {
            LmHandlerBeaconReq();
        }
    }
    break;
    case MLME_BEACON_ACQUISITION:
    {
        if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
        {
            // Beacon has been acquired
            // Request server for ping slot
            LmHandlerPingSlotReq(0);
        }
        else
        {
            // Beacon not acquired
            // Request Device Time again.
            LmHandlerDeviceTimeReq();
        }
    }
    break;
    case MLME_PING_SLOT_INFO:
    {
        if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
        {
            MibRequestConfirm_t mibReq;

            // Class B is now activated
            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = CLASS_B;
            LoRaMacMibSetRequestConfirm(&mibReq);
            // Notify upper layer
            LmHandlerCallbacks->OnClassChange(CLASS_B);
            IsClassBSwitchPending = false;
        }
        else
        {
            LmHandlerPingSlotReq(0);
        }
    }
    break;
    default:
        break;
    }
}

static void MlmeIndication(MlmeIndication_t *mlmeIndication)
{
    // printf("\n\n---------------MlmeIndication step1-------------\n\n");
    RxParams.IsMcpsIndication = 0;
    RxParams.Status = mlmeIndication->Status;
    if (RxParams.Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED)
    {
        // printf("\n\n---------------MlmeIndication step2-------------\n\n");
        LmHandlerCallbacks->OnRxData(NULL, &RxParams);
    }
// printf("\n\n---------------MlmeIndication step3-------------\n\n");
    // Call packages RxProcess function
    LmHandlerPackagesNotify(PACKAGE_MLME_INDICATION, mlmeIndication);
// printf("\n\n---------------MlmeIndication step4-------------\n\n");
    switch (mlmeIndication->MlmeIndication)
    {
    case MLME_SCHEDULE_UPLINK:
    { // The MAC signals that we shall provide an uplink as soon as possible
        // Send an empty message
        LmHandlerAppData_t appData =
            {
                .Buffer = NULL,
                .BufferSize = 0,
                .Port = 0};
// printf("\n\n---------------MlmeIndication step5-------------\n\n");

        if(LmHandlerPackages[PACKAGE_ID_COMPLIANCE] != NULL)
        {
            if (LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning() == false)
            {
                // printf("\n\n---------------MlmeIndication step6-------------\n\n");
                LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG);
            }
        }
        // printf("\n\n---------------MlmeIndication step7-------------\n\n");
    }
    break;
    case MLME_BEACON_LOST:
    {
        MibRequestConfirm_t mibReq;
        // Switch to class A again
        mibReq.Type = MIB_DEVICE_CLASS;
        mibReq.Param.Class = CLASS_A;
        LoRaMacMibSetRequestConfirm(&mibReq);

        BeaconParams.State = LORAMAC_HANDLER_BEACON_LOST;
        BeaconParams.Info.Time.Seconds = 0;
        BeaconParams.Info.GwSpecific.InfoDesc = 0;
        memset1(BeaconParams.Info.GwSpecific.Info, 0, 6);

        LmHandlerCallbacks->OnClassChange(CLASS_A);
        LmHandlerCallbacks->OnBeaconStatusChange(&BeaconParams);

        LmHandlerDeviceTimeReq();
    }
    break;
    case MLME_BEACON:
    {
        if (mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED)
        {
            BeaconParams.State = LORAMAC_HANDLER_BEACON_RX;
            BeaconParams.Info = mlmeIndication->BeaconInfo;

            LmHandlerCallbacks->OnBeaconStatusChange(&BeaconParams);
        }
        else
        {
            BeaconParams.State = LORAMAC_HANDLER_BEACON_NRX;
            BeaconParams.Info = mlmeIndication->BeaconInfo;

            LmHandlerCallbacks->OnBeaconStatusChange(&BeaconParams);
        }
        break;
    }
    default:
        break;
    }
    // printf("\n\n---------------MlmeIndication step2-------------\n\n");
}

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */

LmHandlerErrorStatus_t LmHandlerPackageRegister(uint8_t id, void *params)
{
    LmhPackage_t *package = NULL;
    switch (id)
    {
    case PACKAGE_ID_COMPLIANCE:
    {
        package = LmphCompliancePackageFactory();
        break;
    }
    case PACKAGE_ID_CLOCK_SYNC:
    {
        package = LmphClockSyncPackageFactory();
        break;
    }
    case PACKAGE_ID_REMOTE_MCAST_SETUP:
    {
        package = LmhpRemoteMcastSetupPackageFactory();
        break;
    }
    case PACKAGE_ID_FRAGMENTATION:
    {
        package = LmhpFragmentationPackageFactory();
        break;
    }
    }
    if (package != NULL)
    {
        LmHandlerPackages[id] = package;
        LmHandlerPackages[id]->OnMacMcpsRequest = LmHandlerCallbacks->OnMacMcpsRequest;
        LmHandlerPackages[id]->OnMacMlmeRequest = LmHandlerCallbacks->OnMacMlmeRequest;
        LmHandlerPackages[id]->OnJoinRequest = LmHandlerJoinRequest;
        LmHandlerPackages[id]->OnSendRequest = LmHandlerSend;
        LmHandlerPackages[id]->OnDeviceTimeRequest = LmHandlerDeviceTimeReq;
        LmHandlerPackages[id]->OnSysTimeUpdate = LmHandlerCallbacks->OnSysTimeUpdate;
        LmHandlerPackages[id]->Init(params, LmHandlerParams->DataBuffer, LmHandlerParams->DataBufferMaxSize);

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

bool LmHandlerPackageIsInitialized(uint8_t id)
{
    if (LmHandlerPackages[id]->IsInitialized != NULL)
    {
        return LmHandlerPackages[id]->IsInitialized();
    }
    else
    {
        return false;
    }
}

bool LmHandlerPackageIsRunning(uint8_t id)
{
    if (LmHandlerPackages[id]->IsRunning != NULL)
    {
        return LmHandlerPackages[id]->IsRunning();
    }
    else
    {
        return false;
    }
}

static void LmHandlerPackagesNotify(PackageNotifyTypes_t notifyType, void *params)
{
    for (int8_t i = 0; i < PKG_MAX_NUMBER; i++)
    {
        if (LmHandlerPackages[i] != NULL)
        {
            switch (notifyType)
            {
            case PACKAGE_MCPS_CONFIRM:
            {
                if (LmHandlerPackages[i]->OnMcpsConfirmProcess != NULL)
                {
                    LmHandlerPackages[i]->OnMcpsConfirmProcess((McpsConfirm_t *)params);
                }
                break;
            }
            case PACKAGE_MCPS_INDICATION:
            {
                if ((LmHandlerPackages[i]->OnMcpsIndicationProcess != NULL) &&
                    (LmHandlerPackages[i]->Port == ((McpsIndication_t *)params)->Port))
                {
                    LmHandlerPackages[i]->OnMcpsIndicationProcess((McpsIndication_t *)params);
                }
                break;
            }
            case PACKAGE_MLME_CONFIRM:
            {
                if (LmHandlerPackages[i]->OnMlmeConfirmProcess != NULL)
                {
                    LmHandlerPackages[i]->OnMlmeConfirmProcess((MlmeConfirm_t *)params);
                }
                break;
            }
            case PACKAGE_MLME_INDICATION:
            {
                if (LmHandlerPackages[i]->OnMlmeIndicationProcess != NULL)
                {
                    LmHandlerPackages[i]->OnMlmeIndicationProcess(params);
                }
                break;
            }
            }
        }
    }
}

static void LmHandlerPackagesProcess(void)
{
    for (int8_t i = 0; i < PKG_MAX_NUMBER; i++)
    {
        if ((LmHandlerPackages[i] != NULL) &&
            (LmHandlerPackages[i]->Process != NULL) &&
            (LmHandlerPackageIsInitialized(i) != false))
        {
            LmHandlerPackages[i]->Process();
        }
    }
}
