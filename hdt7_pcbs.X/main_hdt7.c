/*------------------------------------------------------------------------------
Archivo: mainsproject.s
Microcontrolador: PIC16F887
Autor: Andy Bonilla
Compilador: pic-as (v2.30), MPLABX v5.45
    
Programa: laboratorio 1
Hardware: PIC16F887
    
Creado: 16 de julio de 2021    
Descripcion: 
------------------------------------------------------------------------------*/

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT   //configuracion de oscilador interno
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <stdint.h>
#include <pic16f887.h>
#include <xc.h>
#include "Osc_config.h"
#include "ADC_CONFIG.h"
#include "UART_CONFIG.h"

/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/

//-------DIRECTIVAS DEL COMPILADOR
#define _XTAL_FREQ 4000000
//-------VARIABLES DE PROGRAMA
unsigned char conversion1, conversion2, conversion3, conversion4; //guardar adresh
unsigned char cuenta_uart=0;                            //mandar datos uart
unsigned char map_cen_pot1, map_dec_pot1, map_un_pot1;  //mapeo pot1
unsigned char map_cen_pot2, map_dec_pot2, map_un_pot2;  //mapeo pot2
unsigned char map_cen_pot3, map_dec_pot3, map_un_pot3;  //mapeo pot3
unsigned char map_cen_pot4, map_dec_pot4, map_un_pot4;  //mapeo pot4
unsigned char uart_cen_pot1,uart_dec_pot1, uart_un_pot1;    //ascii pot1
unsigned char uart_cen_pot2,uart_dec_pot2, uart_un_pot2;    //ascii pot2
unsigned char uart_cen_pot3,uart_dec_pot3, uart_un_pot3;    //ascii pot3
unsigned char uart_cen_pot4,uart_dec_pot4, uart_un_pot4;    //ascii pot4
/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);
void toggle_adc(void);
void mandar_datos(void);
void mapeos(void);
/*-----------------------------------------------------------------------------
 --------------------------- INTERRUPCIONES -----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    if (PIR1bits.TXIF)
    {
        cuenta_uart++;      //se suma variable guia
        mandar_datos();     //invoco funcion para mandar uart
        PIR1bits.TXIF=0;    //apago interrupcion
    }   
}
/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void)
{
    setup();
    __delay_ms(1);
    ADCON0bits.GO=1;
    while(1)
    {
        //-------CAMBIO DE CANALES EN ADC
        toggle_adc();
        //-------MAPEO DE VALORES DE POTENCIOMETROS
        mapeos();
    }
   
}
/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
void setup(void)
{
    //-------CONFIGURACION ENTRADAS ANALOGICAS
    ANSEL=0;
    ANSELH=0;
    ANSELbits.ANS0 = 1;                 //AN0 para potenciometro 1
    ANSELbits.ANS1 = 1;                 //AN1 para potenciometro 2
    ANSELbits.ANS2 = 1;                 //AN2 para potenciometro 3
    ANSELbits.ANS3 = 1;                 //AN3 para potenciometro 4
    //-------CONFIGURACION IN/OUT
    TRISAbits.TRISA0=1;                 //entrada pot1
    TRISAbits.TRISA1=1;                 //entrada pot2
    TRISAbits.TRISA2=1;                 //entrada pot3
    TRISAbits.TRISA3=1;                 //entrada pot4
    TRISCbits.TRISC6=0;                 //salida TX uart
    TRISCbits.TRISC7=1;                 //entrada RX uart
    //-------LIMPIEZA DE PUERTOS
    PORTA=0;
    PORTC=0;
    //-------CONFIGURACION DE RELOJ A 4MHz
    osc_config(4);
    //-------CONFIGURACION DE ADC
    ADC_config();
    //-------CONFIGURACION DE COMUNICACION UART
    uart_config();
    //-------CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;           //se habilita interrupciones globales
    INTCONbits.PEIE=1;          //interrupcion por perifericos
    PIE1bits.TXIE=1;            //enable interrupcion de tx uart
    PIE1bits.RCIE=1;            //enable interrupcion de rx uart
    PIR1bits.TXIF=0;            //apago bandera interrupcion tx uart
    PIR1bits.RCIF=0;            //apago bandera interrupcion rx uart
}
/*-----------------------------------------------------------------------------
 --------------------------------- FUNCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
//-------FUNCION PARA CAMBIO DE CANALES EN ADC
void toggle_adc()
{
    if(ADCON0bits.GO==0)
    {
        switch(ADCON0bits.CHS)
        {
            case(0):
                conversion1=ADRESH;
                __delay_us(500);
                ADCON0bits.CHS=1;
                break;
                
            case(1):
                conversion2=ADRESH;
                __delay_us(500);
                ADCON0bits.CHS=2;
                break;
                
            case(2):
                conversion3=ADRESH;
                __delay_us(500);
                ADCON0bits.CHS=3;
                break;
            case(3):
                conversion4=ADRESH;
                __delay_us(500);
                ADCON0bits.CHS=0;
                break;
                
        }
        __delay_ms(1);
        ADCON0bits.GO=1;
    }
    return;
}
//-------FUNCION PARA MANDAR DATOS UART 
void mandar_datos(void)
{
    switch(cuenta_uart)
    {
        //default:
          //  TXREG=32;               //mando espacio
            //break;
        case(1):
            TXREG=uart_cen_pot1;    //mando centenas pot1
            break;
        case(2):
            TXREG=uart_dec_pot1;    //mando decenas pot1
            break;
        case(3):
            TXREG=uart_un_pot1;     //mando unidades pot1
            break;
        case(4):
            TXREG=0x2C;             //mando coma de separacion
            break;
        case(5):
            TXREG=uart_cen_pot2;    //mando centenas pot2
            break;
        case(6):
            TXREG=uart_dec_pot2;    //mando decenas pot2
            break;
        case(7):
            TXREG=uart_un_pot2;     //mando unidades pot2
            break;
        case(8):
            TXREG=0x2C;             //mando coma de separacion
            break;
        case(9):
            TXREG=uart_cen_pot3;    //mando centenas pot3
            break;
        case(10):
            TXREG=uart_dec_pot3;    //mando decenas pot3
            break;
        case(11):
            TXREG=uart_un_pot3;     //mando unidades pot3
            break;
        case(12):
            TXREG=0x2C;             //mando coma de separacion
            break;
        case(13):
            TXREG=uart_cen_pot4;    //mando centenas pot4
            break;
        case(14):
            TXREG=uart_dec_pot4;    //mando decenas pot4
            break;
        case(15):
            TXREG=uart_un_pot4;     //mando unidades pot4
            break;
        case(16):
            TXREG=10;               //mando nueva linea
            break;
        case(17):
            TXREG=13;               //mando retorno de carro
            break;
        case(30):
            cuenta_uart=0;          //un tipo de delay para reiniciar cuenta
            break;
    }
    return;
}

void mapeos(void)
{
    //-------MAPEO DE VALORES DE POTENCIOMETROS
    //pot1
    map_cen_pot1=(((conversion1)/100)%10);    //centenas de pot1
    map_dec_pot1=(((conversion1)/10)%10);    //centenas de pot1
    map_un_pot1=((conversion1)%10);    //centenas de pot1
    //pot2
    map_cen_pot2=(((conversion2)/100)%10);    //centenas de pot1
    map_dec_pot2=(((conversion2)/10)%10);    //centenas de pot1
    map_un_pot2=((conversion2)%10);    //centenas de pot1
    //pot3
    map_cen_pot3=(((conversion3)/100)%10);    //centenas de pot1
    map_dec_pot3=(((conversion3)/10)%10);    //centenas de pot1
    map_un_pot3=((conversion3)%10);    //centenas de pot1
    //pot4
    map_cen_pot4=(((conversion4)/100)%10);    //centenas de pot1
    map_dec_pot4=(((conversion4)/10)%10);    //centenas de pot1
    map_un_pot4=((conversion4)%10);    //centenas de pot1
    
    //-------TRASLADO A ASCII
    //pot1
    uart_cen_pot1=(map_cen_pot1+0x30);
    uart_dec_pot1=(map_dec_pot1+0x30);
    uart_un_pot1=(map_un_pot1+0x30);
    //pot2
    uart_cen_pot2=(map_cen_pot2+0x30);
    uart_dec_pot2=(map_dec_pot2+0x30);
    uart_un_pot2=(map_un_pot2+0x30);
    //pot3
    uart_cen_pot3=(map_cen_pot3+0x30);
    uart_dec_pot3=(map_dec_pot3+0x30);
    uart_un_pot3=(map_un_pot3+0x30);
    //pot4
    uart_cen_pot4=(map_cen_pot4+0x30);
    uart_dec_pot4=(map_dec_pot4+0x30);
    uart_un_pot4=(map_un_pot4+0x30);
    return;
}