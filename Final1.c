#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"

uint32_t i, cuenta1, distancia;

 #define GPIO_PORTJ_DIR_R (*((volatile uint32_t *)0x40060400)) //Registro deDirección PJ
 #define GPIO_PORTJ_DEN_R (*((volatile uint32_t *)0x4006051C)) //Registro dehabilitación PJ
 #define GPIO_PORTJ_PUR_R (*((volatile uint32_t *)0x40060510)) //Registro depull-up PJ
 #define GPIO_PORTJ_DATA_R (*((volatile uint32_t *)0x40060004)) //Registro deDatos J
 #define GPIO_PORTJ_IS_R (*((volatile uint32_t *)0x40060404)) //Registro deconfiguración de detección de nivel o flanco
 #define GPIO_PORTJ_IBE_R (*((volatile uint32_t *)0x40060408)) //Registro deconfiguración de interrupción por ambos flancos
 #define GPIO_PORTJ_IEV_R (*((volatile uint32_t *)0x4006040C)) //Registro deconfiguración de interrupción por un flanco
 #define GPIO_PORTJ_ICR_R (*((volatile uint32_t *)0x4006041C)) //Registro delimpieza de interrupcion de flanco en PJ
 #define GPIO_PORTJ_IM_R (*((volatile uint32_t *)0x40060410)) //Registro demascara de interrupcion PJ

int cuenta=0;


int main(void) {
    SYSCTL_RCGCGPIO_R = 0x308; // Habilita relojes de los puertos N y F
    SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3
    SYSCTL_RCGCGPIO_R |= 0x00000108; // (a) activa el reloj para el puerto

    i = SYSCTL_RCGCGPIO_R;


    // Configuracion del puerto K para sensor ultrasonico
    GPIO_PORTK_DIR_R |= 0B000010000; // Pines 0-3 como salidas
    GPIO_PORTK_DIR_R &= ~0B00100000;
    GPIO_PORTK_DEN_R |= 0x30; // Habilitar los pines 0-3 como salidas digitales
    GPIO_PORTK_IM_R |= 0x20;
    GPIO_PORTK_IS_R &= ~0X20; //DETECTA FLANCO
    GPIO_PORTK_IBE_R &= ~0X20;
    GPIO_PORTK_IEV_R |= 0X20; // FLANCO DE SUBIDA
    GPIO_PORTK_ICR_R = 0X20; // LIMPIA INTERRUPCIONES


    GPIO_PORTJ_DIR_R &= ~0x01; // (c) PJ0 dirección entrada - boton SW1
    GPIO_PORTJ_DEN_R |= 0x01; // PJ0 se habilita
    GPIO_PORTJ_PUR_R |= 0x01; // habilita weak pull-up on PJ1
    GPIO_PORTJ_IS_R &= ~0x01; // (d) PJ1 es sensible por flanco
    GPIO_PORTJ_IBE_R &= ~0x01; // PJ0 no es sensible a dos flancos
    GPIO_PORTJ_IEV_R &= ~0x01; // PJ0 detecta eventos de flanco de bajada
    GPIO_PORTJ_ICR_R = 0x01; // limpia la bandera 0
    GPIO_PORTJ_IM_R |= 0x01; // Se desenmascara la interrupcion PJ0 y se envia al controlador de interrupciones
    NVIC_PRI12_R = (NVIC_PRI12_R&0x00FFFFFF)|0x00000000; // (g) prioridad 0
    NVIC_EN1_R = 0x00080000 | (1 << (52 - 32));//(h) habilita la interrupción 51 en NVIC (Pag. 154)

    //Configuracion del puerto D para los motores
    GPIO_PORTD_AHB_DEN_R |= 0x10; //BIT 4 DIGITAL
    GPIO_PORTD_AHB_DIR_R |= 0x10; //bit 4 SALIDA
    GPIO_PORTD_AHB_DATA_R = 0x00; // SALIDA A 0
    GPIO_PORTD_AHB_AFSEL_R = 0x10; //FUNCION ALTERNA EN BIT 4
    GPIO_PORTD_AHB_PCTL_R = 0x00030000; //DIRIGIDO A T3CCP0

    //Configuracion timer 3
    TIMER3_CTL_R=0X00000000; //DESHABILITA TIMER EN LA CONFIGURACION
    TIMER3_CFG_R= 0X00000004; //CONFIGURAR PARA 16 BITS
    TIMER3_TAMR_R= 0X0000000A; //CONFIGURAR PARA MODO PWM, MODO PERIODICO CUENTA HACIA ABAJO
    TIMER3_TAPR_R= 0X00; // RELOJ 16 MHZ
    TIMER3_CTL_R |= 0X00000001; //HABILITA TIMER A



    // Trigger en bajo
    GPIO_PORTK_DATA_R &= ~0B00010000;

    while (1) {
        SysCtlDelay(266666);
        // Si no hay rebote se queda en el while
        // Trigger
        GPIO_PORTK_DATA_R |= 0B00010000;
        SysCtlDelay(100);
        GPIO_PORTK_DATA_R &= ~0B00010000;
        //MOTORES

              /*  if (distancia >= 110) {
                    TIMER3_TAMATCHR_R = 0xDAC0; // 10%
                } else if (distancia > 30) {
                    TIMER3_TAMATCHR_R = 0x7530; // 50%
                } else if (distancia > 20) {
                    TIMER3_TAMATCHR_R = 0x4350; // 74%
                } else if (distancia > 10) {
                    TIMER3_TAMATCHR_R = 0x05dc; // 80%
                }*/

               if (distancia >= 100) {
                            TIMER3_TAMATCHR_R = 0x04b0; // 100% (ciclo de trabajo reducido)
                           } else if (distancia > 80) {
                            TIMER3_TAMATCHR_R = 0x1b58; // 88%
                           } else if (distancia > 60) {
                            TIMER3_TAMATCHR_R = 0x2ee0; // 81%
                           } else if (distancia > 40) {
                               TIMER3_TAMATCHR_R = 0x55f0; // 50%
                           } else if (distancia > 30) {
                               TIMER3_TAMATCHR_R = 0x9470; // 40%
                           } else if (distancia > 20) {
                               TIMER3_TAMATCHR_R = 0xbf68; // 20%
                           } else if (distancia > 10) {
                               TIMER3_TAMATCHR_R = 0xe678; // 10%
                           }

    }
}

void INT_ECHO(void) {
    cuenta1 = 0;
    while ((GPIO_PORTK_DATA_R & 0B00100000) == 0x20 && cuenta1 < 0xFFFF) {
        cuenta1++;
        distancia = cuenta1 / 24;
    }
    GPIO_PORTK_ICR_R = 0X20; // LIMPIA INTERRUPCIONES
}


void GPIOPortJ_Handler(void){
    cuenta = cuenta +1;
    GPIO_PORTJ_ICR_R = 0x01;
}
