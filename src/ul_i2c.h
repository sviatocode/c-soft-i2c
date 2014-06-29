/** \addtogroup group_i2c I2C
  * For software I2C define functions/macros
  *     I2C_SOFT
  *     i2c_scl_0() - set SCL to 0
  *     i2c_scl_1() - set SCL to 1
  *     i2c_sda_0() - set SDA to 0
  *     i2c_sda_1() - set SDA to 1
  *     i2c_sda()   - get SDA state;
  * For AVR hardware I2C realization define constant
  *     I2C_AVR
  */

#ifndef UL_I2C_H_
    #define UL_I2C_H_

#ifdef I2C_SOFT
	#define i2c_delay_bit() delay_us(1000000UL / I2C_BAUD / 2)

    //START condition
    void i2c_start(U8 bus);
    //STOP condition
    void i2c_stop(U8 bus);
    //write 1 bit to I2C bus
    void i2c_wr_bit(U8 bus, U8 b);
    //read 1 bit from I2C bus to 0th bit of result
    U8 i2c_rd_bit(U8 bus);
    //write 1 byte to I2C, returns ACK bit
    U8 i2c_wr_U8(U8 bus, U8 b);
    //read 1 byte from I2C, then write ACK bit
    U8 i2c_rd_U8(U8 bus, U8 ack);
#endif

//to include AVR hardware I2C realization define use_i2c_avr (если нужно, подключить функции аппаратного I2C AVR)
#ifdef I2C_AVR

	#define I2C_STATUS_MASK 0xF8
    enum {
    	I2C_CMD_START   = ((1 << TWINT)|(1 << TWEN)|(1 << TWSTA)),
		I2C_CMD_STOP    = ((1 << TWINT)|(1 << TWEN)|(1 << TWSTO)),
		I2C_CMD_WR      = ((1 << TWINT)|(1 << TWEN)),
		I2C_CMD_RD_ACK  = ((1 << TWINT)|(1 << TWEN)|(1 << TWEA)),
		I2C_CMD_RD_NACK = ((1 << TWINT)|(1 << TWEN))
    };

    inline U8 i2c_busy(U8 bus)    { return (!(TWCR & (1 << TWINT))); }
    inline void i2c_start(U8 bus) { i2c_cmd_wait(I2C_CMD_START); }
    inline void i2c_stop(U8 bus)  { i2c_cmd(I2C_CMD_STOP); delay_us(5); }

    //wait for AVR TWI action to end
    void i2c_wait(U8 bus);
    //execute TWI command cmd
    void i2c_cmd(U8 bus, U8 cmd);
    //process AVR TWI command: TWCR=action, wait, return status of operation
    U8 i2c_cmd_wait(U8 bus, U8 cmd);
    //write 1 byte to I2C, return ACK bit
    U8 i2c_wr_U8(U8 bus, U8 b);
    //read 1 byte from I2C, then write ACK bit
    U8 i2c_rd_U8(U8 bus, U8 ack);
#endif


void setup_i2c();

//check presence or busy state of I2C device with bus address i2c_addr, return 0 if ack received, else status code
U8 i2c_chkack(U8 bus, U8 addr);

//write buf_size bytes from *buf starting from 8-bit addr_start to device with 8-bit address addr_i2c
U8 i2c_wr_a8_buf8(U8 bus, U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size);

//read buf_size bytes to *buf starting from 8-bit addr_start to device with 8-bit address addr_i2c
U8 i2c_rd_a8_buf8(U8 bus, U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size);

// DS1307 definitions
#define I2C_DS1307_ADDR      0xd0
#define I2C_DS1307_SIZE      0x3f
#define I2C_DS1307_REGS_SIZE 0x08
#define I2C_DS1307_SECOND    0x00
#define I2C_DS1307_MINUTE    0x01
#define I2C_DS1307_HOUR      0x02
#define I2C_DS1307_DAY       0x03
#define I2C_DS1307_DATE      0x04
#define I2C_DS1307_MONTH     0x05
#define I2C_DS1307_YEAR      0x06
#define I2C_DS1307_CNTR      0x07
#define I2C_DS1307_CH        0x80
#define I2C_DS1307_12_24     0x40
#define I2C_DS1307_AM_PM     0x20
#define I2C_DS1307_OUT       0x80
#define I2C_DS1307_SQWE      0x10
#define I2C_DS1307_RS1       0x02
#define I2C_DS1307_RS0       0x01

// DS3231 definitions
#define I2C_DS3231_ADDR      0xd0
#define I2C_DS3231_SIZE      0x13
#define I2C_DS3231_REGS_SIZE 0x13
#define I2C_DS3231_SECOND    0x00
#define I2C_DS3231_MINUTE    0x01
#define I2C_DS3231_HOUR      0x02
#define I2C_DS3231_DAY       0x03
#define I2C_DS3231_DATE      0x04
#define I2C_DS3231_MONTH     0x05
#define I2C_DS3231_YEAR      0x06
#define I2C_DS3231_ALM1_SEC  0x07
#define I2C_DS3231_ALM1_MIN  0x08
#define I2C_DS3231_ALM1_HOUR 0x09
#define I2C_DS3231_ALM1_DAY  0x0a
#define I2C_DS3231_ALM1_DATE 0x0a
#define I2C_DS3231_ALM2_MIN  0x0b
#define I2C_DS3231_ALM2_HOUR 0x0c
#define I2C_DS3231_ALM2_DAY  0x0d
#define I2C_DS3231_ALM2_DATE 0x0d
#define I2C_DS3231_CNTR      0x0e
#define I2C_DS3231_STAT      0x0f
#define I2C_DS3231_AGING     0x10
#define I2C_DS3231_TEMP_MSB  0x11
#define I2C_DS3231_TEMP_LSB  0x12
#define I2C_DS3231_12_24     0x40
#define I2C_DS3231_AM_PM     0x20
#define I2C_DS3231_DY_DT     0x40
#define I2C_DS3231_EOSC      0x80
#define I2C_DS3231_BBSQW     0x40
#define I2C_DS3231_CONV      0x20
#define I2C_DS3231_RS2       0x10
#define I2C_DS3231_RS1       0x08
#define I2C_DS3231_INTCN     0x04
#define I2C_DS3231_A2IE      0x02
#define I2C_DS3231_A1IE      0x01
#define I2C_DS3231_OSF       0x80
#define I2C_DS3231_EN32KHZ   0x08
#define I2C_DS3231_BSY       0x04
#define I2C_DS3231_A2F       0x02
#define I2C_DS3231_A1F       0x01

// DS3232 definitions
#define I2C_DS3232_ADDR      0xd0
#define I2C_DS3232_SIZE      0x100
#define I2C_DS3232_REGS_SIZE 0x13

// M41T56 definitions
#define I2C_M41T56_ADDR      0xd0
#define I2C_M41T56_SIZE      0x3f
#define I2C_M41T56_REGS_SIZE 0x08
#define I2C_M41T56_SECOND    0x00
#define I2C_M41T56_MINUTE    0x01
#define I2C_M41T56_HOUR      0x02
#define I2C_M41T56_DAY       0x03
#define I2C_M41T56_DATE      0x04
#define I2C_M41T56_MONTH     0x05
#define I2C_M41T56_YEAR      0x06
#define I2C_M41T56_CNTR      0x07
#define I2C_M41T56_ST        0x80
#define I2C_M41T56_CEB       0x80
#define I2C_M41T56_CB        0x40
#define I2C_M41T56_OUT       0x80
#define I2C_M41T56_FT        0x40
#define I2C_M41T56_S         0x20

// PCF8563 definitions
#define I2C_PCF8563_ADDR      0xa2
#define I2C_PCF8563_SIZE      0x0f
#define I2C_PCF8563_REGS_SIZE 0x0f
#define I2C_PCF8563_SECOND    0x02
#define I2C_PCF8563_MINUTE    0x03
#define I2C_PCF8563_HOUR      0x04
#define I2C_PCF8563_DAY       0x05
#define I2C_PCF8563_WEEKDAY   0x06
#define I2C_PCF8563_MONTH     0x07
#define I2C_PCF8563_YEAR      0x08
#define I2C_PCF8563_ALM_MIN   0x09
#define I2C_PCF8563_ALM_HOUR  0x0a
#define I2C_PCF8563_ALM_DAY   0x0b
#define I2C_PCF8563_ALM_WEEKDAY 0x0c
#define I2C_PCF8563_CNTR1     0x00
#define I2C_PCF8563_CNTR2     0x01
#define I2C_PCF8563_CLKOUT    0x0d
#define I2C_PCF8563_TIMER     0x0e
#define I2C_PCF8563_TIMER_CD  0x0f
#define I2C_PCF8563_VL        0x80
#define I2C_PCF8563_C         0x80
#define I2C_PCF8563_TEST1     0x80
#define I2C_PCF8563_STOP      0x20
#define I2C_PCF8563_TESTC     0x08
#define I2C_PCF8563_TI_TP     0x10
#define I2C_PCF8563_AF        0x08
#define I2C_PCF8563_TF        0x04
#define I2C_PCF8563_AIE       0x02
#define I2C_PCF8563_TIE       0x01
#define I2C_PCF8563_FE        0x80
#define I2C_PCF8563_FD1       0x02
#define I2C_PCF8563_FD0       0x01
#define I2C_PCF8563_TE        0x80
#define I2C_PCF8563_TD1       0x02
#define I2C_PCF8563_TD0       0x01

//TDA7313 definitions
#define I2C_TDA7313_ADDR      0x88
#define I2C_TDA7313_VOL_MAX   0b00111111
#define I2C_TDA7313_VOL       0b00000000
#define I2C_TDA7313_CHN_MAX   0b00011111
#define I2C_TDA7313_LF        0b10000000
#define I2C_TDA7313_RF        0b10100000
#define I2C_TDA7313_LR        0b11000000
#define I2C_TDA7313_RR        0b11100000
#define I2C_TDA7313_ASWITCH   0b01000000
#define I2C_TDA7313_TONE_MAX  0b00001111
#define I2C_TDA7313_BASS      0b01100000
#define I2C_TDA7313_TREBLE    0b01110000

#endif
