/*!
 * \file      LoRaMacParser.h
 *
 * \brief     LoRa MAC layer message parser functionality implementation
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
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_PARSER_H__
#define __LORAMAC_PARSER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "LoRaMacMessageTypes.h"

/*!
 * LoRaMac Parser Status
 */
typedef enum eLoRaMacParserStatus
{
    /*!
     * No error occurred            没有发送错误
     */
    LORAMAC_PARSER_SUCCESS = 0,
    /*!
     * Failure during parsing occurred  解析过程中发生故障
     */
    LORAMAC_PARSER_FAIL,
    /*!
     * Null pointer exception           NULL指针异常
     */
    LORAMAC_PARSER_ERROR_NPE,
    /*!
     * Undefined Error occurred         未定义的错误发生
     */
    LORAMAC_PARSER_ERROR,
}LoRaMacParserStatus_t;


/*!
 * Parse a serialized join-accept message and fills the structured object.
 *
 * \param[IN/OUT] macMsg       - Join-accept message object
 * \retval                     - Status of the operation
 */
LoRaMacParserStatus_t LoRaMacParserJoinAccept( LoRaMacMessageJoinAccept_t *macMsg );

/*!
 * Parse a serialized data message and fills the structured object.
 *
 * \param[IN/OUT] macMsg       - Data message object
 * \retval                     - Status of the operation
 */
LoRaMacParserStatus_t LoRaMacParserData( LoRaMacMessageData_t *macMsg );

/*! \} addtogroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_PARSER_H__

