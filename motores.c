//Cambiia ciclo de trabajo con dos motores en PD6 

#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"


#define SYSCTL_RCGCGPIO_R (*((volatile uint32_t *)0x400FE608))
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

void GPIOPortJ_Handler(void){
    cuenta = cuenta +1;
    GPIO_PORTJ_ICR_R = 0x01;
}



main(void) {
 SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R3; //HABILITA PORTD
 SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3
 SYSCTL_RCGCGPIO_R |= 0x00000108; // (a) activa el reloj para el puerto

 while ((SYSCTL_PRGPIO_R & 0X0108) == 0){}; // reloj listo?
     GPIO_PORTJ_DIR_R &= ~0x01; // (c) PJ0 dirección entrada - boton SW1
     GPIO_PORTJ_DEN_R |= 0x01; // PJ0 se habilita
     GPIO_PORTJ_PUR_R |= 0x01; // habilita weak pull-up on PJ1
     GPIO_PORTJ_IS_R &= ~0x01; // (d) PJ1 es sensible por flanco
     GPIO_PORTJ_IBE_R &= ~0x01; // PJ0 no es sensible a dos flancos
     GPIO_PORTJ_IEV_R &= ~0x01; // PJ0 detecta eventos de flanco de bajada
     GPIO_PORTJ_ICR_R = 0x01; // limpia la bandera 0
     GPIO_PORTJ_IM_R |= 0x01; // Se desenmascara la interrupcion PJ0 y se envia al controlador de interrupciones
     NVIC_PRI12_R = (NVIC_PRI12_R&0x00FFFFFF)|0x00000000; // (g) prioridad 0
     NVIC_EN1_R=0x00080000; //(h) habilita la interrupción 51 en NVIC (Pag. 154)

     GPIO_PORTD_AHB_DEN_R |= 0x10; //BIT 4 DIGITAL
     GPIO_PORTD_AHB_DIR_R |= 0x10; //bit 4 SALIDA
     GPIO_PORTD_AHB_DATA_R = 0x00; // SALIDA A 0
     GPIO_PORTD_AHB_AFSEL_R = 0x10; //FUNCION ALTERNA EN BIT 4
     GPIO_PORTD_AHB_PCTL_R = 0x00030000; //DIRIGIDO A T3CCP0
     TIMER3_CTL_R=0X00000000; //DESHABILITA TIMER EN LA CONFIGURACION
     TIMER3_CFG_R= 0X00000004; //CONFIGURAR PARA 16 BITS
     TIMER3_TAMR_R= 0X0000000A; //CONFIGURAR PARA MODO PWM, MODO PERIODICO CUENTA HACIA ABAJO


     TIMER3_TAPR_R= 0X00; // RELOJ 16 MHZ
     TIMER3_CTL_R |= 0X00000001; //HABILITA TIMER A

     while (1) {
         if (cuenta >= 4) {
             cuenta = 0;
         }

         if (cuenta == 0) {
             TIMER3_TAMATCHR_R = 0xDAC0; // 10%
         } else if (cuenta == 1) {
             TIMER3_TAMATCHR_R = 0x7530; // 50%
         } else if (cuenta == 2) {
             TIMER3_TAMATCHR_R = 0x4350; // 74%
         } else if (cuenta == 3) {
             TIMER3_TAMATCHR_R = 0x05dc; // 80%
         }


     }

}
