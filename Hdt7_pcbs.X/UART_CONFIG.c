/* 
 * File:   uart_config.h
 * Author: Andy Bonilla
 *
 * Created on 17 de julio de 2021, 11:32 PM
 */

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <xc.h>
#include <pic16f887.h>
#include <pic16f887.h>                 //se incluye libreria del compilador
#include "UART_CONFIG.h"
#define _XTAL_FREQ 4000000

/*-----------------------------------------------------------------------------
-------------------------- FUNCION DE LIBRERIA --------------------------------
-----------------------------------------------------------------------------*/
//--------configuracion de bits para comunicacion uart
void uart_config(void)
{
    //CONFIGURACION DE UART
    //braudeaje
    TXSTAbits.TX9 = 0; //TRANSMISION DE 8 BITS
    TXSTAbits.SYNC = 0; //ASINCRONO
    TXSTAbits.BRGH = 1; //HIGH SPEED
    BAUDCTLbits.BRG16 = 0; //BAUD RATE DE 8 BITS
    SPBRGH = 0;
    SPBRG = 103;
    PIE1bits.TXIE = 1;
    TXSTAbits.TXEN = 1;
    
    //CONFIG RX
    RCSTAbits.SPEN = 1;
    RCSTAbits. RX9 = 0;
    RCSTAbits.CREN = 1;
}
