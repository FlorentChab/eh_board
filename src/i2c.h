
#ifndef I2C_H_
#define I2C_H_

#include "command.h"

#ifdef XPLAINED_BOARD
 #define SLAVE_SINGLE_ADDRESS	(0x28)	// slave address of Xplained board
#else
 #define SLAVE_SINGLE_ADDRESS	(0x08)	// slave address of eh_board
#endif

#define SLAVE_MULTI_ADDRESS		(0x77)	// multicast address
#define I2C_MAX_DATA_LENGTH	    (64)    // maximum command packet length from I2C bus

//declarations
uint8_t  i2c_data_buffer[I2C_MAX_DATA_LENGTH]= {0};
uint16_t i2c_data_length = I2C_MAX_DATA_LENGTH;
uint8_t  *i2c_data_ptr;
uint8_t toggle=0;

struct i2c_slave_module i2c_slave_instance;
struct i2c_slave_packet i2c_packet;

//function prototypes
void i2c_initialization(void);
void configure_i2c_slave(void);
void configure_i2c_pins(void);
void i2c_read_request_callback(struct i2c_slave_module *const module);
void i2c_write_request_callback(struct i2c_slave_module *const module);
void configure_i2c_slave_callbacks(void);
void i2c_write_complete_callback(struct i2c_slave_module *const module);
void i2c_read_complete_callback(struct i2c_slave_module *const module);

//------------------------------------------------------------------------------------------
// Configures I2C bus
//------------------------------------------------------------------------------------------
void i2c_initialization(void)
{
	configure_i2c_pins();
	configure_i2c_slave();
	configure_i2c_slave_callbacks();
}

//------------------------------------------------------------------------------------------
// Configures I2C peripheral
//------------------------------------------------------------------------------------------
void configure_i2c_slave(void)
{
	// create and initialize config_i2c_slave structure
	struct i2c_slave_config config_i2c_slave;
	i2c_slave_get_config_defaults(&config_i2c_slave);
	
	// change address and address_mode
	config_i2c_slave.address        = SLAVE_SINGLE_ADDRESS;
	config_i2c_slave.address_mask	= SLAVE_MULTI_ADDRESS;
	config_i2c_slave.address_mode   = I2C_SLAVE_ADDRESS_MODE_TWO_ADDRESSES;
	config_i2c_slave.buffer_timeout = 1000;
	
	// initialize and enable device with config_i2c_slave
	#ifdef XPLAINED_BOARD
	 i2c_slave_init(&i2c_slave_instance, SERCOM2, &config_i2c_slave);
	#else
	 i2c_slave_init(&i2c_slave_instance, SERCOM1, &config_i2c_slave);
	#endif 
	i2c_slave_enable(&i2c_slave_instance);
}

void configure_i2c_pins(void)
{
	// I2C general
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	
	// I2C SERCOM1/PAD[0] - SDA
	#ifdef XPLAINED_BOARD
	 config_pinmux.mux_position = MUX_PA08D_SERCOM2_PAD0;
	 system_pinmux_pin_set_config(PIN_PA08D_SERCOM2_PAD0, &config_pinmux);
	#else
	 config_pinmux.mux_position = MUX_PA00D_SERCOM1_PAD0;
	 system_pinmux_pin_set_config(PIN_PA00D_SERCOM1_PAD0, &config_pinmux);
	#endif 
	
	// I2C SERCOM1/PAD[1] - SCL
	#ifdef XPLAINED_BOARD
	 config_pinmux.mux_position = MUX_PA09D_SERCOM2_PAD1;
	 system_pinmux_pin_set_config(PIN_PA09D_SERCOM2_PAD1, &config_pinmux);
	#else
	 config_pinmux.mux_position = MUX_PA01D_SERCOM1_PAD1;
	 system_pinmux_pin_set_config(PIN_PA01D_SERCOM1_PAD1, &config_pinmux);
	#endif 
}

//---------------------------------------------------------------------------------------
// register and enable callback functions 
//---------------------------------------------------------------------------------------
void configure_i2c_slave_callbacks(void)
{
	// register and enable callback functions
	i2c_slave_register_callback(&i2c_slave_instance, i2c_read_request_callback,
	I2C_SLAVE_CALLBACK_READ_REQUEST);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_READ_REQUEST);
	i2c_slave_register_callback(&i2c_slave_instance, i2c_write_request_callback,
	I2C_SLAVE_CALLBACK_WRITE_REQUEST);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_WRITE_REQUEST);
	
	// added by Thomas
	i2c_slave_register_callback(&i2c_slave_instance, i2c_write_complete_callback,
	I2C_SLAVE_CALLBACK_WRITE_COMPLETE);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_WRITE_COMPLETE);
	
	i2c_slave_register_callback(&i2c_slave_instance, i2c_read_complete_callback,
	I2C_SLAVE_CALLBACK_READ_COMPLETE);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_READ_COMPLETE);
}

//-----------------------------------------------------------------------------------
// callback function triggered when the I2C Master performs a READ operation --> slave must WRITE
//-----------------------------------------------------------------------------------
void i2c_read_request_callback(struct i2c_slave_module *const module)
{
	i2c_packet.data_length = i2c_data_length;
	i2c_packet.data        = i2c_data_ptr;	  // i2c_data_buffer;
	i2c_slave_write_packet_job(module, &i2c_packet);						// write buffer to master 
	//set_pwm(35000, 75);
}

//-----------------------------------------------------------------------------------
// function called after a WRITE operation has been completed
// i.e. after the Master has finished READING from the bus
//-----------------------------------------------------------------------------------
void i2c_write_complete_callback(struct i2c_slave_module *const module)
{
	// not used so far
	toggle ^= 1;
	#ifdef XPLAINED_BOARD
      if (toggle == 1) {
	    port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);			// turn on LED
	  } else {
	    port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);		// turn off LED
      }
    #endif
}

//-----------------------------------------------------------------------------------
// callback function triggered when the I2C Master performs a WRITE operation --> slave must READ
//-----------------------------------------------------------------------------------
void i2c_write_request_callback(struct i2c_slave_module *const module)
{
	i2c_packet.data_length = I2C_MAX_DATA_LENGTH;
	i2c_packet.data        = i2c_data_buffer;
	if (i2c_slave_read_packet_job(module, &i2c_packet) != STATUS_OK) {		// read buffer from master
  	  // do something here in case of an error 
	}
	
}

//-----------------------------------------------------------------------------------
// function called after a READ operation has been completed
// i.e. after the Master has finished WRITING to the bus
//-----------------------------------------------------------------------------------
void i2c_read_complete_callback(struct i2c_slave_module *const module)
{
    i2c_data_length = execute_command(i2c_data_buffer, i2c_data_ptr);
}


#endif