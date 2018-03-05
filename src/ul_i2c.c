#include "ul_i2c.h"

//Software I2C
#ifdef I2C_SOFT
    void i2c_start() {
        //prepare SDA level in case of repeated start
//        i2c_scl_0(bus);
//        i2c_sda_1(bus); i2c_delay_bit();
        //1-0 transition on SDA while SCL=1
        i2c_scl_1(); i2c_delay_bit();
        i2c_sda_0(); i2c_delay_bit();
        i2c_scl_0(); i2c_delay_bit();
    }
    void i2c_stop() {
        //0-1 transition on SDA while SCL=1
        i2c_sda_0(); i2c_delay_bit();
        i2c_scl_1(); i2c_delay_bit();
        i2c_sda_1(); i2c_delay_bit();
    }
    void i2c_wr_bit(U8 b) {
        if (b) i2c_sda_1(); else i2c_sda_0();
        i2c_delay_bit();
        i2c_scl_1(); i2c_delay_bit();
        i2c_scl_0();
        i2c_sda_1(); i2c_delay_bit();
    }
    U8 i2c_rd_bit() {
        i2c_sda_1(); //i2c_delay_bit();
        i2c_scl_1(); i2c_delay_bit();
        U8 bit = i2c_sda();
        i2c_scl_0(); i2c_delay_bit();
        return(bit);
    }
    U8 i2c_wr_U8(U8 b) {
        for (U8 i=8; i; i--) {
            i2c_wr_bit(b & 0x80);
            b <<= 1;
        }
        i2c_delay_bit();
        return(i2c_rd_bit()); //return ACK
    }
    U8 i2c_rd_U8(U8 ack) {
        U8 result = 0;
        for (U8 i=8; i; i--) {
            result <<= 1;
            result |= !!(i2c_rd_bit());
        }
        i2c_wr_bit(ack);
        return(result);
    }
    void setup_i2c() {
        i2c_scl_1();
        i2c_sda_1();
    }
#endif

U8 i2c_chkack(U8 addr) {
    if (!i2c_sda() || !i2c_scl()) return 1;
    i2c_start();
    U8 ack = !!i2c_wr_U8(addr); //device address in write mode
    i2c_stop();
    return(ack); //ACK received ? 0 : 1
}

void i2c_wr_a8_buf8(U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size) {
    i2c_start();
    i2c_wr_U8(addr_i2c);
    i2c_wr_U8(addr_start);
    while (buf_size--)
        i2c_wr_U8(*buf++);
    i2c_stop();
}

void i2c_rd_a8_buf8(U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size) {
    i2c_start();
    i2c_wr_U8(addr_i2c);
    i2c_wr_U8(addr_start);
    if (buf_size) {
        i2c_start();
        i2c_wr_U8(addr_i2c | 1);
        while (buf_size--)
            *buf++ = i2c_rd_U8(buf_size == 0);
    }
    i2c_stop();
}
