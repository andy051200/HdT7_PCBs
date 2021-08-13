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
unsigned char conversion1;
unsigned char conversion2;
unsigned char conversion3;
unsigned char conversion4;
unsigned char cen_pot1, dec_pot1, un_pot1;
unsigned char cen_pot2, dec_pot2, un_pot2;
unsigned char cen_pot3, dec_pot3, un_pot3;
unsigned char cen_pot4, dec_pot4, un_pot4;
unsigned char cuenta_uart;
/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);
void toggle_adc(void);
unsigned char datos_ascii(uint8_t numero);
uint8_t uart_ascii();
void envio_uart(void);
//void mapeos(void);
/*-----------------------------------------------------------------------------
 --------------------------- INTERRUPCIONES -----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    if (TXIF)
    {
        cuenta_uart++;          //se suma a variable de control
        envio_uart();           //se llama funcion para mandar los datos
        PIR1bits.TXIF=0;                 //se apga bandera de interrupcion
    }
    
    
}
/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void) {
    setup();
    __delay_ms(1);
    ADCON0bits.GO=1;
    while(1)
    {
        toggle_adc();
        //TXREG=conversion1;
        //PORTD=conversion1;
    }
    return;
}
/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
void setup(void){
    //-------CONFIGURACION ENTRADAS ANALOGICAS
    ANSELbits.ANS0 = 1;                         //AN0 para potenciometro 1
    ANSELbits.ANS1 = 1;                         //AN0 para potenciometro 2
    ANSELbits.ANS2 = 1;                         //AN0 para potenciometro 3
    ANSELbits.ANS3 = 1;                         //AN0 para potenciometro 4
    ANSELH = 0;
    //-------CONFIGURACION IN/OUT
    TRISAbits.TRISA0 = 1;                       //entrada potenciometro 1
    TRISAbits.TRISA1 = 1;                       //entrada potenciometro 2
    TRISAbits.TRISA2 = 1;                       //entrada potenciometro 3
    TRISAbits.TRISA3 = 1;                       //entrada potenciometro 4
    TRISCbits.TRISC6=0;                         //salida TX uart
    TRISCbits.TRISC7=1;                         //entrada RX uart
    TRISD = 0;
    //-------LIMPIEZA DE PUERTOS
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    //-------CONFIGURACION DE RELOJ A 4MHz
    osc_config(4);
    //-------CONFIGURACION DE ADC
    ADC_config();
    //-------CONFIGURACION DE COMUNICACION UART
    uart_config();
    //-------CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;                   //se habilitan interrupciones globales
    PIE1bits.ADIE = 0 ;                 //se prende interrupcion por ADC
    PIR1bits.ADIF = 0;                  //se baja bandera de conversion
    PIR1bits.ADIF = 0;                  //se baja bandera de conversion
    PIE1bits.TXIE=1;                    //enable interrupcion de tx uart
    PIR1bits.TXIF=0;                    //apago bandera interrupcion tx uart
    
}
/*-----------------------------------------------------------------------------
 --------------------------------- FUNCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
//-------FUNCION PARA CAMBIO DE CANALES EN ADC
void toggle_adc(void)
{
    if (ADCON0bits.GO==0)
    {
        switch(ADCON0bits.CHS)
        {
            case(0):
                conversion1=ADRESH;         //potenciometro 1
                __delay_us(500);            //delay para cargar capacitor          
                ADCON0bits.CHS=1;           //switch de canal, cambio a 1
                ADCON0bits.GO=1;            //se inicia otra conversion ADC
                break;
                    
            case(1):
                conversion2=ADRESH;         //potenciometro 2
                __delay_us(500);            //delay para cargar capacitor
                ADCON0bits.CHS=2;           //switch de canal, cambio a 2
                ADCON0bits.GO=1;            //se inicia otra conversion ADC
                break;
            case(2):
                conversion3=ADRESH;         //potenciometro 2
                __delay_us(500);            //delay para cargar capacitor
                ADCON0bits.CHS=3;           //switch de canal, cambio a 3
                ADCON0bits.GO=1;            //se inicia otra conversion ADC
                break;
            case(3):
                conversion4=ADRESH;         //potenciometro 2
                __delay_us(500);            //delay para cargar capacitor
                ADCON0bits.CHS=0;           //switch de canal, cambio a 0
                ADCON0bits.GO=1;            //se inicia otra conversion ADC
                break;
            
            __delay_us(500);                //delay para carga de capacitor
           
        }
    }
}
//-------FUNCION PARA CONVERSOR A ASCII
unsigned char datos_ascii(uint16_t numero)    //funcion para convertir a valores ascii
{
    switch(numero)
    {
        default:
            return 0x30;        //retorna 0 en ascii
            break;
        case(0):
            return 0x30;        //retorna 0 en ascii
            break;
            
        case(1):
            return 0x31;        //retorna 1 en ascii
            break;
            
        case(2):
            return 0x32;        //retorna 2 en ascii
            break;
            
        case(3):
            return 0x33;        //retorna 3 en ascii
            break;
            
        case(4):
            return 0x34;        //retorna 4 en ascii
            break;
            
        case(5):
            return 0x35;        //retorna 5 en ascii
            break;
            
        case(6):
            return 0x36;        //retorna 6 en ascii
            break;
            
        case(7):
            return 0x37;        //retorna 7 en ascii
            break;
            
        case(8):
            return 0x38;        //retorna 8 en ascii
            break;
            
        case(9):
            return 0x39;        //retorna 9 en ascii
            break;       
    }   
}

//-------FUNCION PARA MANDAR DATOS EN UART
void envio_uart(void)
{
    switch(cuenta_uart)
    {
        case(1):            //GAUGE, DE 0 A 255, centenas
            TXREG = datos_ascii(((conversion1/100))%10);
            break;
            
        case(2):            //GAUGE, DE 0 A 255, decenas
            TXREG = datos_ascii(((conversion1/10))%10);
            break;
            
        case(3):            //GAUGE, DE 0 A 255, unidades
            TXREG = datos_ascii((conversion1)%10);
            break;
            
        case(4):            //ESPACIO EN BLANCO
            TXREG =32;
            break;
            
        case(5):            //TANQUE, DE 0 A 100, centenas
            TXREG = datos_ascii((((conversion2/2.55)/100)%10));
            break;
            
        case(6):            //TANQUE, DE 0 A 100, decenas
            TXREG = datos_ascii((((conversion2/2.55)/10)%10));
            break;
            
        case(7):            //TANQUE, DE 0 A 100, unidades
            TXREG =datos_ascii(((conversion2/2.55)%10));
            break;
                
        case(8):            //ESPACIO EN BLANCO
            TXREG =32;
            break;
            
        case(9):            //TERMOMETRO, DE 0 A 70, decenas
            TXREG =datos_ascii((((conversion3/3.641)/10)%10));
            break;
            
        case(10):            //TERMOMETRO, DE 0 A 70, unidades
            TXREG =datos_ascii(((conversion3/21.3)%10));
            break;
            
        case(11):            //ESPACIO EN BLANCO
            TXREG =32;
            break;
            
        case(12):            //AGUJA, DE 0 A 12, DECENAS
            TXREG =datos_ascii(((conversion4/21.3)/10)%10);
            break;
            
        case(13):            //AGUJA, DE 0 A 12, UNIDADES
            TXREG =datos_ascii((conversion4/21.3)%10);
            break;
            
        case(14):            //ESPACIO EN BLANCO
            TXREG =32;
            break;
            
        case(15):            //ENTER DE LINEA
            TXREG =13;
            cuenta_uart=0;
            break;
    } 
}