/*!
 * \file      LoRaMacCrypto.h
 *
 * \brief     LoRa MAC layer cryptographic functionality implementation   LoRa MAC层加密功能的实现  
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
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_CRYPTO_H__
#define __LORAMAC_CRYPTO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "system/utilities.h"
#include "LoRaMacTypes.h"
#include "LoRaMacMessageTypes.h"
#include "LoRaMacCryptoNvm.h"

/*!
 * Indicates if LoRaWAN 1.1.x crypto scheme is enabled 表示是否为LoRaWAN 1.1。启用X加密方案
 */
#define USE_LRWAN_1_1_X_CRYPTO                      0

/*!
 * Indicates if a random devnonce must be used or not   指示是否必须使用随机devnonce
 */
#define USE_RANDOM_DEV_NONCE                        1

/*!
 * Indicates if JoinNonce is counter based and requires to be checked   指示JoinNonce是否基于计数器，是否需要检查
 */
#define USE_JOIN_NONCE_COUNTER_CHECK                0

/*!
 * Initial value of the frame counters      帧计数器的初始值
 */
#define FCNT_DOWN_INITAL_VALUE          0xFFFFFFFF

/*!
 * LoRaMac Cryto Status
 */
typedef enum eLoRaMacCryptoStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_CRYPTO_SUCCESS = 0,
    /*!
     * MIC does not match       MIC不匹配
     */
    LORAMAC_CRYPTO_FAIL_MIC,
    /*!
     * Address does not match
     */
    LORAMAC_CRYPTO_FAIL_ADDRESS,
    /*!
     * JoinNonce was not greater than previous one.
     */
    LORAMAC_CRYPTO_FAIL_JOIN_NONCE,
    /*!
     * RJcount0 reached 2^16-1
     */
    LORAMAC_CRYPTO_FAIL_RJCOUNT0_OVERFLOW,
    /*!
     * FCNT_ID is not supported
     */
    LORAMAC_CRYPTO_FAIL_FCNT_ID,
    /*!
     * FCntUp/Down check failed (new FCnt is smaller than previous one)     FCNTUP/DOWN检查失败（新FCNT小于以前的FCNT）
     */
    LORAMAC_CRYPTO_FAIL_FCNT_SMALLER,
    /*!
     * FCntUp/Down check failed (duplicated)
     */
    LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED,
    /*!
     * MAX_GAP_FCNT check failed
     */
    LORAMAC_CRYPTO_FAIL_MAX_GAP_FCNT,
    /*!
     * Not allowed parameter value
     */
    LORAMAC_CRYPTO_FAIL_PARAM,
    /*!
     * Null pointer exception       NULL指针异常
     */
    LORAMAC_CRYPTO_ERROR_NPE,
    /*!
     * Invalid key identifier exception
     */
    LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID,
    /*!
     * Invalid address identifier exception     无效地址标识符异常
     */
    LORAMAC_CRYPTO_ERROR_INVALID_ADDR_ID,
    /*!
     * Invalid LoRaWAN specification version
     */
    LORAMAC_CRYPTO_ERROR_INVALID_VERSION,
    /*!
     * Incompatible buffer size
     */
    LORAMAC_CRYPTO_ERROR_BUF_SIZE,
    /*!
     * The secure element reports an error      安全元素报告错误
     */
    LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC,
    /*!
     * Error from parser reported               解析器的错误报告
     */
    LORAMAC_CRYPTO_ERROR_PARSER,
    /*!
     * Error from serializer reported
     */
    LORAMAC_CRYPTO_ERROR_SERIALIZER,
    /*!
     * RJcount1 reached 2^16-1 which should never happen
     */
    LORAMAC_CRYPTO_ERROR_RJCOUNT1_OVERFLOW,
    /*!
     * Undefined Error occurred     未定义的错误发生
     */
    LORAMAC_CRYPTO_ERROR,
}LoRaMacCryptoStatus_t;

/*!
 * Signature of callback function to be called by the LoRaMac Crypto module when the
 * non-volatile context have to be stored. It is also possible to save the entire
 * crypto module context.
 * 当必须存储非易失性上下文时，LoRaMac Crypto模块要调用的回调函数的签名。也可以保存整个加密模块上下文。
 *
 */
typedef void ( *LoRaMacCryptoNvmEvent )( void );

/*!
 * Initialization of LoRaMac Crypto module
 * It sets initial values of volatile variables and assigns the non-volatile context.
 * LoRaMac加密模块初始化设置易失性变量的初始值，并分配非易失性上下文。
 *
 * \param[IN]     nvm                 - Pointer to the non-volatile memory data
 *                                      structure.
 * \retval                            - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoInit( LoRaMacCryptoNvmData_t* nvm );

/*!
 * Sets the LoRaWAN specification version to be used.       设置要使用的LoRaWAN规范版本。      
 *
 * \warning This function should be used for ABP only. In case of OTA the version will be set automatically.
 *          此函数应仅用于ABP。在OTA的情况下，版本将自动设置。
 *
 * \param[IN]     version             - LoRaWAN specification version to be used.
 *
 * \retval                            - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoSetLrWanVersion( Version_t version );

/*!
 * Returns updated fCntID downlink counter value.   返回更新后的fCntID下行计数器值。
 *
 * \param[IN]     fCntID         - Frame counter identifier
 * \param[IN]     maxFcntGap     - Maximum allowed frame counter difference (only necessary for L2 LW1.0.x)
 * \param[IN]     frameFcnt      - Received frame counter (used to update current counter value)
 * \param[OUT]    currentDown    - Current downlink counter value
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntDown( FCntIdentifier_t fCntID, uint16_t maxFCntGap, uint32_t frameFcnt, uint32_t* currentDown );

/*!
 * Returns updated fCntUp uplink counter value.     返回更新后的fCntUp上行链路计数器值
 *
 * \param[IN]     currentUp      - Uplink counter value
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntUp( uint32_t* currentUp );

/*!
 * Provides multicast context.      提供多播上下文
 *
 * \param[IN]     multicastList - Pointer to the multicast context list
 *
 * \retval                      - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoSetMulticastReference( MulticastCtx_t* multicastList );

/*!
 * Sets a key       设置一个密钥
 *
 * \param[IN]     keyID          - Key identifier
 * \param[IN]     key            - Key value (16 byte), if its a multicast key it must be encrypted with McKEKey
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoSetKey( KeyIdentifier_t keyID, uint8_t* key );

/*!
 * Prepares the join-request message.       准备入网请求消息
 * It computes the mic and add it to the message.       它计算麦克风并将其添加到消息中
 *
 * \param[IN/OUT] macMsg         - Join-request message object
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoPrepareJoinRequest( LoRaMacMessageJoinRequest_t* macMsg );

/*!
 * Prepares a rejoin-request type 1 message.    准备类型1的重新连接请求消息。
 * It computes the mic and add it to the message.       它计算麦克风并将其添加到消息中
 *
 * \param[IN/OUT] macMsg         - Rejoin message object
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoPrepareReJoinType1( LoRaMacMessageReJoinType1_t* macMsg );

/*!
 * Prepares a rejoin-request type 0 or 2 message.   准备类型0或2的重新连接请求消息。
 * It computes the mic and add it to the message.       它计算麦克风并将其添加到消息中
 *
 * \param[IN/OUT] macMsg         - Rejoin message object
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoPrepareReJoinType0or2( LoRaMacMessageReJoinType0or2_t* macMsg );

/*!
 * Handles the join-accept message.     处理Join-peccept消息。
 * It decrypts the message, verifies the MIC and if successful derives the session keys.   
 * 它解密消息，验证MIC，如果成功，则派生会话密钥 
 *
 * \param[IN]     joinReqType    - Type of last join-request or rejoin which triggered the join-accept response
 * \param[IN]     joinEUI        - Join server EUI (8 byte)
 * \param[IN/OUT] macMsg         - Join-accept message object
 * \retval                       - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoHandleJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEUI, LoRaMacMessageJoinAccept_t* macMsg );

/*!
 * Secures a message (encryption + integrity).      加密消息（加密+完整性）。
 *
 * \param[IN]     fCntUp          - Uplink sequence counter
 * \param[IN]     txDr            - Data rate used for the transmission
 * \param[IN]     txCh            - Index of the channel used for the transmission
 * \param[IN/OUT] macMsg          - Data message object
 * \retval                        - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoSecureMessage( uint32_t fCntUp, uint8_t txDr, uint8_t txCh, LoRaMacMessageData_t* macMsg );

/*!
 * Unsecures a message (decryption + integrity verification).       解密消息（解密+完整性验证）。
 *
 * \param[IN]     addrID          - Address identifier
 * \param[IN]     address         - Address
 * \param[IN]     fCntID          - Frame counter identifier
 * \param[IN]     fCntDown        - Downlink sequence counter
 * \param[IN/OUT] macMsg          - Data message object
 * \retval                        - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoUnsecureMessage( AddressIdentifier_t addrID, uint32_t address, FCntIdentifier_t fCntID, uint32_t fCntDown, LoRaMacMessageData_t* macMsg );

/*!
 * Derives the McRootKey from the AppKey.       从Appkey衍生出McRootkey。
 *
 * 1.0.x
 * McRootKey = aes128_encrypt(AppKey, 0x00 | pad16)
 *
 * 1.1.x
 * McRootKey = aes128_encrypt(AppKey, 0x20 | pad16)
 *
 * \param[IN]     versionMinor    - LoRaWAN specification minor version to be used.
 * \param[IN]     keyID           - Key identifier of the root key to use to perform the derivation ( AppKey )
 * \retval                        - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcRootKey( uint8_t versionMinor, KeyIdentifier_t keyID );

/*!
 * Derives the McKEKey from the McRootKey.      从mcootkey派生McKEKey
 *
 * McKEKey = aes128_encrypt(McRootKey , 0x00  | pad16)
 *
 * \param[IN]     keyID           - Key identifier of the root key to use to perform the derivation ( McRootKey )
 * \retval                        - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcKEKey( KeyIdentifier_t keyID );

/*!
 * Derives a Multicast group key pair ( McAppSKey, McNwkSKey ) from McKey
 *
 * McAppSKey = aes128_encrypt(McKey, 0x01 | McAddr | pad16)
 * McNwkSKey = aes128_encrypt(McKey, 0x02 | McAddr | pad16)
 *
 * \param[IN]     addrID          - Address identifier to select the multicast group
 * \param[IN]     mcAddr          - Multicast group address (4 bytes)
 * \retval                        - Status of the operation
 */
LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcSessionKeyPair( AddressIdentifier_t addrID, uint32_t mcAddr );

// 兼容DFRobot_LoRaRadio新增函数  手动加解密
/*!
 * Computes the LoRaMAC payload encryption
 *
 * \param   buffer          - Data buffer
 * \param   size            - Data buffer size
 * \param   key             - AES key to be used
 * \param   address         - Frame address
 * \param   dir             - Frame direction [0: uplink, 1: downlink]
 * \param   sequenceCounter - Frame sequence counter
 * \param   encBuffer       - Encrypted buffer
 */
void LoRaMacPayloadEncrypt(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer);

/*!
 * Computes the LoRaMAC payload decryption
 *
 * \param   buffer          - Data buffer
 * \param   size            - Data buffer size
 * \param   key             - AES key to be used
 * \param   address         - Frame address
 * \param   dir             - Frame direction [0: uplink, 1: downlink]
 * \param   sequenceCounter - Frame sequence counter
 * \param   decBuffer       - Decrypted buffer
 */
void LoRaMacPayloadDecrypt(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *decBuffer);

/*! \} addtogroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_CRYPTO_H__
