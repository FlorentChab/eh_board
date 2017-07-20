
#ifndef SYS_IO_H_
#define SYS_IO_H_

#define NUM_COLS		4 //64
#define NUM_ROWS		4 //64

// IO
#define	ROW_SR_SET		PIN_PA10
#define	ROW_SR_RST  	PIN_PA11
	
#define	RST_SR_D		PIN_PA17
#define	RST_SR_CLK		PIN_PA15
#define	RST_SR_SET		PIN_PA12
#define	RST_SR_RST  	PIN_PA13

#define	CDS_PHI1		PIN_PA16
#define	CDS_PHI2		PIN_PA20

#ifdef XPLAINED_BOARD
 #define	ROW_SR_D	PIN_PB16
 #define	ROW_SR_CLK	PIN_PB17
 #define	MUX_SR_D	PIN_PA27	
 #define	MUX_SR_SET	PIN_PB23  
 #define	MUX_SR_RST  PIN_PA21	
 #define	LED_POWER	PIN_PB30
#else
 #define	ROW_SR_D	PIN_PA09  //conflicts with SERCOM2 I2C SCL pin used on Xplained
 #define	ROW_SR_CLK	PIN_PA08  //conflicts with SERCOM2 I2C SDA pin used on Xplained
 #define	MUX_SR_D	PIN_PA25  //not in Xplained header
 #define	MUX_SR_SET	PIN_PA23  //not in Xplained header
 #define	MUX_SR_RST  PIN_PA22  //not in Xplained header
 #define	LED_POWER   PIN_PB31  //not in Xplained header
#endif
	
#define	MUX_SR_CLK		PIN_PB22
#define AD8235_POWER	PIN_PB08
#define	EH_25			PIN_PB13
#define	EH_50			PIN_PB14
#define	EH_75			PIN_PB15
#define	EH_100			PIN_PB10
	
#define AD8235_VREF		DAC_CHANNEL_0		//PA02
#define CDS_VBP2		DAC_CHANNEL_1		//PA05

// pins used by the ADC (specified in a2d.h):
// PIXEL_OUT  -> PA03   (channel 1)
// AD8235_OUT -> PA06   (channel 6)
// EH_OUT     -> PB01   (channel 9)

// pins used for I2C comms (specified in i2c.h):
// PA00 -> SDA
// PA01 -> SCL
// PA08 -> SDA for Xplained
// PA09 -> SCL for Xplained
//

// pin used for PWM output (specified in timers.h):
// PA18 for Xplained
// PA14 otherwise

// PB05 used in Xplained as digital output for debugging purposes, i.e. blinking
	
//function prototypes
void configure_gclock_generator(void);
void configure_port_pins(void);
void configure_gclock_channel(void);

//declarations
struct system_gclk_gen_config gclock_gen_conf;

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void configure_gclock_generator(void) 
{
  system_gclk_gen_get_config_defaults(&gclock_gen_conf);
  gclock_gen_conf.source_clock    = SYSTEM_CLOCK_SOURCE_OSC16M;
  gclock_gen_conf.division_factor = 2;
  system_gclk_gen_set_config(GCLK_GENERATOR_1, &gclock_gen_conf);
  system_gclk_gen_enable(GCLK_GENERATOR_1);
}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void configure_port_pins(void) 
{
  struct port_config config_port_pin;
  port_get_config_defaults(&config_port_pin);
  config_port_pin.direction = PORT_PIN_DIR_OUTPUT; 

  port_pin_set_config(ROW_SR_SET, &config_port_pin);
  port_pin_set_config(ROW_SR_RST, &config_port_pin);
  port_pin_set_config(RST_SR_D, &config_port_pin);
  port_pin_set_config(RST_SR_CLK, &config_port_pin);
  port_pin_set_config(RST_SR_SET, &config_port_pin);
  port_pin_set_config(RST_SR_RST, &config_port_pin);
  port_pin_set_config(CDS_PHI1, &config_port_pin);
  port_pin_set_config(CDS_PHI2, &config_port_pin);
 
  port_pin_set_config(ROW_SR_D, &config_port_pin);
  port_pin_set_config(ROW_SR_CLK, &config_port_pin);
  port_pin_set_config(MUX_SR_D, &config_port_pin);
  port_pin_set_config(MUX_SR_SET, &config_port_pin);
  port_pin_set_config(MUX_SR_RST, &config_port_pin);
  port_pin_set_config(LED_POWER, &config_port_pin);

  port_pin_set_config(MUX_SR_CLK, &config_port_pin);
  port_pin_set_config(AD8235_POWER, &config_port_pin);
  port_pin_set_config(EH_25, &config_port_pin);
  port_pin_set_config(EH_50, &config_port_pin);
  port_pin_set_config(EH_75, &config_port_pin);
  port_pin_set_config(EH_100, &config_port_pin);
  
  #ifdef XPLAINED_BOARD
   port_pin_set_config(PIN_PB05, &config_port_pin);
  #endif

}

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void configure_gclock_channel(void)
{
	// Not necessary?
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = GCLK_GENERATOR_1;

	system_gclk_chan_set_config(TC4_GCLK_ID, &gclk_chan_conf);
	system_gclk_chan_enable(TC4_GCLK_ID);
}

#endif