/*!
 * \file      timer.h
 *
 * \brief     Timer objects and scheduling management implementation
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
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// /*!
//  * \brief Timer object description
//  */
// typedef struct TimerEvent_s
// {
//     uint32_t Timestamp;                  //! Current timer value                                当前时间戳
//     uint32_t ReloadValue;                //! Timer delay value                                  定时器延迟值     
//     bool IsStarted;                      //! Is the timer currently running                     定时器当前是否正在运行
//     bool IsNext2Expire;                  //! Is the next timer to expire                        下一个定时器是否到期
//     void ( *Callback )( void* context ); //! Timer IRQ callback function                        定时器IRQ回调函数
//     void *Context;                       //! User defined data object pointer to pass back      用户定义的数据对象指针传回
//     struct TimerEvent_s *Next;           //! Pointer to the next Timer object.                  指向下一个 Timer 对象的指针
// }TimerEvent_t;

/**@brief Timer object description
 */
typedef struct TimerEvent_s
{
	uint8_t timerNum;			  /**< Used with ESP32 MCU 1 for TX, 2 for RX*/
	// bool oneShot = true;		  /**< True if it is a one shot timer */
    bool oneShot;
	uint32_t Timestamp;			  /**< Current timer value */
	// uint32_t ReloadValue = 10000; /**< Timer delay value	*/
    uint32_t ReloadValue;
	bool IsRunning;				  /**< Is the timer currently running	*/
	void (*Callback)(void);		  /**< Timer IRQ callback function	*/
	struct TimerEvent_s *Next;	  /**< Pointer to the next Timer object.	*/
} TimerEvent_t;

/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#define TIMERTIME_T_MAX                             ( ( uint32_t )~0 )
#endif

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) );           // 定时器初始化

/*!
 * \brief Sets a user defined object pointer
 *
 * \param [IN] context User defined data object pointer to pass back
 *                     on IRQ handler callback
 */
void TimerSetContext( TimerEvent_t *obj, void* context );                           // 设置自定义用户数据指针       项目未使用

/*!
 * Timer IRQ event handler
 */
void TimerIrqHandler( void );                                                       // 定时器中断处理               项目未使用

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStart( TimerEvent_t *obj );                                               // 定时器启动

/*!
 * \brief Checks if the provided timer is running
 *
 * \param [IN] obj Structure containing the timer object parameters
 *
 * \retval status  returns the timer activity status [true: Started,
 *                                                    false: Stopped]
 */
bool TimerIsStarted( TimerEvent_t *obj );                                           // 定时器是否正在运行           项目未使用

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStop( TimerEvent_t *obj );                                                // 停止定时器

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerReset( TimerEvent_t *obj );                                               // 重置定时器                   项目未使用

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void TimerSetValue( TimerEvent_t *obj, uint32_t value );                            // 定时器设置超时时间

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t TimerGetCurrentTime( void );                                            // 获取当前时间

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \remark TimerGetElapsedTime will return 0 for argument 0.
 *
 * \param [IN] past         fix moment in Time
 * \retval time             returns elapsed time
 */
TimerTime_t TimerGetElapsedTime( TimerTime_t past );                                // 返回至参数时刻以来经过的时间

/*!
 * \brief Computes the temperature compensation for a period of time on a
 *        specific temperature.
 *
 * \param [IN] period Time period to compensate
 * \param [IN] temperature Current temperature
 *
 * \retval Compensated time period
 */
TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature );         // 计算一段时间在特定温度下的温度补偿时间

/*!
 * \brief Processes pending timer events
 */
void TimerProcess( void );                                                          // 处理待处理的计时器事件                   项目未使用

#ifdef __cplusplus
}
#endif

#endif // __TIMER_H__
