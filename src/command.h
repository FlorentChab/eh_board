#ifndef COMMAND_
#define COMMAND_

#include "timers.h"
#include "d2a.h"
#include "a2d.h"
#include "i2c.h"

#define SET_PWM				   1
#define SET_CDS_VBP2		   2
#define SET_AD8235_VREF		   3
#define READ_PIXEL_OUT		   4
#define READ_EH_OUT			   5
#define READ_AD8235_OUT		   6
#define ACQUIRE_IMAGE		   7
#define READ_IMAGE			   8
#define SET_INTEG_TIME		   9
#define RST_STATE_MACHINE      10
#define SET_FREE_RUNNING       11
#define CLEAR_FREE_RUNNING     12
#define SET_MANUAL_FSM_DEBUG   13
#define CLEAR_MANUAL_FSM_DEBUG 14
#define ADVANCE_FSM_STATE      15

//function prototypes 
uint16_t execute_command(uint8_t *, uint8_t *);

//--------------------------------------------------------------------------
// decodes command received in the command_buffer array and executes it
//--------------------------------------------------------------------------
uint16_t execute_command(uint8_t *data_buffer, uint8_t *data_ptr)
{
    uint8_t  command;	
    uint16_t dac_mV;
    uint16_t adc_result;
	uint16_t data_length=0;
   
    command = data_buffer[0];	
	switch (command)
	{
		case SET_PWM:
		   pwm_frequency = data_buffer[2]*65536 + data_buffer[3]*256 + data_buffer[4];
//		   pwm_frequency = (data_buffer[2]<<16) + (data_buffer[3]<<8) + data_buffer[4];
		   pwm_dutycycle = data_buffer[1];
		   set_pwm(pwm_frequency, pwm_dutycycle);
		   break;
						
		case SET_CDS_VBP2:
		   dac_mV = data_buffer[1]*256 + data_buffer[2];
		   dac_chan_write(&dac_instance, CDS_VBP2, mV_to_int(dac_mV) );
		   break;
						
		case SET_AD8235_VREF:
		   dac_mV = data_buffer[1]*256 + data_buffer[2];
		   dac_chan_write(&dac_instance, AD8235_VREF, mV_to_int(dac_mV) );
		   break;
						
		case READ_PIXEL_OUT:
		   set_adc_channel(PIXEL_OUT);
		   adc_result = read_adc();
		   data_buffer[0] = command;
		   data_buffer[1] = (adc_result >>    8); // high byte
		   data_buffer[2] = (adc_result &  0xFF); // low byte
		   data_length = 3;
		   data_ptr = data_buffer;
		   break;					
						
		case READ_EH_OUT:
		   set_adc_channel(EH_OUT);
		   adc_result = read_adc();
		   data_buffer[0] = command;
		   data_buffer[1] = (adc_result >>    8); // high byte
		   data_buffer[2] = (adc_result &  0xFF); // low byte
		   data_length = 3;
		   data_ptr = data_buffer;
		   break;
						
		case AD8235_OUT:
		   set_adc_channel(AD8235_OUT);
		   adc_result = read_adc();
		   data_buffer[0] = command;
		   data_buffer[1] = (adc_result >>    8); // high byte
		   data_buffer[2] = (adc_result &  0xFF); // low byte   
		   data_length = 3;
		   data_ptr = data_buffer;
		   break;
			
	    case ACQUIRE_IMAGE:
		   trigger_image_acq = 1;
		   break;		

	    case READ_IMAGE:
		   data_length = image_array_size;
		   data_ptr = image_array;
	       break;
		   
		case SET_INTEG_TIME:
		   integration_time = (data_buffer[1]<<16) + (data_buffer[2]<<8) + data_buffer[3];
		   break;   
		   
		case RST_STATE_MACHINE:						// added 07/12/17 FC
		    fsm_state = RESET;
			controller_fsm(&fsm_state);
			break;
			
		case SET_FREE_RUNNING :						
			free_running_image_acq = 1;
			break;
		
		case CLEAR_FREE_RUNNING :
			free_running_image_acq = 0;
			break;
		
		case SET_MANUAL_FSM_DEBUG :
			manual_fsm_debugging = 1;
			break;
		
		case CLEAR_MANUAL_FSM_DEBUG :
			manual_fsm_debugging = 0;
			break;
		
		case ADVANCE_FSM_STATE :
			manual_fsm_advance = 1;
			trigger_image_acq  = 1;
			break;

/*																	
		case state_PROVIDEIMAGE:
            i2cpacketlength = packet_size+1;
            packet_index = read_buffer[1];
            start_index = packet_index*packet_size-packet_size;
                        
            for(int i = 0; i < (i2cpacketlength-1); i++)
            {
                packet_array[i] = image_array[start_index+i];
            }
                        
			packet_array[i2cpacketlength-1] = checkByte;
			write_pointer = packet_array;
			state = state_IDLE;
			break;

			// Florent 5/18/2017 /
		case state_REMOTEINTTIME:
			int_time = read_buffer[1];
			set_timer();
			state = state_IDLE;
			break;
*/						
//		case state_OPTIMIZEHARVEST:
			/*
				*	frequency fixed
				*	dutycycle variable: d
				*	EHC Vout = f(d)
				*
				*
				*	Notes:
				*		Use timer interrupt to optimize EHC Vout. Optimize every x seconds.
				*			OR
				*		Optimize every state machine cycle. Probably not feasable.
				*/
//			state = state_IDLE;
//			break;				
						
		default:
		  break;
						
     } // switch (state)			
     return data_length;
}

#endif