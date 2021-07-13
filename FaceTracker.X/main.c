
#include "mcc_generated_files/mcc.h"

#include <xc.h>
#include "PCA9685.h"

#define DATA_MIN 0
#define DATA_MAX 90

void send(unsigned char data);
void recieve_ang(uint8_t data, uint8_t *ch, uint8_t *ang);

uint8_t flg;

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    ANSELA = 0x0;
    ANSELB = 0x0;

    //LED    
    TRISA = 0x0;
    LATA = 0x0;

    //SDA(RC4),SCL(RC3) set pull up(i2c)
    
    //i2c setting
    SSPADD = 0x13;
    SSPCON1 = 0x28;
    SSPCON2 = 0x0;
    SSPSTAT = 0;
    
     //set PCA9685 setting
    __delay_ms(500);
    init();
    
    set_pwm_freq(50);
    LATA = 0x01;
    
    //USART setting
    //TX on
    TXSTA = 0x02;
    //RC on
    RCSTA = 0x90;
    //BAUDCON 16bit
    BAUDCON = 0x08;
    //BRGH = 0, BRG16 = 1 9600bit mode
    SPBRG = 51;
    
    //interrupt setting
    //interrupt flg clear
    PIR1bits.RCIF = 0;
    
    //USART recive interrupt enable
    PIE1bits.RCIE = 1;
    PEIE = 1;
    GIE = 1;

    flg = 1;
    
    __delay_ms(1000);
    LATA = 0x0;
    while (1){}
}


/**
 * send data
 * @param data
 */
void send(unsigned char data){
    while(!TXSTAbits.TRMT);
    TXREG = data;
}

/**
 * 8th bit 0: servo 0channel, 1:servo 1channel
 * 7~0th angle range(0~90) 
 * @param data USART recived data
 * @param ch servo channel
 * @param ang servo angle
 */
void recieve_ang(uint8_t data, uint8_t *ch, uint8_t *ang){
    const uint8_t mask = 0x80;
    //detect servo channel    
    (*ch) = (mask & data) ? 1 : 0;
    
    data = data & ~mask;
    (*ang) = data * 2;
}

/**
 * interrupt func
 */
void __interrupt() isr(void){
    if(PIR1bits.RCIF){
        //flg clear
        PIR1bits.RCIF = 0;
        //error
        if((RCSTAbits.OERR) || (RCSTAbits.FERR)){
            RCSTA = 0;
            RCSTA = 0x90;
        }else{
        //not error
            if(flg){
                LATA = 0x01;
                flg = 0;
            }else{
                LATA = 0x0;
                flg = 1;
            }
            
            uint8_t data = RCREG;
            send(data);
            
            uint8_t ch;
            uint8_t ang;
            recieve_ang(data, &ch, &ang);
            servo_write(ch, ang);                          
        }
    }
}