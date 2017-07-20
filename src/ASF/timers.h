#ifndef TIMERS_H_
#define TIMERS_H_

#include <math.h>
#include "controller.h"

#define PWM_CLK_FREQ	(16000000.0)	// CLK frequency of the TC module used to generate PWM signal

//declarations
struct tcc_module tcc0_instance;
struct tcc_config config_tcc0;
struct tc_module tc4_instance;
struct tc_config config_tc4;
uint16_t pwm_frequency, pwm_dutycycle;
uint32_t tcc0_start_count = 0;
uint32_t req_fsm_wait;

//function prototypes
void configure_timers(void);
void initialize_timers(void);
void configure_tc4(void);
void set_pwm(uint32_t, uint8_t);
void configure_tcc0(void);
void configure_tcc0_callbacks(void);
void tcc0_callback(struct tcc_module *);

//-------------------------------------------------------------------------------------
// configure timers but don't initialize them yet
//-------------------------------------------------------------------------------------
void configure_timers(void)
{
    configure_tc4();
    configure_tcc0();
}

//-------------------------------------------------------------------------------------
// initialize the timers used in application
//-------------------------------------------------------------------------------------
void initialize_timers(void)
{
	tc_init(&tc4_instance, TC4, &config_tc4);
	tc_enable(&tc4_instance);

    tcc0_start_count = 0xFFFFFF - default_fsm_wait;				//start TCC0 with some delay to trigger first call to callback
	config_tcc0.counter.count = tcc0_start_count;

    tcc_init(&tcc0_instance, TCC0, &config_tcc0);
    tcc_enable(&tcc0_instance);

    configure_tcc0_callbacks();	
}
 
//-------------------------------------------------------------------------------------
// configure timer TC4 to generate a PWM signal
//-------------------------------------------------------------------------------------
void configure_tc4(void)
{
	tc_get_config_defaults(&config_tc4);
	config_tc4.counter_size    = TC_COUNTER_SIZE_8BIT;
	config_tc4.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config_tc4.clock_source    = GCLK_GENERATOR_1;
	config_tc4.counter_8_bit.compare_capture_channel[0] = (20);
	config_tc4.counter_8_bit.period = 200;
	config_tc4.pwm_channel[0].enabled = true;
	#ifdef XPLAINED_BOARD
	 config_tc4.pwm_channel[0].pin_out = PIN_PA18E_TC4_WO0;
	 config_tc4.pwm_channel[0].pin_mux = MUX_PA18E_TC4_WO0;
	#else
	 config_tc4.pwm_channel[0].pin_out = PIN_PA14E_TC4_WO0;
	 config_tc4.pwm_channel[0].pin_mux = MUX_PA14E_TC4_WO0;
	#endif
}

//-------------------------------------------------------------------------------------
//	sets PWM frequency and dutycycle
//	inputs:
//		frequency:	desired frequency of the PWM in Hz
//			range:	more than 15685 Hz
//		dutycycle:	desired dutycycle of the PWM in %
//			range:	0 to 100
//-------------------------------------------------------------------------------------
void set_pwm(uint32_t frequency, uint8_t dutycycle)
{
	uint8_t pwm_div = 1;
	uint8_t pwm_per;
	uint8_t pwm_ccc;
	
	if ( (frequency >= 15686) && (frequency < 20915) )
	{
		pwm_div = 4;
	} 
	else if ( (frequency >= 20915) && (frequency < 31372) )
	{
		pwm_div = 3;
	}
	else if ( (frequency >= 31372) && (frequency < 62745) )
	{
		pwm_div = 2;
	}
	else if (frequency >= 62745) 
	{
		pwm_div = 1;
	}
	
	pwm_per = (uint8_t)round(PWM_CLK_FREQ/(pwm_div * frequency));
	pwm_ccc = (uint8_t)round((dutycycle/100.0) * pwm_per);
	
	tc_disable(&tc4_instance);
	config_tc4.counter_8_bit.compare_capture_channel[0] = (pwm_ccc);
	config_tc4.counter_8_bit.period = pwm_per;
	tc_init(&tc4_instance, TC4, &config_tc4);
	tc_enable(&tc4_instance);
	
	system_gclk_gen_disable(GCLK_GENERATOR_1);
	gclock_gen_conf.division_factor = pwm_div;
	system_gclk_gen_set_config(GCLK_GENERATOR_1, &gclock_gen_conf);
	system_gclk_gen_enable(GCLK_GENERATOR_1);
	
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void configure_tcc0(void)
{
    tcc_get_config_defaults(&config_tcc0, TCC0);
    config_tcc0.counter.clock_source    = GCLK_GENERATOR_0;
    config_tcc0.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV4;
    config_tcc0.counter.period  = 0xFFFFFF;			//can go to 24 bits on ATSAML21J18A, check ATSAML21J17B-UUT 
    config_tcc0.counter.oneshot = true;
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void configure_tcc0_callbacks(void)
{
    tcc_register_callback(&tcc0_instance, tcc0_callback, TCC_CALLBACK_OVERFLOW);
    tcc_enable_callback(&tcc0_instance, TCC_CALLBACK_OVERFLOW);
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void tcc0_callback(struct tcc_module *const module_inst)
{
	req_fsm_wait = controller_fsm(&fsm_state);

    tcc0_start_count = 0xFFFFFF - req_fsm_wait;
	tcc_set_count_value(module_inst, tcc0_start_count);
	tcc_restart_counter(module_inst);

	#ifdef XPLAINED_BOARD
	 //port_pin_toggle_output_level(PIN_PB05);
	#endif
}

#endif