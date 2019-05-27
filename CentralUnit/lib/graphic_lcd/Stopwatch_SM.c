#include "Stopwatch_SM.h"
void SM_init(
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
) {
	struct SM_Output tmp = {O_hours, O_minutes, O_seconds, O_tenths, O_mode, O_hblink, O_mblink, O_alarm_ringing, O_alarm_onoff};
	SM_Output = tmp;

	SM_local.hours = 0;
	SM_local.minutes = 0;
	SM_local.seconds = 0;
	SM_local.tenths = 0;
	SM_local.tmp_hours = 0;
	SM_local.tmp_minutes = 0;
	SM_local.A_hours = 0;
	SM_local.A_minutes = 0;
	SM_local.A_act_min = 0;
	SM_local.SW_hours = 0;
	SM_local.SW_minutes = 0;
	SM_local.SW_seconds = 0;
	SM_local.SW_tenths = 0;
	SM_local.SW_lapmode = 0;

	Control_init();
	TimeCount_init();
	AlarmMng_init();
}

Signal_t setEvent_Signal(boolean_T I_tick, boolean_T I_plus_button, boolean_T I_minus_button, boolean_T I_time_mode,
						boolean_T I_timeset_mode, boolean_T I_alarm_mode,	boolean_T I_SW_mode, boolean_T I_alarmonoff_button,	boolean_T I_postpone) {
	Signal_t sig = NULL_SIG;
	// Either
	if((I_tick && !SM_Input.I_tick) || (!I_tick && SM_Input.I_tick))
		sig = TICK_E;

	//Rising
	if(I_plus_button	&& !SM_Input.I_plus_button) sig = PLUS_E;
	if(I_minus_button	&& !SM_Input.I_minus_button) sig = MINUS_E;
	if(I_time_mode		&& !SM_Input.I_time_mode) sig = TIME_MODE_E;
	if(I_timeset_mode	&& !SM_Input.I_timeset_mode) sig = TIMESET_MODE_E;
	if(I_alarm_mode		&& !SM_Input.I_alarm_mode) sig = ALARM_MODE_E;
	if(I_SW_mode		&& !SM_Input.I_SW_mode) sig = SWATCH_MODE_E;
	if(I_alarmonoff_button && !SM_Input.I_alarmonoff_button) sig = ALARM_ONOFF_E;
	if(I_postpone		&& !SM_Input.I_postpone) sig = POSTPONE_E;

	struct SM_Input tmp = {
		I_tick,
		I_plus_button,
		I_minus_button,
		I_time_mode,
		I_timeset_mode,
		I_alarm_mode,
		I_SW_mode,
		I_alarmonoff_button,
		I_postpone
	};
	SM_Input = tmp;	// save actual input to check the next one
	return sig;
}

void SM_step(
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
){
	Signal_t sig = setEvent_Signal(I_tick, I_plus_button, I_minus_button, I_time_mode, I_timeset_mode, I_alarm_mode, I_SW_mode, I_alarmonoff_button, I_postpone);
	TimeCount_dispatch(sig);
	Control_dispatch(sig);
	AlarmMng_dispatch(sig);
}


//------------------------------------
void TimeCount_init(){
	SM_local.hours = 0;
	SM_local.minutes = 0;
	SM_local.seconds = 0;
	SM_local.tenths = 0;
	State_TimeCount = TIMECOUNT_S;
	TimeCount_dispatch(ENTRY_SIG);
}
void TimeCount_dispatch(unsigned const sig){
	switch(State_TimeCount){
		case TIMECOUNT_S:
			TimeCount_manage_TIMECOUNT_S(sig);
			break;

		default:
			break;
	}
}
void TimeCount_tran(enum State_TimeCount target){
	TimeCount_dispatch(EXIT_SIG);
	State_TimeCount = target;
	TimeCount_dispatch(ENTRY_SIG);
}

void TimeCount_manage_TIMECOUNT_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			break;
		case EXIT_SIG:
			break;

		case TICK_E:
			SM_local.tenths=(SM_local.tenths+1)%10;
			if(SM_local.tenths==0){
				SM_local.seconds=(SM_local.seconds+1)%60;
				if(SM_local.seconds==0){
					SM_local.minutes=(SM_local.minutes+1)%60;
					if(SM_local.minutes==0){
						SM_local.hours=(SM_local.hours+1)%24;
					}
				}
			}
			break;

		default:
			break;
	}
}
//------------------------------------
void AlarmMng_init(){
//en:Ahours= uint8(0);Aminutes= uint8(0);AlarmON=false;alarm_ringing=false;
	SM_local.A_hours = 0;
	SM_local.A_minutes = 0;
	*SM_Output.O_alarm_onoff = false;
	*SM_Output.O_alarm_ringing = false;
	State_AlarmMng = DISABLED_S;
	AlarmMng_dispatch(ENTRY_SIG);
}

void AlarmMng_tran(enum State_AlarmMng target){
	AlarmMng_dispatch(EXIT_SIG);
	State_AlarmMng = target;
	AlarmMng_dispatch(ENTRY_SIG);
}

void AlarmMng_tran_nested(enum State_AlarmMng target, enum State_AlarmMng parent){
	AlarmMng_dispatch(EXIT_SIG);
	State_AlarmMng = parent;
	AlarmMng_dispatch(EXIT_SIG);
	State_AlarmMng = target;
	AlarmMng_dispatch(ENTRY_SIG);
}

void AlarmMng_dispatch(unsigned const sig){
	switch(State_AlarmMng){
		case DISABLED_S:
			AlarmMng_manage_DISABLED_S(sig);
			break;
		case ENABLED_S:
			AlarmMng_manage_ENABLED_S(sig);
			break;
		case STOPPING_S:
			AlarmMng_manage_STOPPING_S(sig);
			break;
		case SILENT_S:
			AlarmMng_manage_SILENT_S(sig);
			break;
		case RINGING_S:
			AlarmMng_manage_RINGING_S(sig);
			break;
		case POSTPONE_S:
			AlarmMng_manage_POSTPONE_S(sig);
			break;

		default:
			break;
	}
}

void AlarmMng_manage_DISABLED_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			break;
		case EXIT_SIG:
			break;

		default:
			break;
	}

	if(*SM_Output.O_alarm_onoff == true){
		// my exit
		State_AlarmMng = ENABLED_S;	// cannot use tran because of loop in calling Exit on myself
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
}

void AlarmMng_manage_ENABLED_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			State_AlarmMng = SILENT_S;
			AlarmMng_dispatch(ENTRY_SIG);
			break;
		case EXIT_SIG:
			break;

		default:
			break;
	}

	if(*SM_Output.O_alarm_onoff == false){
		// my exit
		State_AlarmMng = DISABLED_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
}

void AlarmMng_manage_SILENT_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_alarm_ringing = false;
			break;
		case EXIT_SIG:
			break;

		default:
			break;
	}

	if(*SM_Output.O_alarm_onoff == false){
		// my exit
		// parent exit
		State_AlarmMng = ENABLED_S;
		AlarmMng_dispatch(EXIT_SIG);
		// new state
		State_AlarmMng = DISABLED_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
	if(SM_local.hours==SM_local.A_hours && SM_local.minutes==SM_local.A_minutes){
		SM_local.A_act_min = SM_local.minutes;
		// my exit
		State_AlarmMng = RINGING_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}

}

void AlarmMng_manage_RINGING_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			break;
		case EXIT_SIG:
			break;
		case TICK_E:
			if(SM_local.tenths==0){
				*SM_Output.O_alarm_ringing = !(*SM_Output.O_alarm_ringing);
			}
			break;

		case ALARM_ONOFF_E:
			AlarmMng_tran(STOPPING_S);
			break;
		case POSTPONE_E:
			AlarmMng_tran(POSTPONE_S);
			break;

		default:
			break;
	}

//----
	if(*SM_Output.O_alarm_onoff == false){
		// my exit
		// parent exit
		State_AlarmMng = ENABLED_S;
		AlarmMng_dispatch(EXIT_SIG);
		// new state
		State_AlarmMng = DISABLED_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
//---- 1
	if(SM_local.minutes>=((SM_local.A_minutes + ALARM_STOP_AFTER)%60)){
		// my exit
		State_AlarmMng = SILENT_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
//---- 2
	if(SM_local.minutes==((SM_local.A_act_min + POSTPONE_AFTER)%60)){
		// my exit
		State_AlarmMng = POSTPONE_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
}

void AlarmMng_manage_POSTPONE_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_alarm_ringing = false;
			SM_local.A_act_min = (SM_local.minutes + POSTPONE_MINUTES)%60;
			break;
		case EXIT_SIG:
			break;
		case ALARM_ONOFF_E:
			AlarmMng_tran(STOPPING_S);
			break;

		default:
			break;
	}	

//----
	if(*SM_Output.O_alarm_onoff == false){
		// my exit
		// parent exit
		State_AlarmMng = ENABLED_S;
		AlarmMng_dispatch(EXIT_SIG);
		// new state
		State_AlarmMng = DISABLED_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}

//---- 1
	if(SM_local.minutes==SM_local.A_act_min){
		State_AlarmMng = RINGING_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}
}

void AlarmMng_manage_STOPPING_S(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_alarm_ringing = false;
			break;
		case EXIT_SIG:
			break;

		default:
			break;
	}

//----
	if(*SM_Output.O_alarm_onoff == false){
		// my exit
		// parent exit
		State_AlarmMng = ENABLED_S;
		AlarmMng_dispatch(EXIT_SIG);
		// new state
		State_AlarmMng = DISABLED_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}

//---- 1
	if(SM_local.minutes!=SM_local.A_minutes){
		State_AlarmMng = SILENT_S;
		AlarmMng_dispatch(ENTRY_SIG);
		return;
	}

}

//------------------------------------
void Control_init(){
	*SM_Output.O_hblink=true;
	*SM_Output.O_mblink=true;
	State_Control=TIME_MODE;
	Control_dispatch(ENTRY_SIG);
}

void Control_tran(enum State_Control target){
	Control_dispatch(EXIT_SIG);
	State_Control = target;
	Control_dispatch(ENTRY_SIG);
}

void Control_tran_nested(enum State_Control target, enum State_Control parent){
	Control_dispatch(EXIT_SIG);
	State_Control = parent;
	Control_dispatch(EXIT_SIG);
	State_Control = target;
	Control_dispatch(ENTRY_SIG);
}

void Control_dispatch(unsigned const sig) {
	switch(State_Control){
		case TIME_MODE:
			Control_manage_TIME_MODE(sig);
			break;
		case TIMESET_MODE:
			Control_manage_TIME_SETMODE(sig);
			break;
		case TS_SETHOURS:
			Control_manage_TS_SETHOURS(sig);
			break;
		case TS_SETMINUTES:
			Control_manage_TS_SETMINUTES(sig);
			break;

		case ALARM_MODE:
			Control_manage_ALARM_MODE(sig);
			break;
		case A_SETHOURS:
			Control_manage_A_SETHOURS(sig);
			break;
		case A_SETMINUTES:
			Control_manage_A_SETMINUTES(sig);
			break;

		case SWATCH_MODE:
			Control_manage_SWATCH_MODE(sig);
			break;
		case SW_RUNNING:
			Control_manage_SW_RUNNING(sig);
			break;
		case SW_STOPPED:
			Control_manage_SW_STOPPED(sig);
			break;
		default:
			break;
	}
}

void Control_manage_TIME_MODE(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_mode = 0;
			break;
		case EXIT_SIG:
			break;
		case TICK_E:
			*SM_Output.O_hours = SM_local.hours;
			*SM_Output.O_minutes = SM_local.minutes;
			*SM_Output.O_seconds = SM_local.seconds;
			break;
		case TIME_MODE_E:
			break;
		case TIMESET_MODE_E:
			Control_tran(TIMESET_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(ALARM_MODE);
			break;
		case SWATCH_MODE_E:
			Control_tran(SWATCH_MODE);
			break;
		default:
			break;
	}
}
void Control_manage_TIME_SETMODE(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			// mode=1; Dhours=hours; Dminutes=minutes; tmpH=hours; tmpM=minutes;
			*SM_Output.O_mode = 1;
			*SM_Output.O_hblink = true;
			*SM_Output.O_mblink = true;
			*SM_Output.O_hours = SM_local.hours;
			*SM_Output.O_minutes = SM_local.minutes;
			SM_local.tmp_hours = SM_local.hours;
			SM_local.tmp_minutes = SM_local.minutes;
			State_Control = TS_SETHOURS;
			Control_dispatch(ENTRY_SIG);
			break;
		case EXIT_SIG:
			//hblink=true; mblink=true; minutes=tmpM; hours=tmpH;
			*SM_Output.O_hblink = true;
			*SM_Output.O_mblink = true;
			SM_local.hours = SM_local.tmp_hours;
			SM_local.minutes = SM_local.tmp_minutes;
			*SM_Output.O_hours = SM_local.tmp_hours;
			*SM_Output.O_minutes = SM_local.tmp_minutes;
			break;
		case TIME_MODE_E:
			Control_tran(TIME_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran(TIMESET_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(ALARM_MODE);
			break;
		case SWATCH_MODE_E:
			Control_tran(SWATCH_MODE);
			break;
		default:
			break;
	}
}

uint8_T inc_hours(uint8_T hours){
	return (hours+1)%24;
}
uint8_T inc_minutes(uint8_T minutes){
	return (minutes+1)%60;
}
uint8_T dec_hours(uint8_T hours){
	if(hours==0){
		hours=23;
	}else {
		hours = hours-1;
	}
	return hours;
}
uint8_T dec_minutes(uint8_T minutes){
	if(minutes==0){
		minutes=59;
	}else {
		minutes = minutes-1;
	}
	return minutes;
}
void Control_manage_TS_SETHOURS(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_hours = SM_local.tmp_hours;
			break;
		case EXIT_SIG:
			*SM_Output.O_hblink = true;
			break;
		case TICK_E:
			if(SM_local.tenths == 0){
				*SM_Output.O_hblink = !(*SM_Output.O_hblink);
			}
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, TIMESET_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran(TS_SETMINUTES);
			break;
		case ALARM_MODE_E:
			Control_tran_nested(ALARM_MODE, TIMESET_MODE);
			break;
		case SWATCH_MODE_E:
			Control_tran_nested(SWATCH_MODE, TIMESET_MODE);
			break;
		case PLUS_E:
			SM_local.tmp_hours = inc_hours(SM_local.tmp_hours);
			Control_tran(TS_SETHOURS);
			break;
		case MINUS_E:
			SM_local.tmp_hours = dec_hours(SM_local.tmp_hours);
			Control_tran(TS_SETHOURS);
			break;
		default:
			break;
	}
}
void Control_manage_TS_SETMINUTES(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_minutes = SM_local.tmp_minutes;
			break;
		case EXIT_SIG:
			*SM_Output.O_mblink = true;
			break;
		case TICK_E:
			if(SM_local.tenths == 0){
				*SM_Output.O_mblink = !*SM_Output.O_mblink;
			}
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, TIMESET_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran(TS_SETHOURS);
			break;
		case ALARM_MODE_E:
			Control_tran_nested(ALARM_MODE, TIMESET_MODE);
			break;
		case SWATCH_MODE_E:
			Control_tran_nested(SWATCH_MODE, TIMESET_MODE);
			break;
		case PLUS_E:
			SM_local.tmp_minutes = inc_minutes(SM_local.tmp_minutes);
			Control_tran(TS_SETMINUTES);
			break;
		case MINUS_E:
			SM_local.tmp_minutes = dec_minutes(SM_local.tmp_minutes);
			Control_tran(TS_SETMINUTES);
			break;
		default:
			break;
	}
}

void Control_manage_ALARM_MODE(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			//entry:mode=uint8(2);Dhours=Ahours;Dminutes=Aminutes;hblink=true;mblink=true;
			*SM_Output.O_mode = 2;
			*SM_Output.O_hours = SM_local.A_hours;
			*SM_Output.O_minutes = SM_local.A_minutes;
			*SM_Output.O_hblink = true;
			*SM_Output.O_mblink = true;
			State_Control = A_SETHOURS;
			Control_dispatch(ENTRY_SIG);
			break;
		case EXIT_SIG:
			//exit:hblink=true;mblink=true;
			*SM_Output.O_hblink = true;
			*SM_Output.O_mblink = true;
			break;
		case TICK_E:
			*SM_Output.O_hours = 0;
			*SM_Output.O_minutes = 0;
			break;
		case TIME_MODE_E:
			Control_tran(TIME_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran(TIMESET_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(ALARM_MODE);
			break;
		case SWATCH_MODE_E:
			Control_tran(SWATCH_MODE);
			break;
		default:
			break;
	}
}

void Control_manage_A_SETHOURS(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_hours = SM_local.A_hours;
			break;
		case EXIT_SIG:
			*SM_Output.O_hblink = true;
			break;
		case TICK_E:
			if(SM_local.tenths == 0){
				*SM_Output.O_hblink = !(*SM_Output.O_hblink);
			}
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, ALARM_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran_nested(TIMESET_MODE, ALARM_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(A_SETMINUTES);
			break;
		case SWATCH_MODE_E:
			Control_tran_nested(SWATCH_MODE, ALARM_MODE);
			break;
		case PLUS_E:
			SM_local.A_hours = inc_hours(SM_local.A_hours);
			Control_tran(A_SETHOURS);
			break;
		case MINUS_E:
			SM_local.A_hours = dec_hours(SM_local.A_hours);
			Control_tran(A_SETHOURS);
			break;
		case ALARM_ONOFF_E:
			*SM_Output.O_alarm_onoff = !*SM_Output.O_alarm_onoff;
			break;
		default:
			break;
	}
}
void Control_manage_A_SETMINUTES(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_minutes = SM_local.A_minutes;
			break;
		case EXIT_SIG:
			*SM_Output.O_mblink = true;
			break;
		case TICK_E:
			if(SM_local.tenths == 0){
				*SM_Output.O_mblink = !(*SM_Output.O_mblink);
			}
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, ALARM_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran_nested(TIMESET_MODE, ALARM_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(A_SETHOURS);
			break;
		case SWATCH_MODE_E:
			Control_tran_nested(SWATCH_MODE, ALARM_MODE);
			break;
		case PLUS_E:
			SM_local.A_minutes = inc_minutes(SM_local.A_minutes);
			Control_tran(A_SETMINUTES);
			break;
		case MINUS_E:
			SM_local.A_minutes = dec_minutes(SM_local.A_minutes);
			Control_tran(A_SETMINUTES);
			break;
		case ALARM_ONOFF_E:
			*SM_Output.O_alarm_onoff = !*SM_Output.O_alarm_onoff;
			break;
		default:
			break;
	}
}

void Control_manage_SWATCH_MODE(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_mode = 3;
			//SWhours= uint8(0);SWminutes= uint8(0);SWseconds= uint8(0);SWtenths= uint8(0)
			SM_local.SW_hours = 0;
			SM_local.SW_minutes = 0;
			SM_local.SW_seconds = 0;
			SM_local.SW_tenths = 0;
			State_Control = SW_STOPPED;
			Control_dispatch(ENTRY_SIG);
			break;
		case EXIT_SIG:
			break;
		case TIME_MODE_E:
			Control_tran(TIME_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran(TIMESET_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran(ALARM_MODE);
			break;
		case SWATCH_MODE_E:
			SM_local.SW_hours = 0;
			SM_local.SW_minutes = 0;
			SM_local.SW_seconds = 0;
			SM_local.SW_tenths = 0;
			State_Control = SW_STOPPED;
			Control_dispatch(ENTRY_SIG);
			break;
		default:
			break;
	}
}

void Control_manage_SW_STOPPED(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			*SM_Output.O_hours = SM_local.SW_hours;
			*SM_Output.O_minutes = SM_local.SW_minutes;
			*SM_Output.O_seconds = SM_local.SW_seconds;
			*SM_Output.O_tenths = SM_local.SW_tenths;
			break;
		case EXIT_SIG:
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, SWATCH_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran_nested(TIMESET_MODE, SWATCH_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran_nested(ALARM_MODE, SWATCH_MODE);
			break;
		case SWATCH_MODE_E:
			SM_local.SW_hours = 0;
			SM_local.SW_minutes = 0;
			SM_local.SW_seconds = 0;
			SM_local.SW_tenths = 0;
			Control_tran(SW_STOPPED);
			break;
		case PLUS_E:
			Control_tran(SW_RUNNING);
			break;

		default:
			break;
	}
}
void Control_manage_SW_RUNNING(unsigned const sig){
	switch(sig){
		case ENTRY_SIG:
			SM_local.SW_lapmode=false;
			break;
		case EXIT_SIG:
			break;
		case TIME_MODE_E:
			Control_tran_nested(TIME_MODE, SWATCH_MODE);
			break;
		case TIMESET_MODE_E:
			Control_tran_nested(TIMESET_MODE, SWATCH_MODE);
			break;
		case ALARM_MODE_E:
			Control_tran_nested(ALARM_MODE, SWATCH_MODE);
			break;
		case SWATCH_MODE_E:
			SM_local.SW_hours = 0;
			SM_local.SW_minutes = 0;
			SM_local.SW_seconds = 0;
			SM_local.SW_tenths = 0;
			Control_tran(SW_STOPPED);
			break;

		case PLUS_E:
			Control_tran(SW_STOPPED);
			break;

		case MINUS_E:
			SM_local.SW_lapmode = !SM_local.SW_lapmode;
			break;

		case TICK_E:
			SM_local.SW_tenths=(SM_local.SW_tenths+1)%10;
			if(SM_local.SW_tenths==0){
				SM_local.SW_seconds=(SM_local.SW_seconds+1)%60;
				if(SM_local.SW_seconds==0){
					SM_local.SW_minutes=(SM_local.SW_minutes+1)%60;
					if(SM_local.SW_minutes==0){
						SM_local.SW_hours=(SM_local.SW_hours+1)%24;
					}
				}
			}
			// this should be in each else branch but for readibility I place heare because it is executed always
			if(!SM_local.SW_lapmode){
				*SM_Output.O_hours = SM_local.SW_hours;
				*SM_Output.O_minutes = SM_local.SW_minutes;
				*SM_Output.O_seconds = SM_local.SW_seconds;
				*SM_Output.O_tenths = SM_local.SW_tenths;
			}
			break;

		default:
			break;
	}
}
