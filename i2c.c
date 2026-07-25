//i2c_peripheral.c
#include <LPC21xx.h>
#include "types.h"
#include "pin_func_defines.h"
#include "pin_connect_block.h"
#include "i2c_defines.h"
void init_i2c(void) 
{ 
   //Cfg p0.2 for SCL functions 
   cfgportpinfunc(0,2,SCL_PIN_0_2); 
   //Cfg p0.3 for SDA functions  
   cfgportpinfunc(0,3,SDA_PIN_0_3); 
   //Cfg Speed for I2C Serial Communication 
   I2SCLL=I2C_DIVIDER; 
   I2SCLH=I2C_DIVIDER; 
   //I2C Peripheral Enable for Communication 
   I2CONSET=1<<I2EN_BIT;  
}
void i2c_start(void) 
{ 
  // start condition 
  I2CONSET=1<<STA_BIT; 
  //wait for start bit status 
  while(((I2CONSET>>SI_BIT)&1)==0); 
  // clear start condition 
  I2CONCLR=1<<STA_BIT; 
}	 
void i2c_restart(void) 
{ 
   // start condition 
   I2CONSET=1<<STA_BIT; 
   //clr prev SI_BIT 
   I2CONCLR=1<<SIC_BIT; 
    //wait for SI bit status 
   while(((I2CONSET>>SI_BIT)&1)==0); 
   // clear start condition 
   I2CONCLR=1<<STA_BIT; 
}	 
void i2c_write(u8 dat) 
{ 
   //put data into I2DAT 
    I2DAT=dat;	 
    //clr SI_BIT using I2CON 
    I2CONCLR=1<<SIC_BIT; 
    //wait for SI bit status 
    while(((I2CONSET>>SI_BIT)&1)==0); 
}	 
void i2c_stop(void) 
{ 
    // issue stop condition 
	I2CONSET=1<<STO_BIT; 
    // clr SI bit status	 
    I2CONCLR=1<<SIC_BIT; 
    //stop will be cleared automatically 
} 

// Function to receive one byte from I2C and send NACK
u8 i2c_nack(void) 
{	 
     // Clear the SI (Serial Interrupt) flag to resume I2C operation
     I2CONCLR = 1 << SIC_BIT;

     // Wait until the next byte is received (SI flag becomes set)
     while(((I2CONSET >> SI_BIT) & 1) == 0);

     // Return the received data without acknowledging it (NACK)
     // NACK indicates this is the last byte to be received.
     return I2DAT;
}

// Function to receive one byte from I2C and send ACK
u8 i2c_mack(void) 
{	 
    // Assert ACK so the slave will continue sending the next byte
    I2CONSET = 1 << AA_BIT;

    // Clear the SI flag to continue the I2C transaction
    I2CONCLR = 1 << SIC_BIT;

    // Wait until the next byte is received (SI flag becomes set)
    while(((I2CONSET >> SI_BIT) & 1) == 0);

    // Clear the AA bit so ACK is not sent unintentionally later
    I2CONCLR = 1 << AAC_BIT;

    // Return the received data byte
    return I2DAT;
}
