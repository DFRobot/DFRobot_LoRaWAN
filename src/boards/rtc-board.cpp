#include "rtc-board.h"
#include <esp_timer.h>
#include <stdint.h>
#include <esp_attr.h>
#include "system/utilities.h"

#define MIN_ALARM_DELAY                             3

uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
    // 获取当前时间（微秒）
    int64_t time_us = esp_timer_get_time();

    // 转换为秒和毫秒
    uint32_t seconds = (uint32_t)(time_us / 1000000ULL);
    *milliseconds = (uint16_t)((time_us % 1000000ULL) / 1000ULL);

    return seconds;
}

RTC_DATA_ATTR uint32_t RtcBkupRegisters[] = { 0, 0 };

void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{
    CRITICAL_SECTION_BEGIN( );
    RtcBkupRegisters[0] = data0;
    RtcBkupRegisters[1] = data1;
    CRITICAL_SECTION_END( );
}

void RtcBkupRead( uint32_t* data0, uint32_t* data1 )
{
    CRITICAL_SECTION_BEGIN( );
    *data0 = RtcBkupRegisters[0];
    *data1 = RtcBkupRegisters[1];
    CRITICAL_SECTION_END( );
}

uint32_t RtcGetMinimumTimeout( void )
{
    return( MIN_ALARM_DELAY );
}

TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature )
{
    return period;
}

void RtcProcess( void )
{

}

uint32_t RtcMs2Tick( TimerTime_t milliseconds )
{
    return ( uint32_t )( ( ( ( uint64_t )milliseconds ) << 15 ) / 1000 );
}


// vvvvvvvvvvvvv以下函数先空实现占位，后续用ticker库代替vvvvvvvvvvvvvvvvvvv
void RtcSetAlarm( uint32_t timeout )
{

}

void RtcStopAlarm( void )
{

}


uint32_t RtcSetTimerContext( void )
{
    return 0;
}

uint32_t RtcGetTimerContext( void )
{
    return 0;
}

uint32_t RtcGetTimerElapsedTime( void )
{
    return 0;
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^