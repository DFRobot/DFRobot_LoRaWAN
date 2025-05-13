/*!
 * \file      LoRaMacConfirmQueue.h
 *
 * \brief     LoRa MAC confirm queue implementation
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
 *              (C)2013 Semtech
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
 * \defgroup  LORAMACCONFIRMQUEUE LoRa MAC confirm queue implementation
 *            This module specifies the API implementation of the LoRaMAC confirm queue.
 *            The confirm queue is implemented with as a ring buffer. The number of
 *            elements can be defined with \ref LORA_MAC_MLME_CONFIRM_QUEUE_LEN. The
 *            current implementation does not support multiple elements of the same
 *            Mlme_t type.
 * 
 *            该模块指定了 LoRaMAC 确认队列的 API 实现。
 *            确认队列以环形缓冲区的方式实现。
 *            元素的数量可以通过 \ref LORA_MAC_NLINE_CONFIRM_QUEUE_LEN 定义。
 *            当前的实现不支持相同 Mlme_t 类型的多个元素。
 * \{
 */
#ifndef __LORAMAC_CONFIRMQUEUE_H__
#define __LORAMAC_CONFIRMQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "LoRaMac.h"

/*!
 * LoRaMac MLME-Confirm queue length
 */
#define LORA_MAC_MLME_CONFIRM_QUEUE_LEN             5

/*!
 * Structure to hold multiple MLME request confirm data     结构体保存多个MLME请求确认数据
 */
typedef struct sMlmeConfirmQueue
{
    /*!
     * Holds the previously performed MLME-Request      保存先前执行的MLME-Request
     */
    Mlme_t Request;
    /*!
     * Status of the operation                          操作状态
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * Set to true, if the request is ready to be handled       是否准备好处理请求
     */
    bool ReadyToHandle;
    /*!
     * Set to true, if it is not permitted to set the ReadyToHandle variable        如果不允许使用调用LoRaMacConfirmQueueSetStatusCmn的函数来设置ReadyToHandle变量，则设置为true
     * with a function call to LoRaMacConfirmQueueSetStatusCmn.
     */
    bool RestrictCommonReadyToHandle;
}MlmeConfirmQueue_t;

/*!
 * \brief   Initializes the confirm queue                               初始化确认队列，参数为MAC原语
 *
 * \param   [IN] primitives - Pointer to the LoRaMac primitives.
 */
void LoRaMacConfirmQueueInit( LoRaMacPrimitives_t* primitive );

/*!
 * \brief   Adds an element to the confirm queue.                       将元素添加到确认队列
 *
 * \param   [IN] mlmeConfirm - Pointer to the element to add.
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueAdd( MlmeConfirmQueue_t* mlmeConfirm );

/*!
 * \brief   Removes the last element which was added into the queue.            删除队列中最后添加的元素
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueRemoveLast( void );

/*!
 * \brief   Removes the first element which was added to the confirm queue.     删除队列中最先添加的元素
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueRemoveFirst( void );

/*!
 * \brief   Sets the status of an element.                                  设置元素的状态
 *
 * \param   [IN] status - The status to set.
 *
 * \param   [IN] request - The related request to set the status.
 */
void LoRaMacConfirmQueueSetStatus( LoRaMacEventInfoStatus_t status, Mlme_t request );

/*!
 * \brief   Gets the status of an element.                                  获取元素的状态
 *
 * \param   [IN] request - The request to query the status.
 *
 * \retval  The status of the related MlmeRequest.
 */
LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatus( Mlme_t request );

/*!
 * \brief   Sets a common status for all elements in the queue.             为队列中的所有元素设置公共状态
 *
 * \param   [IN] status - The status to set.
 */
void LoRaMacConfirmQueueSetStatusCmn( LoRaMacEventInfoStatus_t status );

/*!
 * \brief   Gets the common status of all elements.                         获取所有元素的公共状态。
 *
 * \retval  The common status of all elements.
 */
LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatusCmn( void );

/*!
 * \brief   Verifies if a request is in the queue and active.       验证请求是否在队列中并且处于活动状态
 *
 * \param   [IN] request - The request to verify.
 *
 * \retval  [true - element is in the queue, false - element is not in the queue].
 */
bool LoRaMacConfirmQueueIsCmdActive( Mlme_t request );

/*!
 * \brief   Handles all callbacks of active requests                处理活动请求的所有回调
 *
 * \param   [IN] mlmeConfirm - Pointer to the generic mlmeConfirm structure.
 */
void LoRaMacConfirmQueueHandleCb( MlmeConfirm_t* mlmeConfirm );

/*!
 * \brief   Query number of elements in the queue.                  查询队列中元素个数
 *
 * \retval  Number of elements.
 */
uint8_t LoRaMacConfirmQueueGetCnt( void );

/*!
 * \brief   Verify if the confirm queue is full.                    查询确认队列是否已满
 *
 * \retval  [true - queue is full, false - queue is not full].
 */
bool LoRaMacConfirmQueueIsFull( void );

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_CONFIRMQUEUE_H__
