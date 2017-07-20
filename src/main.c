/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#define XPLAINED_BOARD			//enable this line if working with the Xplained board

#include <asf.h>
#include "sys_io.h"
#include "i2c.h"
#include "command.h"
#include "controller.h"
#include "timers.h"
#include "d2a.h"
#include "a2d.h"

//global variables
//uint16_t pwm_frequency, pwm_dutycycle;
//uint32_t integration_time      = 10000;			// pixel integration time in micro-seconds
//uint32_t controller_fsm_period = 1000;			// time between states in controller fsm
//extern uint8_t fsm_state;


int main (void)
{
	int led_toggle = 0;
	int button_pressed = 0, button_pressed_1 = 0;

	system_init();
	configure_gclock_generator();
	configure_port_pins();
	i2c_initialization();
	dac_initialization();
	adc_initialization();


    dac_chan_write(&dac_instance, CDS_VBP2, mV_to_int(1000) );
    dac_chan_write(&dac_instance, AD8235_VREF, mV_to_int(1500) );


    fsm_state        = RESET;
	default_fsm_wait = 1500;
	integration_time = 20000;
	configure_timers();
	initialize_timers();
	
	free_running_image_acq = 1;
	
	uint8_t i;
	
	for (i=0 ; i < image_array_size ; i++ )
	{
		image_array[i] = 77;
	}
	
	while (1) {
		
		#ifdef XPLAINED_BOARD
		  if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {   // is button pressed
			  button_pressed = 1;
		  } else button_pressed = 0;
		
		  if (button_pressed == 1 && button_pressed_1 == 0) {
		    led_toggle ^=1;
		
		    if (led_toggle == 1) {
			  port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);			// turn on LED
		    } else {
			  port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);		    // turn off LED
		    }
		  }
		  button_pressed_1 = button_pressed;
		#endif
	}
}
