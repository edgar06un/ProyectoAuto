#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"
#include <stdlib.h>


uint32_t i, cuenta1, distancia,NumeroInt,i;
int resultado,voltaje,start;
int cuenta=0;




int main(void) {


    SYSCTL_RCGCGPIO_R = 0x1308; // Habilita relojes de los puertos  D
    SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3
    SYSCTL_RCGCGPIO_R |= 0x00000108; // (a) activa el reloj para el puerto J
    SYSCTL_RCGCTIMER_R |= 0X01; //Conecta el reloj al bloque de timer 0


    i = SYSCTL_RCGCGPIO_R;

    NumeroInt = 0x1234;

    // Configuracion del puerto K para sensor ultrasonico
    GPIO_PORTK_DIR_R |= 0B000010000; // Pines 0-3 como salidas
    GPIO_PORTK_DIR_R &= ~0B00100000;
    GPIO_PORTK_DEN_R |= 0x30; // Habilitar los pines 0-3 como salidas digitales
    GPIO_PORTK_IM_R |= 0x20;
    GPIO_PORTK_IS_R &= ~0X20; //DETECTA FLANCO
    GPIO_PORTK_IBE_R &= ~0X20;
    GPIO_PORTK_IEV_R |= 0X20; // FLANCO DE SUBIDA
    GPIO_PORTK_ICR_R = 0X20; // LIMPIA INTERRUPCIONES



    NVIC_PRI12_R = (NVIC_PRI12_R&0x00FFFFFF)|0x00000000; // (g) prioridad 0
    NVIC_EN1_R = 0x00080000 | (1 << (52 - 32));//(h) habilita la interrupción 51 en NVIC (Pag. 154)

    NVIC_EN0_R |= 0x100000; //Habilita interrupcion 20 timer 0 sub B

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

    //Configuracion del puerto N y timer 0
    GPIO_PORTN_DIR_R = 0x01; //Puerto N salida
    GPIO_PORTN_DEN_R = 0x1; //Habilita puerto N (PN0)


    TIMER0_CTL_R = 0x0; //Apaga timer 0 subtimer B
    TIMER0_CFG_R = 0x04; //Timer trabaja a 16 bits
    TIMER0_TBMR_R = 0x012; //Modo periodico y cuenta hacia arriba
    TIMER0_TBILR_R = 0xf424; // Valor para 1 segundo
    TIMER0_TBPR_R = 0x0ff; //Prescalador a 256
    TIMER0_IMR_R = 0x100; //Habilita interrupciones en subtimer B
    TIMER0_ICR_R = 0x100; //Para limpiar interrupcion subtimer B (Agregando un 1 a un registro revisar datasheet)
    TIMER0_CTL_R = 0x0100; //Habilita timer0 y subtimer B y empieza a contar.


    NumeroInt = 0x0;


    // Trigger en bajo
    GPIO_PORTK_DATA_R &= ~0B00010000;


    // Para escribir en el archivo de texto
      //  FILE* archivo = fopen("archivo.txt", "w");
        //   if (archivo == NULL) {
          //     printf("No se pudo abrir el archivo.\n");
            //   return 1;
           //}

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

             GPIO_PORTN_DATA_R ^= 0x01; //Cuenta timer


           /*  if (NumeroInt < 50) {
                fprintf(archivo, "%d,", NumeroInt);
                         fflush(archivo);  // Para asegurar que los datos se escriban en el archivo inmediatamente
             }*/

    }


   // fclose(archivo);

}


//Rutina para sensor de distancia
void INT_ECHO(void) {
    cuenta1 = 0;
    while ((GPIO_PORTK_DATA_R & 0B00100000) == 0x20 && cuenta1 < 0xFFFF) {
        cuenta1++;
        distancia = cuenta1 / 24;
    }
    GPIO_PORTK_ICR_R = 0X20; // LIMPIA INTERRUPCIONES
}


//Rutina para timer contador de segundos
void Timer0SubB(void){
    TIMER0_ICR_R = 0x100; //Limpia interrupciones de subtimer B
    NumeroInt = NumeroInt +1;

}


