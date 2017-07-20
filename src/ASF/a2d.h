#ifndef A2D_H_
#define A2D_H_

#define ADC_CHANNEL_0	0
#define ADC_CHANNEL_1	1
#define ADC_CHANNEL_2	2
#define ADC_CHANNEL_3	3
#define ADC_CHANNEL_4	4
#define ADC_CHANNEL_5	5
#define ADC_CHANNEL_6	6
#define ADC_CHANNEL_7	7
#define ADC_CHANNEL_8	8
#define ADC_CHANNEL_9	9

#define	PIXEL_OUT	ADC_CHANNEL_1
#define AD8235_OUT	ADC_CHANNEL_6
#define EH_OUT		ADC_CHANNEL_9

//function prototypes
void adc_initialization(void);
void configure_adc(void);
void configure_adc_pins(void);
uint16_t read_adc_channel(uint8_t);
void set_adc_channel(uint8_t);
uint16_t read_adc(void);

//declarations
struct adc_module adc_instance;
struct adc_config config_adc;

//--------------------------------------------------------------------------
// initializes the ADC
//--------------------------------------------------------------------------
void adc_initialization(void)
{
	configure_adc();
	configure_adc_pins();
}

//--------------------------------------------------------------------------
// configures the ADC
//--------------------------------------------------------------------------
void configure_adc(void)
{
	adc_get_config_defaults(&config_adc);
	config_adc.positive_input = ADC_CHANNEL_0;
    
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
    //config_adc.reference       = ADC_REFERENCE_INTVCC1;
    config_adc.reference       = ADC_REFERENCE_INTVCC2;   //set conversion reference to VDDANA
		
	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}

//--------------------------------------------------------------------------
// configures ADC pins 
//--------------------------------------------------------------------------
void configure_adc_pins(void)
{
	// ADC general
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	
	// PIXEL_OUT
	config_pinmux.mux_position = MUX_PA03B_ADC_AIN1;
	system_pinmux_pin_set_config(PIN_PA03B_ADC_AIN1, &config_pinmux);
	
	// AD8235_OUT
	config_pinmux.mux_position = MUX_PA06B_ADC_AIN6;
	system_pinmux_pin_set_config(PIN_PA06B_ADC_AIN6, &config_pinmux);
	
	// EH_OUT, not available in SAML21G18B
	//config_pinmux.mux_position = MUX_PB01B_ADC_AIN9;
	//system_pinmux_pin_set_config(PIN_PB01B_ADC_AIN9, &config_pinmux);
}

//--------------------------------------------------------------------------
// sets the ADC input to one of its several channels
//--------------------------------------------------------------------------
void set_adc_channel(uint8_t channel)
{
	adc_disable(&adc_instance);
	adc_get_config_defaults(&config_adc);
	switch(channel) {	
	   case ADC_CHANNEL_0:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN0;
	      break;
	   case ADC_CHANNEL_1:	
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN1;
		  break;
	   case ADC_CHANNEL_2:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN2;
	      break;
	   case ADC_CHANNEL_3:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN3;
	      break;
	   case ADC_CHANNEL_4:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN4;
	      break;
	   case ADC_CHANNEL_5:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN5;
	      break;
	   case ADC_CHANNEL_6:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN6;
	      break;
	   case ADC_CHANNEL_7:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN7;
	      break;
	   case ADC_CHANNEL_8:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN8;
	      break;
	   case ADC_CHANNEL_9:
	      config_adc.positive_input = ADC_POSITIVE_INPUT_PIN9;
	      break;
	   default:	  
          break;		  
	}
	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}

//--------------------------------------------------------------------------
// starts an analog-to-digital conversion and returns result of conversion
//--------------------------------------------------------------------------
uint16_t read_adc(void)
{
	uint16_t result;

	adc_start_conversion(&adc_instance);
	do {
		// wait for conversion to be done and read out result */
	} while (adc_read(&adc_instance, &result) == STATUS_BUSY);
	
	return result;
}

#endif
