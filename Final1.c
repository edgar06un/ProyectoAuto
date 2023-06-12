#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"


uint32_t i, cuenta1, distancia,resultado,voltaje,start,NumeroInt;


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


    SysCtlClockFreqSet ((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
               SYSCTL_PLLFREQ1_R = 0x00000004;
               SYSCTL_PLLFREQ0_R = 0x00800060;

               while ((SYSCTL_PLLSTAT_R & 0X01) == 0); // ESPERA SE ESTABILICE El PLL


       //configuracion
       SYSCTL_RCGCADC_R = 0X1; // habilita reloj en ADC0
       SYSCTL_RCGCGPIO_R = 0x130D;
       resultado = 0123;



    // Configuracion del puerto K para sensor ultrasonico
    GPIO_PORTK_DIR_R |= 0B000010000; // Pines 0-3 como salidas
    GPIO_PORTK_DIR_R &= ~0B00100000;
    GPIO_PORTK_DEN_R |= 0x30; // Habilitar los pines 0-3 como salidas digitales
    GPIO_PORTK_IM_R |= 0x20;
    GPIO_PORTK_IS_R &= ~0X20; //DETECTA FLANCO
    GPIO_PORTK_IBE_R &= ~0X20;
    GPIO_PORTK_IEV_R |= 0X20; // FLANCO DE SUBIDA
    GPIO_PORTK_ICR_R = 0X20; // LIMPIA INTERRUPCIONES


    // Configuracion del puerto J para el boton de INICIO
            GPIO_PORTJ_AHB_DIR_R = 0x0; // Pin 1 como entrada
                GPIO_PORTJ_AHB_PUR_R |= 0x02; // Habilitar resistencia pull-up en el pin 1
                GPIO_PORTJ_AHB_IM_R = 0x0;
                GPIO_PORTJ_AHB_IS_R = 0X0; //DETECTA FLANCO
                GPIO_PORTJ_AHB_IBE_R = 0X0; // DETECTA 1 FLANCO
                GPIO_PORTJ_AHB_IEV_R = 0X2; // FLANCO DE SUBIDA
                GPIO_PORTJ_AHB_ICR_R = 0X2; //LIMPIA INTERRUPCIONES
                GPIO_PORTJ_AHB_IM_R = 0X02; // HABILITA INTERRUPCIONES

                GPIO_PORTJ_AHB_DEN_R |= 0x02; // Habilitar el pin 1 como entrada digital


            //CONFIGURACION DEL PUERTO C
            GPIO_PORTC_AHB_DIR_R = 0xC0;
            //GPIO_PORTC_AHB_PUR_R = 0XC0;
            GPIO_PORTC_AHB_DEN_R = 0XD0;
            GPIO_PORTC_AHB_DATA_R = 0x00;
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

    //configuración de puerto D
        GPIO_PORTD_AHB_AFSEL_R = 0x1; //FUNCION ALTERNA EN PDO
        GPIO_PORTD_AHB_DEN_R = 0X0; // SE APAGA FUNCION DIGITAL EN PDO
        GPIO_PORTD_AHB_AMSEL_R = 0X01; //HABILITA ENTRADA ANALÓGICCA
        //CONFIGURACION DEL CONVERTIDOR ADCO SECUANCIADOR 3
        ADC0_SSPRI_R = 0X00123; //SECUENCIADOR 3 ES EL DE MAYOR PRIORIDAD
        ADC0_ACTSS_R = 0X0; // APAGAA SECUANCIADOR3 PARA CONFIGURARLO
        ADC0_EMUX_R= 0X0; //DISPARO POR SOFTWARE
        ADC0_SSMUX3_R= 0X0F;
        ADC0_SSEMUX3_R =0X0; // ENTRADA A SECUENCIADOR 3 POR EL CANAL 15 (PDO)
        ADC0_SSCTL3_R= 0X02; // ULTIMO CANAL DE LA CONVERSION, ENTRADA POR PDO, SIN INTERRUPCIONES
        ADC0_ACTSS_R= 0X08; // HABILITA SECUENCIADOR 3

        ////////// APAGA PLL
        SYSCTL_PLLFREQ1_R = 0x0;
        SYSCTL_PLLFREQ0_R = 0x0;
        /////
        start=0;
        SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3

        i = SYSCTL_RCGCGPIO_R;



    // Trigger en bajo
    GPIO_PORTK_DATA_R &= ~0B00010000;

    while (1) {
        while(start==1){
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


                              ADC0_PSSI_R = 0x08; //inicia conversión en secuanciador 3
                                  while ((ADC0_ACTSS_R & 0x10000)== 0x10000);
                                  resultado= ADC0_SSFIFO3_R;
                                  voltaje=(330*resultado)/4095;

                                  if(voltaje<160){
                                      GPIO_PORTC_AHB_DATA_R |= 0xC0;
                                  }
                                  else{
                                      GPIO_PORTC_AHB_DATA_R = 0x00;
                                  }

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


void IntPJ1(void) {
    // Pin 4 caused the interrupt
    if(GPIO_PORTJ_AHB_DATA_R == 0x2){
        if(start==1){
            start=0;
        }
        else{
            start=1;
        }
    }
  // Clear the interrupt flag
  GPIO_PORTJ_AHB_ICR_R = 0x02;
}


void Timer0SubB(void){
    TIMER0_ICR_R =0x100;
    NumeroInt = NumeroInt+1;
}
