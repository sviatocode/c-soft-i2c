#include "ul_i2c.h"

//Software I2C
#ifdef I2C_SOFT
    void i2c_start(U8 bus) {
        //prepare SDA level in case of repeated start
//        i2c_scl_0(bus);
//        i2c_sda_1(bus); i2c_delay_bit();
        //1-0 transition on SDA while SCL=1
        i2c_scl_1(bus); i2c_delay_bit();
        i2c_sda_0(bus); i2c_delay_bit();
        i2c_scl_0(bus); i2c_delay_bit();
    }
    void i2c_stop(U8 bus) {
        //0-1 transition on SDA while SCL=1
        i2c_sda_0(bus); i2c_delay_bit();
        i2c_scl_1(bus); i2c_delay_bit();
        i2c_sda_1(bus); i2c_delay_bit();
    }
    void i2c_wr_bit(U8 bus, U8 b) {
        if (b) i2c_sda_1(bus); else i2c_sda_0(bus);
        i2c_delay_bit();
        i2c_scl_1(bus); i2c_delay_bit();
        i2c_scl_0(bus);
        i2c_sda_1(bus); i2c_delay_bit();
    }
    U8 i2c_rd_bit(U8 bus) {
        i2c_sda_1(bus); //i2c_delay_bit();
        i2c_scl_1(bus); i2c_delay_bit();
        U8 bit = i2c_sda(bus);
        i2c_scl_0(bus); i2c_delay_bit();
        return(bit);
    }
    U8 i2c_wr_U8(U8 bus, U8 b) {
        for (U8 i=8; i; i--) {
            i2c_wr_bit(bus, b & 0x80);
            b <<= 1;
        }
        i2c_delay_bit();
        return(i2c_rd_bit(bus)); //return ACK
    }
    U8 i2c_rd_U8(U8 bus, U8 ack) {
        U8 result = 0;
        for (U8 i=8; i; i--) {
            result <<= 1;
            result |= !!(i2c_rd_bit(bus));
        }
        i2c_wr_bit(bus, ack);
        return(result);
    }
    void setup_i2c() {
    	for (U8 b=0; b<I2C_BUSES; b++) {
    		i2c_scl_1(b);
    		i2c_sda_1(b);
    	}
    }
#endif


//AVR hardware I2C
#ifdef I2C_AVR
    void i2c_wait() {
        while (i2c_busy());
    }
    void i2c_cmd(U8 cmd) {
        TWCR = cmd;
    }
    U8 i2c_cmd_wait(U8 cmd) {
        i2c_cmd(cmd);
        i2c_wait();
        return(TWSR);
    }
    U8 i2c_wr_U8(U8 bus, U8 b) {
        TWDR = b;
        i2c_cmd_wait(I2C_CMD_WR);
        return(TWSR & 8);
    }
    U8 i2c_rd_U8(U8 bus, U8 ack) {
        i2c_cmd_wait(ack ? I2C_CMD_RD_NACK : I2C_CMD_RD_ACK);
        return(TWDR);
    }
    void setup_i2c() {
    }
#endif

#ifdef I2C_STM32
    U8 addr_byte;
    void i2c_start(U8 bus) {
    	while (I2C_GetFlagStatus(I2C_STM32, I2C_FLAG_BUSY));
    	I2C_GenerateSTART(I2C_STM32, ENABLE);
    	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    	addr_byte = 1;
    }
    void i2c_stop(U8 bus) {
    	I2C_GenerateSTOP(I2C_STM32, ENABLE);
    	while((I2C_STM32)->CR1 & I2C_CR1_STOP);
    }
    U8 i2c_wr_U8(U8 bus, U8 b) {
    	if (addr_byte) {
        	addr_byte = 0;
        	I2C_Send7bitAddress(I2C_STM32, b, (b & 1) ? I2C_Direction_Receiver : I2C_Direction_Transmitter);
        	if (!(b & 1))
        		while(!I2C_CheckEvent(I2C_STM32, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
        	else
        		I2C_AcknowledgeConfig(I2C_STM32, ENABLE);
        	i7s_buf[5] = I7S_2;
    	} else {
			I2C_SendData(I2C_STM32, b);
			while(!I2C_CheckEvent(I2C_STM32, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    	}
    	U8 ack = (I2C_GetFlagStatus(I2C_STM32, I2C_FLAG_AF) == SET);
    	if (ack) I2C_ClearFlag(I2C_STM32, I2C_FLAG_AF);
    	return ack;
    }
    U8 i2c_rd_U8(U8 bus, U8 ack) {
    	I2C_AcknowledgeConfig(I2C_STM32, (ack) ? DISABLE : ENABLE);
    	while(!I2C_CheckEvent(I2C_STM32, I2C_EVENT_MASTER_BYTE_RECEIVED));
    	return I2C_ReceiveData(I2C_STM32);
    }
    void setup_i2c() {
    }
#endif

U8 i2c_chkack(U8 bus, U8 addr) {
	if (!i2c_sda(bus) || !i2c_scl(bus)) return 1;
	i2c_start(bus);
	U8 ack = !!i2c_wr_U8(bus, addr); //device address in write mode
	i2c_stop(bus);
	return(ack); //ACK received ? 0 : 1
}

U8 i2c_wr_a8_buf8(U8 bus, U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size) {
    i2c_start(bus);
    i2c_wr_U8(bus, addr_i2c);
    i2c_wr_U8(bus, addr_start);
    while (buf_size--)
        i2c_wr_U8(bus, *buf++);
    i2c_stop(bus);
    return(0);
}

U8 i2c_rd_a8_buf8(U8 bus, U8 addr_i2c, U8 addr_start, U8 *buf, U8 buf_size) {
    i2c_start(bus);
    i2c_wr_U8(bus, addr_i2c);
    i2c_wr_U8(bus, addr_start);
    if (buf_size) {
        i2c_start(bus);
        i2c_wr_U8(bus, addr_i2c | 1);
        while (buf_size--)
            *buf++ = i2c_rd_U8(bus, buf_size == 0);
    }
    i2c_stop(bus);
    return(0);
}


#ifdef I2C_PAGED
    
    //for paged EEPROM read/write define use_i2c_paged (если нужно, подключить функции для записи в устройства с страничной организацией)
    U8  i2c_paged_dev_addr;  /**< \brief I2C paged device address <br>адрес устройства \ingroup group_i2c */
    U16 i2c_paged_size;      /**< \brief I2C paged device size <br>размер \ingroup group_i2c */
    U8  i2c_paged_page_mask; /**< \brief I2C paged device page address mask <br>маска адреса страницы \ingroup group_i2c */
    U16 i2c_paged_byte_addr; /**< \brief I2C paged device byte address <br>текущий адрес записи \ingroup group_i2c */
    U8  i2c_paged_flags;     /**< \brief I2C paged device state <br>текущее состояние записи и другие флаги \ingroup group_i2c */

    #define I2C_PAGED_FLAG_STARTED 1 /**< \brief I2C paged device write started <br>=1 если запись страницы уже начата \ingroup group_i2c */
    #define I2C_PAGED_FLAG_16BIT   2 /**< \brief I2C paged device uses 16-bit address <br>=1 для использования 16-битной адресации \ingroup group_i2c */

    /** \brief start paged write to I2C device <br>начать страничную запись \ingroup group_i2c */
    void i2c_paged_wr_start(U8 dev_addr, U16 byte_addr) {
        i2c_paged_dev_addr  = dev_addr;  //remember device address (запомнить адрес устройства)
        i2c_paged_byte_addr = byte_addr; //remember starting byte (запомнить адрес первого байта)
        i2c_paged_flags &= ~I2C_PAGED_FLAG_STARTED; //clear started flag (обнулить флаг начала записи)
    }

    /** \brief end paged write to I2C device <br>завершить страничную запись \ingroup group_i2c */
    void i2c_paged_wr_stop(void) {
        //if data was written, stop writing and wait to the end of write cycle (если данные уже записывались, прекратить запись и ожидать завершения цикла записи)
        if (i2c_paged_flags & I2C_PAGED_FLAG_STARTED) {
            i2c_stop(); //I2C STOP
            //ACK polling (проверка завершения записи страницы)
            while (i2c_chkack(i2c_paged_dev_addr));
        }
        i2c_stop(); //I2C STOP
        i2c_paged_flags &= ~I2C_PAGED_FLAG_STARTED; //clear STARTED flag (обнулить флаг начатой записи)
    }

    /** \brief paged mode byte write <br>побайтовая запись в страничном режиме \ingroup group_i2c */
    void i2c_paged_wr_U8(U8 adata) {
        //if not overflow, write (если достигнут полный объем, не писать больше)
        if (i2c_paged_byte_addr < i2c_paged_size) {
                //if first data, setup address (если начало записи, дать команду начала записи и настроить адрес)
                if ((i2c_paged_flags & I2C_PAGED_FLAG_STARTED) == 0) {
                    //mark that writing was started (отметить что запись уже происходила)
                    i2c_paged_flags |= I2C_PAGED_FLAG_STARTED;
                    i2c_start();
                    //write device address, write mode (записать адрес устройства)
                    i2c_wr_U8(i2c_paged_dev_addr);
                    //if necessary, write high address byte (в 16-битном режиме адресации записать старший байт адреса)
                    if (i2c_paged_flags & I2C_PAGED_FLAG_16BIT) i2c_wr_U8(i2c_paged_byte_addr >> 8);
                    //write low address byte (записать младший байт адреса)
                    i2c_wr_U8(i2c_paged_byte_addr);
            }
            i2c_wr_U8(adata); //write data byte (записать байт данных)
            i2c_paged_byte_addr++; //increment byte address (увеличить счетчик адреса)
            //if end of current page, end write cycle (если достигнут конец страницы, завершить ее запись)
            if (((U8)i2c_paged_byte_addr & i2c_paged_page_mask) == 0) i2c_paged_wr_stop();
        }
    }

#endif
