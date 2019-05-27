#include "ee.h"





/***************************************************************************
 *
 * Kernel ( CPU 0 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(ReceiveData);
    DeclareTask(CheckMessage);
    DeclareTask(TaskPollingRooms);
    DeclareTask(TaskLCD);
    DeclareTask(UserTask);

    const EE_THREAD_PTR EE_hal_thread_body[EE_MAX_TASK] = {
        &FuncReceiveData,		/* thread ReceiveData */
        &FuncCheckMessage,		/* thread CheckMessage */
        &FuncTaskPollingRooms,		/* thread TaskPollingRooms */
        &FuncTaskLCD,		/* thread TaskLCD */
        &FuncUserTask 		/* thread UserTask */

    };

    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x4U,		/* thread ReceiveData */
        0x4U,		/* thread CheckMessage */
        0x2U,		/* thread TaskPollingRooms */
        0x1U,		/* thread TaskLCD */
        0x1U 		/* thread UserTask */
    };

    /* dispatch priority */
    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x4U,		/* thread ReceiveData */
        0x4U,		/* thread CheckMessage */
        0x2U,		/* thread TaskPollingRooms */
        0x1U,		/* thread TaskLCD */
        0x1U 		/* thread UserTask */
    };

    /* thread status */
    #if defined(__MULTI__) || defined(__WITH_STATUS__)
        EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
            EE_READY,
            EE_READY,
            EE_READY,
            EE_READY,
            EE_READY
        };
    #endif

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL
    };

    EE_TYPEPRIO EE_th_nact[EE_MAX_TASK];
    /* The first stacked task */
    EE_TID EE_stkfirst = EE_NIL;

    /* The first task into the ready queue */
    EE_TID EE_rqfirst  = EE_NIL;

    /* system ceiling */
    EE_TYPEPRIO EE_sys_ceiling= 0x0000U;



/***************************************************************************
 *
 * Counters
 *
 **************************************************************************/
    EE_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER] = {
        {0, -1}         /* myCounter */
    };



/***************************************************************************
 *
 * Alarms
 *
 **************************************************************************/
    const EE_alarm_ROM_type   EE_alarm_ROM[EE_ALARM_ROM_SIZE] = {
        {0, EE_ALARM_ACTION_TASK    , ReceiveData, NULL},
        {0, EE_ALARM_ACTION_TASK    , TaskPollingRooms, NULL},
        {0, EE_ALARM_ACTION_TASK    , TaskLCD, NULL},
        {0, EE_ALARM_ACTION_TASK    , UserTask, NULL}
    };

    EE_alarm_RAM_type         EE_alarm_RAM[EE_MAX_ALARM];

