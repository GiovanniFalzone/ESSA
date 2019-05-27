#ifndef SWATCH_SM_H
#define SWATCH_SM_H

#define false 0
#define true 1

#define POSTPONE_AFTER		2
#define POSTPONE_MINUTES	1
#define ALARM_STOP_AFTER	5

typedef unsigned char uint8_T;
typedef unsigned char boolean_T;
typedef int int32_T;

typedef enum Signal {
	ENTRY_SIG, EXIT_SIG, TICK_E, PLUS_E, MINUS_E, TIME_MODE_E, TIMESET_MODE_E, ALARM_MODE_E, SWATCH_MODE_E, ALARM_ONOFF_E, POSTPONE_E, NULL_SIG
} Signal_t;

enum State_Control {
	TIME_MODE, TIMESET_MODE, ALARM_MODE, SWATCH_MODE, TS_SETHOURS, TS_SETMINUTES, A_SETHOURS, A_SETMINUTES, SW_STOPPED, SW_RUNNING
};

enum State_TimeCount {
	TIMECOUNT_S
};

enum State_AlarmMng {
	DISABLED_S, ENABLED_S, STOPPING_S, SILENT_S, RINGING_S, POSTPONE_S
};

struct SM_local{
	uint8_T hours;
	uint8_T minutes;
	uint8_T seconds;
	uint8_T tenths;

	uint8_T tmp_hours;
	uint8_T tmp_minutes;

	uint8_T A_hours;
	uint8_T A_minutes;
	uint8_T A_act_min;

	uint8_T SW_hours;
	uint8_T SW_minutes;
	uint8_T SW_seconds;
	uint8_T SW_tenths;
	uint8_T SW_lapmode;
};

struct SM_Output{
	uint8_T *O_hours;
	uint8_T	*O_minutes;
	uint8_T *O_seconds;
	uint8_T *O_tenths;
	uint8_T *O_mode;
	boolean_T *O_hblink;
	boolean_T *O_mblink;
	boolean_T *O_alarm_ringing;
	boolean_T *O_alarm_onoff;
};

struct SM_Input{
	boolean_T I_tick;
	boolean_T I_plus_button;
	boolean_T I_minus_button;
	boolean_T I_time_mode;
	boolean_T I_timeset_mode;
	boolean_T I_alarm_mode;
	boolean_T I_SW_mode;
	boolean_T I_alarmonoff_button;
	boolean_T I_postpone;
};

struct SM_local SM_local;
struct SM_Output SM_Output;
struct SM_Input SM_Input;

enum State_Control State_Control;
enum State_TimeCount State_TimeCount;
enum State_AlarmMng State_AlarmMng;

extern Signal_t setEvent_Signal(boolean_T I_tick, boolean_T I_plus_button, boolean_T I_minus_button, boolean_T I_time_mode, boolean_T I_timeset_mode, boolean_T I_alarm_mode,	boolean_T I_SW_mode, boolean_T I_alarmonoff_button,	boolean_T I_postpone);

extern void Control_init();
extern void Control_dispatch(unsigned const sig);
extern void Control_tran(enum State_Control target);

extern void Control_manage_TIME_MODE(unsigned const sig);
extern void Control_manage_TIME_SETMODE(unsigned const sig);
extern void Control_manage_ALARM_MODE(unsigned const sig);
extern void Control_manage_SWATCH_MODE(unsigned const sig);

extern void Control_manage_TS_SETHOURS(unsigned const sig);
extern void Control_manage_TS_SETMINUTES(unsigned const sig);
extern void Control_manage_A_SETHOURS(unsigned const sig);
extern void Control_manage_A_SETMINUTES(unsigned const sig);

extern void Control_manage_SW_RUNNING(unsigned const sig);
extern void Control_manage_SW_STOPPED(unsigned const sig);

extern void TimeCount_init();
extern void TimeCount_dispatch(unsigned const sig);
extern void TimeCount_tran(enum State_TimeCount target);

extern void AlarmMng_init();
extern void AlarmMng_dispatch(unsigned const sig);
extern void AlarmMng_tran(enum State_AlarmMng target);

extern void AlarmMng_manage_DISABLED_S(unsigned const sig);
extern void AlarmMng_manage_ENABLED_S(unsigned const sig);
extern void AlarmMng_manage_STOPPING_S(unsigned const sig);
extern void AlarmMng_manage_SILENT_S(unsigned const sig);
extern void AlarmMng_manage_RINGING_S(unsigned const sig);
extern void AlarmMng_manage_POSTPONE_S(unsigned const sig);

// main State Machine of the system
extern void SM_init(
	boolean_T *I_tick,
	boolean_T *I_plus_button,
	boolean_T *I_minus_button,
	boolean_T *I_time_mode,
	boolean_T *I_timeset_mode,
	boolean_T *I_alarm_mode,
	boolean_T *I_SW_mode,
	boolean_T *I_alarmonoff_button,
	boolean_T *I_postpone,
	uint8_T *O_hours,
	uint8_T *O_minutes,
	uint8_T *O_seconds,
	uint8_T *O_tenths,
	uint8_T *O_mode,
	boolean_T *O_hblink,
	boolean_T *O_mblink,
	boolean_T *O_alarm_ringing,
	boolean_T *O_alarm_onoff
);

extern void SM_step(
	boolean_T I_tick,
	boolean_T I_plus_button,
	boolean_T I_minus_button,
	boolean_T I_time_mode,
	boolean_T I_timeset_mode,
	boolean_T I_alarm_mode,
	boolean_T I_SW_mode,
	boolean_T I_alarmonoff_button,
	boolean_T I_postpone,
	uint8_T *O_hours,
	uint8_T	*O_minutes,
	uint8_T *O_seconds,
	uint8_T *O_tenths,
	uint8_T *O_mode,
	boolean_T *O_hblink,
	boolean_T *O_mblink,
	boolean_T *O_alarm_ringing,
	boolean_T *O_alarm_onoff
);

extern void SM_end();

#endif //SWATCH_SM_H
