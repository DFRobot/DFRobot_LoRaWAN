/*!
 * \file      timer.c
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
#include <Ticker.h>
#include <Arduino.h>
#include "system/utilities.h"
#include "boards/mcu/board.h"
#include "boards/rtc-board.h"
#include "boards/mcu/timer.h"

Ticker timerTickers[10];
uint32_t timerTimes[10];
bool timerInUse[10] = {false, false, false, false, false, false, false, false, false, false};

/*!
 * Safely execute call back
 */
#define ExecuteCallBack( _callback_, context ) \
    do                                         \
    {                                          \
        if( _callback_ == NULL )               \
        {                                      \
            while( 1 );                        \
        }                                      \
        else                                   \
        {                                      \
            _callback_( context );             \
        }                                      \
    }while( 0 );

/*!
 * Timers list head pointer
 */
// static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
// static void TimerInsertNewHeadTimer( TimerEvent_t *obj );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
// static void TimerInsertTimer( TimerEvent_t *obj );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
// static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
// static bool TimerExists( TimerEvent_t *obj );

void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
    // Look for an available Ticker
	for (int idx = 0; idx < 10; idx++)
	{
		if (timerInUse[idx] == false)
		{
			timerInUse[idx] = true;
			obj->timerNum = idx;
			obj->Callback = callback;
			return;
		}
	}
}

void TimerSetContext( TimerEvent_t *obj, void* context )
{
    // obj->Context = context;
}

void TimerStart( TimerEvent_t *obj )
{
    int idx = obj->timerNum;
	if (obj->oneShot)
	{
		timerTickers[idx].once_ms(timerTimes[idx], obj->Callback);
	}
	else
	{
		timerTickers[idx].attach_ms(timerTimes[idx], obj->Callback);
	}
}

// static void TimerInsertTimer( TimerEvent_t *obj )
// {
//     // TimerEvent_t* cur = TimerListHead;
//     // TimerEvent_t* next = TimerListHead->Next;

//     // while( cur->Next != NULL )
//     // {
//     //     if( obj->Timestamp > next->Timestamp )
//     //     {
//     //         cur = next;
//     //         next = next->Next;
//     //     }
//     //     else
//     //     {
//     //         cur->Next = obj;
//     //         obj->Next = next;
//     //         return;
//     //     }
//     // }
//     // cur->Next = obj;
//     // obj->Next = NULL;
// }

// static void TimerInsertNewHeadTimer( TimerEvent_t *obj )
// {
//     // TimerEvent_t* cur = TimerListHead;

//     // if( cur != NULL )
//     // {
//     //     cur->IsNext2Expire = false;
//     // }

//     // obj->Next = cur;
//     // TimerListHead = obj;
//     // TimerSetTimeout( TimerListHead );
// }

bool TimerIsStarted( TimerEvent_t *obj )
{
    return obj->IsRunning;
}

void TimerIrqHandler( void )
{
    // TimerEvent_t* cur;
    // TimerEvent_t* next;

    // uint32_t old =  RtcGetTimerContext( );
    // uint32_t now =  RtcSetTimerContext( );
    // uint32_t deltaContext = now - old; // intentional wrap around

    // // Update timeStamp based upon new Time Reference
    // // because delta context should never exceed 2^32
    // if( TimerListHead != NULL )
    // {
    //     for( cur = TimerListHead; cur->Next != NULL; cur = cur->Next )
    //     {
    //         next = cur->Next;
    //         if( next->Timestamp > deltaContext )
    //         {
    //             next->Timestamp -= deltaContext;
    //         }
    //         else
    //         {
    //             next->Timestamp = 0;
    //         }
    //     }
    // }

    // // Execute immediately the alarm callback
    // if ( TimerListHead != NULL )
    // {
    //     cur = TimerListHead;
    //     TimerListHead = TimerListHead->Next;
    //     cur->IsStarted = false;
    //     ExecuteCallBack( cur->Callback, cur->Context );
    // }

    // // Remove all the expired object from the list
    // while( ( TimerListHead != NULL ) && ( TimerListHead->Timestamp < RtcGetTimerElapsedTime( ) ) )
    // {
    //     cur = TimerListHead;
    //     TimerListHead = TimerListHead->Next;
    //     cur->IsStarted = false;
    //     ExecuteCallBack( cur->Callback, cur->Context );
    // }

    // // Start the next TimerListHead if it exists AND NOT running
    // if( ( TimerListHead != NULL ) && ( TimerListHead->IsNext2Expire == false ) )
    // {
    //     TimerSetTimeout( TimerListHead );
    // }
}

void TimerStop( TimerEvent_t *obj )
{
    // CRITICAL_SECTION_BEGIN( );

    // TimerEvent_t* prev = TimerListHead;
    // TimerEvent_t* cur = TimerListHead;

    // // List is empty or the obj to stop does not exist
    // if( ( TimerListHead == NULL ) || ( obj == NULL ) )
    // {
    //     CRITICAL_SECTION_END( );
    //     return;
    // }

    // obj->IsStarted = false;

    // if( TimerListHead == obj ) // Stop the Head
    // {
    //     if( TimerListHead->IsNext2Expire == true ) // The head is already running
    //     {
    //         TimerListHead->IsNext2Expire = false;
    //         if( TimerListHead->Next != NULL )
    //         {
    //             TimerListHead = TimerListHead->Next;
    //             TimerSetTimeout( TimerListHead );
    //         }
    //         else
    //         {
    //             RtcStopAlarm( );
    //             TimerListHead = NULL;
    //         }
    //     }
    //     else // Stop the head before it is started
    //     {
    //         if( TimerListHead->Next != NULL )
    //         {
    //             TimerListHead = TimerListHead->Next;
    //         }
    //         else
    //         {
    //             TimerListHead = NULL;
    //         }
    //     }
    // }
    // else // Stop an object within the list
    // {
    //     while( cur != NULL )
    //     {
    //         if( cur == obj )
    //         {
    //             if( cur->Next != NULL )
    //             {
    //                 cur = cur->Next;
    //                 prev->Next = cur;
    //             }
    //             else
    //             {
    //                 cur = NULL;
    //                 prev->Next = cur;
    //             }
    //             break;
    //         }
    //         else
    //         {
    //             prev = cur;
    //             cur = cur->Next;
    //         }
    //     }
    // }
    // CRITICAL_SECTION_END( );

    int idx = obj->timerNum;
	timerTickers[idx].detach();
}

// static bool TimerExists( TimerEvent_t *obj )
// {
//     // TimerEvent_t* cur = TimerListHead;

//     // while( cur != NULL )
//     // {
//     //     if( cur == obj )
//     //     {
//     //         return true;
//     //     }
//     //     cur = cur->Next;
//     // }
//     // return false;
// }

void TimerReset( TimerEvent_t *obj )
{
    // TimerStop( obj );
    // TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    int idx = obj->timerNum;
	timerTimes[idx] = value;
}

TimerTime_t TimerGetCurrentTime( void )                 //  利用esp32内置函数       mating
{
    return millis();
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )     //  利用esp32内置函数       mating
{
    // if ( past == 0 )
    // {
    //     return 0;
    // }
    // uint32_t nowInTicks = RtcGetTimerValue( );
    // uint32_t pastInTicks = RtcMs2Tick( past );

    // // Intentional wrap around. Works Ok if tick duration below 1ms
    // return RtcTick2Ms( nowInTicks - pastInTicks );

    uint32_t nowInTicks = millis();
	uint32_t pastInTicks = past;
	TimerTime_t diff = nowInTicks - pastInTicks;

	return diff;
}

// static void TimerSetTimeout( TimerEvent_t *obj )
// {
//     // int32_t minTicks= RtcGetMinimumTimeout( );
//     // obj->IsNext2Expire = true;

//     // // In case deadline too soon
//     // if( obj->Timestamp  < ( RtcGetTimerElapsedTime( ) + minTicks ) )
//     // {
//     //     obj->Timestamp = RtcGetTimerElapsedTime( ) + minTicks;
//     // }
//     // RtcSetAlarm( obj->Timestamp );
// }

TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature )
{
    return RtcTempCompensation( period, temperature );
}

void TimerProcess( void )
{
    RtcProcess( );
}
