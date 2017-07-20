#ifndef D2A_H_
#define D2A_H_

#define uC_VDD	(3.589)			// exact value of the uC supply voltage, ADC and DAC conversion accuracy depends on it

//function prototypes
void dac_initialization(void);
void configure_dac(void);
void configure_dac_channel(void);
void configure_dac_pins(void);
uint16_t mV_to_int(uint16_t);

//declarations
struct dac_module dac_instance;

//--------------------------------------------------------------------------
// initializes the DAC
//--------------------------------------------------------------------------
void dac_initialization(void)
{
	configure_dac();
	configure_dac_channel();
//	configure_dac_pins();
//	dac_enable(&dac_instance);
}

//--------------------------------------------------------------------------
// configure DAC
//--------------------------------------------------------------------------
void configure_dac(void)
{
	struct dac_config config_dac;
	dac_get_config_defaults(&config_dac);
	config_dac.reference = DAC_REFERENCE_VDDANA;
	dac_init(&dac_instance, DAC, &config_dac);
}

//--------------------------------------------------------------------------
// configure DAC channel
//--------------------------------------------------------------------------
void configure_dac_channel(void)
{
	struct dac_chan_config config_dac_chan;
	dac_chan_get_config_defaults(&config_dac_chan);
	
	dac_chan_set_config(&dac_instance, DAC_CHANNEL_0, &config_dac_chan);
	dac_chan_enable(&dac_instance, DAC_CHANNEL_0);
	
	dac_chan_set_config(&dac_instance, DAC_CHANNEL_1, &config_dac_chan);
	dac_chan_enable(&dac_instance, DAC_CHANNEL_1);
}

//--------------------------------------------------------------------------
// set output pins for DAC
//--------------------------------------------------------------------------
void configure_dac_pins(void)
{
	// DAC General
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	
	// DAC VOUT[0]
	config_pinmux.mux_position = MUX_PA02B_DAC_VOUT0; // set multiplexer pin to VOUT0
	system_pinmux_pin_set_config(PIN_PA02B_DAC_VOUT0, &config_pinmux);
	
	// DAC VOUT[1]
	config_pinmux.mux_position = MUX_PA05B_DAC_VOUT1; // set pin multiplexer pin to VOUT1
	system_pinmux_pin_set_config(PIN_PA05B_DAC_VOUT1, &config_pinmux);
}

//--------------------------------------------------------------------------
//	converts a mV value into an integer to write to a DAC channel
//	Input
//		dac_voltage_mV:	Desired voltage in mV
//		Range:			0 to uC_VDD
//
//	Output
//		dac_value:		Corresponding int value to write to a DAC channel
//--------------------------------------------------------------------------
uint16_t mV_to_int(uint16_t voltage_mV)
{
	float 		k, temp;
	uint16_t 	output_value;
	
	k = 4095/(uC_VDD*1000);
	temp = voltage_mV * k;
	output_value = ((uint16_t)temp) & 0xFFF;
	
	return output_value;
}

#endif