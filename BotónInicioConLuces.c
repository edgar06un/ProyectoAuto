#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"

int resultado,voltaje,start;

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

int main(void){


        // activa PLL

        SysCtlClockFreqSet ((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
        SYSCTL_PLLFREQ1_R = 0x00000004;
        SYSCTL_PLLFREQ0_R = 0x00800060;

        while ((SYSCTL_PLLSTAT_R & 0X01) == 0); // ESPERA SE ESTABILICE El PLL


//configuracion
SYSCTL_RCGCADC_R = 0X1; // habilita reloj en ADC0
SYSCTL_RCGCGPIO_R = 0X8C; // HABILITA RELOJ DE PUERTO D y A
SYSCTL_RCGCGPIO_R |= 0X0100; // HABILITA RELOJ DE PUERTO D y A
resultado = 0123;

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

        NVIC_EN1_R |= 0x80000; // Habilitar interrupción del puerto J

    //CONFIGURACION DEL PUERTO C
    GPIO_PORTC_AHB_DIR_R|= 0xC0;
    //GPIO_PORTC_AHB_PUR_R = 0XC0;
    GPIO_PORTC_AHB_DEN_R |= 0XD0;
    GPIO_PORTC_AHB_DATA_R |= 0x00;
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
while(1){
while(start==1){
    //incio de conversión
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
return 0;
}
