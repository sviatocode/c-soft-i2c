c-soft-i2c
==========

Software I2C library


Setup:
```c
//defines
#define I2C_SOFT           //use soft I2C
#define I2C_BUSES        1 //number of buses
#define I2C_BAUD  400000uL //bitrate

//bus lines manipulation
inline void i2c_scl_0(u8 bus) { DDRC |= (1<<PC4); }           //set SCL to 0
inline void i2c_scl_1(u8 bus) { DDRC &= ~(1<<PC4); }          //set SCL to 1
inline   u8 i2c_scl(u8 bus)   { return !!(PINC & (1<<PC4)); } //read SCL state

inline void i2c_sda_0(u8 bus) { DDRC |= (1<<PC3); }           //set SDA to 0
inline void i2c_sda_1(u8 bus) { DDRC &= ~(1<<PC3); }          //set SDA to 1
inline   u8 i2c_sda(u8 bus)   { return !!(PINC & (1<<PC3)); } //read SDA state

#include "ul_i2c.h"
```


Use:
```c
//check presence of DS3231
u8 present = !i2c_chkack(bus, I2C_DS3231_ADDR);

//read DS3231 memory
if (present)
	i2c_rd_a8_buf8(0, I2C_DS3231_ADDR, 0, &buf, I2C_DS3231_REGS_SIZE );

//write DS3231 memory
if (present)
	i2c_wr_a8_buf8(0, I2C_DS3231_ADDR, 0, &buf, I2C_DS3231_REGS_SIZE );
```

