/*!
 * \file      rtc-board.h
 *
 * \brief     Target board RTC timer and low power modes management
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
#ifndef __RTC_BOARD_H__
#define __RTC_BOARD_H__



#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif

#include <stdbool.h>
// #include "boards/mcu/timer.h"

/*!
 * \brief Temperature coefficient of the clock source                   时钟源的温度系数
 */
#define RTC_TEMP_COEFFICIENT                            ( -0.035f )

/*!
 * \brief Temperature coefficient deviation of the clock source         时钟源的温度系数偏差
 */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035f )

/*!
 * \brief Turnover temperature of the clock source                      时钟源的周转温度
 */
#define RTC_TEMP_TURNOVER                               ( 25.0f )

/*!
 * \brief Turnover temperature deviation of the clock source            时钟源的周转温度偏差
 */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0f )

/*!
 * \brief Initializes the RTC timer
 *
 * \remark The timer is based on the RTC
 */
void RtcInit( void );                                                 // 初始化 RTC 计时器          未使用

/*!
 * \brief Returns the minimum timeout value
 *
 * \retval minTimeout Minimum timeout value in in ticks
 */
uint32_t RtcGetMinimumTimeout( void );                                // 返回最小超时值             已实现

/*!
 * \brief converts time in ms to time in ticks
 *
 * \param[IN] milliseconds Time in milliseconds
 * \retval returns time in timer ticks
 */
uint32_t RtcMs2Tick( TimerTime_t milliseconds );                      // 将毫秒时间转换为刻度时间    已实现

/*!
 * \brief converts time in ticks to time in ms
 *
 * \param[IN] time in timer ticks
 * \retval returns time in milliseconds
 */
TimerTime_t RtcTick2Ms( uint32_t tick );                              // 将以刻度为单位的时间转换为以毫秒为单位的时间       未使用

/*!
 * \brief Performs a delay of milliseconds by polling RTC
 *
 * \param[IN] milliseconds Delay in ms
 */
void RtcDelayMs( TimerTime_t milliseconds );                          // 通过轮询 RTC 执行毫秒延迟                        未使用

/*!
 * \brief Calculates the wake up time between wake up and MCU start
 *
 * \note Resolution in RTC_ALARM_TIME_BASE
 */
void RtcSetMcuWakeUpTime( void );                                     // 意义不明       未使用

/*!
 * \brief Returns the wake up time in ticks
 *
 * \retval wakeUpTime The WakeUpTime value in ticks
 */
int16_t RtcGetMcuWakeUpTime( void );                                  // 返回唤醒时间（以刻度为单位）   未使用

/*!
 * \brief Sets the alarm
 *
 * \note The alarm is set at now (read in this funtion) + timeout
 *
 * \param timeout [IN] Duration of the Timer ticks
 */
void RtcSetAlarm( uint32_t timeout );                                 // 设置闹钟               不实现，相关功能改用esp32 Ticker库实现

/*!
 * \brief Stops the Alarm
 */
void RtcStopAlarm( void );                                            // 停止闹钟               不实现，相关功能改用esp32 Ticker库实现

/*!
 * \brief Starts wake up alarm
 *
 * \note  Alarm in RtcTimerContext.Time + timeout
 *
 * \param [IN] timeout Timeout value in ticks
 */
void RtcStartAlarm( uint32_t timeout );                               // 启动唤醒闹钟           未使用

/*!
 * \brief Sets the RTC timer reference
 *
 * \retval value Timer reference value in ticks
 */
uint32_t RtcSetTimerContext( void );                                  // 设置 RTC 定时器参考    不实现，相关功能改用esp32 Ticker库实现
  
/*!
 * \brief Gets the RTC timer reference
 *
 * \retval value Timer value in ticks
 */
uint32_t RtcGetTimerContext( void );                                  // 获取 RTC 定时器参考    不实现，相关功能改用esp32 Ticker库实现

/*!
 * \brief Gets the system time with the number of seconds elapsed since epoch
 *
 * \param [OUT] milliseconds Number of milliseconds elapsed since epoch
 * \retval seconds Number of seconds elapsed since epoch
 */
uint32_t RtcGetCalendarTime( uint16_t *milliseconds );                // 获取系统时间以及自纪元以来经过的秒数 已实现

/*!
 * \brief Get the RTC timer value
 *
 * \retval RTC Timer value
 */
uint32_t RtcGetTimerValue( void );                                    // 获取 RTC 定时器值          未使用

/*!
 * \brief Get the RTC timer elapsed time since the last Alarm was set
 *
 * \retval RTC Elapsed time since the last alarm in ticks.
 */
uint32_t RtcGetTimerElapsedTime( void );                              // 获取自上次设置闹钟以来 RTC 计时器经过的时间    不实现，相关功能改用esp32 Ticker库实现

/*!
 * \brief Writes data0 and data1 to the RTC backup registers
 *
 * \param [IN] data0 1st Data to be written
 * \param [IN] data1 2nd Data to be written
 */
void RtcBkupWrite( uint32_t data0, uint32_t data1 );                                // ------------ 已实现

/*!
 * \brief Reads data0 and data1 from the RTC backup registers
 *
 * \param [OUT] data0 1st Data to be read
 * \param [OUT] data1 2nd Data to be read
 */
void RtcBkupRead( uint32_t* data0, uint32_t* data1 );                               // ------------ 已实现

/*!
 * \brief Processes pending timer events
 */
void RtcProcess( void );                                                            // 处理RTC事件   空实现

/*!
 * \brief Computes the temperature compensation for a period of time on a
 *        specific temperature.
 *
 * \param [IN] period Time period to compensate in milliseconds
 * \param [IN] temperature Current temperature
 *
 * \retval Compensated time period
 */
TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature );           // 温度补偿     假实现

#ifdef __cplusplus
}
#endif

#endif // __RTC_BOARD_H__
