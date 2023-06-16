#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"

uint32_t l, cuenta1, distancia ,resultado,voltaje,start,NumeroInt;

//GPS y Bluetooth

// Definición de variables para el manejo de datos
char data[100]; // Arreglo donde se guardan los bytes recibidos por UART
char location[25];
char speed[6];
char hour[7];

//variables de los ejes
float vx,vy,vz,x,y,z;
int result1;
int result2;
int result3;
int RawMin = 0;
int RawMax = 4096;
int i,j;
float temp2;

//Definicion de funciones
void Registers_init(void);
void UART_init(void);
void ADC_init(void);
char UART3Rx(void);
void UART3Tx(char c);
void DelayMs(int n);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void HC05_init(void); // Initialize UART5 module for HC-05
char Bluetooth_Read(void); //Read data from Rx5 pin of TM4C1294NCPDT
void Bluetooth_Write(unsigned char data); // Transmit a character to HC-05 over Tx5 pin
void Bluetooth_Write_String(char *str); // Transmit a string to HC-05 over Tx5 pin

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



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
    SYSCTL_RCGCGPIO_R = 0x1308; // Habilita relojes de los puertos N, K, J, D y C
    SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3

    SysCtlClockFreqSet ((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
               SYSCTL_PLLFREQ1_R = 0x00000004;
               SYSCTL_PLLFREQ0_R = 0x00800060;

               while ((SYSCTL_PLLSTAT_R & 0X01) == 0); // ESPERA SE ESTABILICE El PLL


               start=0;

       //configuracion
       SYSCTL_RCGCADC_R = 0X1; // habilita reloj en ADC0
       SYSCTL_RCGCGPIO_R |= 0x130D;
       resultado = 0123;

       //GPS
           Registers_init();
           //+++++++++++++++++++

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
    GPIO_PORTD_AHB_DATA_R |= 0x00; // SALIDA A 0
    GPIO_PORTD_AHB_AFSEL_R |= 0x10; //FUNCION ALTERNA EN BIT 4
    GPIO_PORTD_AHB_PCTL_R |= 0x00030000; //DIRIGIDO A T3CCP0

    //Configuracion timer 3
    TIMER3_CTL_R=0X00000000; //DESHABILITA TIMER EN LA CONFIGURACION
    TIMER3_CFG_R= 0X00000004; //CONFIGURAR PARA 16 BITS
    TIMER3_TAMR_R= 0X0000000A; //CONFIGURAR PARA MODO PWM, MODO PERIODICO CUENTA HACIA ABAJO
    TIMER3_TAPR_R= 0X00; // RELOJ 16 MHZ
    TIMER3_CTL_R |= 0X00000001; //HABILITA TIMER A

    //configuración de puerto D
        GPIO_PORTD_AHB_AFSEL_R |= 0x1; //FUNCION ALTERNA EN PDO
        GPIO_PORTD_AHB_DEN_R |= 0X0; // SE APAGA FUNCION DIGITAL EN PDO
        GPIO_PORTD_AHB_AMSEL_R |= 0X01; //HABILITA ENTRADA ANALÓGICCA
        //CONFIGURACION DEL CONVERTIDOR ADCO SECUANCIADOR 3
        ADC0_SSPRI_R = 0X00123; //SECUENCIADOR 3 ES EL DE MAYOR PRIORIDAD
        ADC0_ACTSS_R = 0X0; // APAGAA SECUANCIADOR3 PARA CONFIGURARLO
        ADC0_EMUX_R= 0X0; //DISPARO POR SOFTWARE
        ADC0_SSMUX3_R= 0X0F;
        ADC0_SSEMUX3_R =0X0; // ENTRADA A SECUENCIADOR 3 POR EL CANAL 15 (PDO)
        ADC0_SSCTL3_R= 0X0e0; // ULTIMO CANAL DE LA CONVERSION, ENTRADA POR PDO, SIN INTERRUPCIONES
        ADC0_ACTSS_R= 0X08; // HABILITA SECUENCIADOR 3

        ////////// APAGA PLL
        SYSCTL_PLLFREQ1_R = 0x0;
        SYSCTL_PLLFREQ0_R = 0x0;
        /////
        start=0;
        SYSCTL_RCGCTIMER_R |= 0X08; //HABILITA TIMER 3

        l = SYSCTL_RCGCGPIO_R;



    // Trigger en bajo
    GPIO_PORTK_DATA_R &= ~0B00010000;



    while (1) {
        if(start==1){
            SysCtlDelay(266666);
            // Si no hay rebote se queda en el while

            // Trigger
            GPIO_PORTK_DATA_R |= 0B00010000;
            SysCtlDelay(100);
            GPIO_PORTK_DATA_R &= ~0B00010000;
        //MOTORES

              /*  if ( >= 110) {
                    TIMER3_TAMATCHR_R = 0xDAC0; // 10%
                } else if ( > 30) {
                    TIMER3_TAMATCHR_R = 0x7530; // 50%
                } else if ( > 20) {
                    TIMER3_TAMATCHR_R = 0x4350; // 74%
                } else if ( > 10) {
                    TIMER3_TAMATCHR_R = 0x05dc; // 80%
                }*/



               if ( distancia>= 200) {
                            TIMER3_TAMATCHR_R = 0x04b0; // 100% (ciclo de trabajo reducido)
                           } else if (distancia > 150) {
                            TIMER3_TAMATCHR_R = 0x2ee0; // 88%
                           } else if (distancia > 110) {
                            TIMER3_TAMATCHR_R = 0x2ee0; // 81%
                           } else if (distancia > 80) {
                               TIMER3_TAMATCHR_R = 0x55f0; // 50%
                           } else if (distancia > 60) {
                               TIMER3_TAMATCHR_R = 0x9470; // 40%
                           } else if (distancia > 40) {
                               TIMER3_TAMATCHR_R = 0xe678; // 20%
                           } else if (distancia > 10) {
                               TIMER3_TAMATCHR_R = 0xe678; // 10%
                           }
               GPIO_PORTN_DATA_R ^= 0x01;

               ADC0_PSSI_R |= 0x08; //inicia conversión en secuanciador 3
                             while ((ADC0_ACTSS_R & 0x10000)== 0x10000);
                             resultado= ADC0_SSFIFO3_R;
                             voltaje=(330*resultado)/4095;
                             temp2=107.5-((75*(3.3)*ADC0_SSFIFO3_R)/4096);
                             if (temp2 > 100){
                                 TIMER3_TAMATCHR_R = 0xe678; // ciclo de trabajo a 20 %
                             }

                             if(voltaje<160){
                                 GPIO_PORTC_AHB_DATA_R |= 0xC0;
                             }
                             else{
                                 GPIO_PORTC_AHB_DATA_R = 0x00;
                             }




               //GPS++++++++++++++++++++
              for (i = 0; i < sizeof(data)-1 ; i++) {
                  data[i] = UART3Rx(); // Se guarda el valor en el arreglo 'data'
               }

               for (i = 0, j = 20; i < sizeof(location)-1 ; i++, j++) {
                   location[i] = data[j] ; // Se guarda el valor en el arreglo 'data';
               }
               for (i = 0, j = 47; i < sizeof(speed)-1 ; i++, j++) {
                   speed[i] = data[j] ; // Se guarda el valor en el arreglo 'data';
               }
               for (i = 0, j = 8; i < sizeof(hour)-1 ; i++, j ++) {
                   hour[i] = data[j] ; // Se guarda el valor en el arreglo 'data';
               }
               DelayMs(1000);

               ADC1_PSSI_R = 0x0001;             // Inicia conversión del SS0
               while ((ADC1_RIS_R & 0x01)==0);
               // Espera a que SS0 termine conversión de las muestra (polling)
               result1 = (ADC1_SSFIFO0_R & 0xFFF);// Resultado en FIFO0 se asigna a variable "result1"
               result2 = (ADC1_SSFIFO0_R & 0xFFF); // Resultado en FIFO0 se asigna a variable "result2"
               result3 = (ADC1_SSFIFO0_R & 0xFFF);

               ADC1_ISC_R = 0x0001;              // Limpia la bandera RIS del ADC1

               vx = (result1*3.3)/4096;        //Voltaje  visto por PK0
               vy = (result2*3.3)/4096;        //Voltaje  visto por PK1;
               vz = (result3*3.3)/4096;        //Voltaje  visto por PK2 
               DelayMs(20000);

                x = mapfloat(result1, RawMin, RawMax, -300, 300);
                DelayMs(20000);

                y = mapfloat(result2, RawMin, RawMax, -300, 300);
                DelayMs(20000);

                z = mapfloat(result3, RawMin, RawMax, -300, 300);
                DelayMs(20000);


                Bluetooth_Write_String("\nlocation: ");
                Bluetooth_Write_String(location);
                Bluetooth_Write_String("\nvelocidad: ");
                Bluetooth_Write_String(speed);
                Bluetooth_Write_String("\nhour: ");
                Bluetooth_Write_String(hour);
                //Bluetooth_Write_String(vx);
                //Bluetooth_Write_String(vy);
                //Bluetooth_Write_String(vz));

                //FinGPS

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

//GPS+++++++++++++++++++++++++++++++++++++++++++++++++

/* UART3 Receive */
char UART3Rx(void) {
    char c;
    while((UART3_FR_R & 0x10) != 0); /* wait until the buffer is not empty */
    c = UART3_DR_R;                  /* read the received data */
    return c;                       /* and return it */
}

/* UART3 Transmit */
void UART3Tx(char c) {
    while((UART3_FR_R & 0x20) != 0); /* wait until Tx buffer not full */
    UART3_DR_R = c;                  /* before giving it another byte */
}

/* Append delay functions  here */
void DelayMs(counter) {
    unsigned long i = 0;
    for(i=0; i< counter; i++);
}

void UART_init(void){
//This program sets up UART3 on TI ARM LaunchPad (TM4C1294)
// UART3 Tx is on PA5, Rx is on PA4.
    SYSCTL_RCGCUART_R |= 0x08;  //provide clock to UART3
    //while((SYSCTL_PRUART_R&SYSCTL_PRUART_R0) == 0); // Se espera a que el reloj se estabilice (p.505)
    DelayMs(500);
    //UART3 initialization
    UART3_CTL_R = 0;         // disable UART3
    UART3_IBRD_R = 104;      // 16MHz/16=1MHz, 1MHz/104=9600 baud rate
    UART3_FBRD_R = 11;       // fraction part,
    UART3_CC_R = 0;          // use system clock
    UART3_LCRH_R = 0x60;     // 8-bit, no parity, 1-stop bit
    UART3_CTL_R = 0x301;     // enable UART3, TXE, RXE
    // UART3 Tx is on PA5, Rx is on PA4.  Set them up.
    GPIO_PORTA_AHB_DEN_R |= 0x30;      // make PA5, PA4 as digital
    GPIO_PORTA_AHB_AMSEL_R = 0;       // turn off analog function
    GPIO_PORTA_AHB_AFSEL_R |= 0x30;    // use PA5, PA4 alternate function
    GPIO_PORTA_AHB_PCTL_R |= 0x00110000;     // configure PA5, PA4 for UART3
    DelayMs(500);             // wait for output line to stabilize
}

void ADC_init(void){
    GPIO_PORTK_DIR_R = 0x00;    // 2) PK0,1,2 entrada (analógica)
    GPIO_PORTK_AFSEL_R |= 0x07; // 3) Habilita Función Alterna de PK0,1,2
    GPIO_PORTK_DEN_R = 0x00;    // 4) Deshabilita Función Digital de PK0,1,2
    GPIO_PORTK_AMSEL_R |= 0x07; // 5) Habilita Función Analógica de PK0,1,2
    SYSCTL_RCGCADC_R  |= 0x02;   // 6) Habilita reloj para lógica de ADC1
    while((SYSCTL_PRADC_R&0x02)==0);
    ADC1_PC_R = 0x01;       // 7) Configura para 125Ksamp/s
    ADC1_SSPRI_R = 0x1320;  // 8) SS0 con la más alta prioridad
    ADC1_ACTSS_R = 0x0000;  // 9) Deshabilita SS0 antes de cambiar configuración de registros
    ADC1_EMUX_R = 0x0000;   // 10) Se configura SS0 para iniciar muestreo por software
    ADC1_SSEMUX0_R = 0x111;  // 11)Entradas AIN(15:0) para SS0 AIN(19:16)
    ADC1_SSMUX0_R = (ADC1_SSMUX0_R & 0xFFFFF000) + 0x210; //secuencia1 de muestreo AIN8 (PE3);
                             //Segunda secuencia de muestreo AIN0 (PK0)del AIN(19:16)
    ADC1_SSCTL0_R = 0x0600;
    // 0: AIN0 del AIN(19:16),sin bandera de interrupción, NO ES fin de secuencia, no modo diferencial 0100
    // 0: AIN8, sin bandera de interrupción, NO ES fin de secuencia,
    // 6: TEMP, con bandera de iterrupcion, fin de secuencia, no modo diferencia 0110
    ADC1_IM_R = 0x0000;     // 13) Deshabilita interrupciones de SS0
    ADC1_ACTSS_R |= 0x0001; // 14) Habilita SS0
    //ADC1_SSTSH0_R = 0X0040;
    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;    // encender PLL
    while((SYSCTL_PLLSTAT_R&0x01)==0);              // espera a que el PLL fije su frecuencia
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR;   // apagar PLL
    ADC1_ISC_R = 0x0001;                // Se recomienda Limpia la bandera RIS del ADC1
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

char Bluetooth_Read(void) {
    char data;
      while((UART4_FR_R & (1<<4)) != 0) /* wait until Rx buffer is not full */
    data = UART4_DR_R ;      /* before giving it another byte */
    return (unsigned char) data;
}

void Bluetooth_Write(unsigned char data) {
    while((UART4_FR_R & (1<<5))!= 0); /* wait until Tx buffer not full */
    UART4_DR_R = data;                  /* before giving it another byte */
}

void Bluetooth_Write_String(char *str) {
  while(*str)    {
        Bluetooth_Write(*(str++));
    }
}

void HC05_init(void)
{
    SYSCTL_RCGCUART_R |= 0x10;  /* enable clock to UART4 */
    SYSCTL_RCGCGPIO_R |= 0x0001;  /* enable clock to PORTK for PA2/Rx and PA3/Tx */
    DelayMs(500);
    /* UART4 initialization */
    UART4_CTL_R = 0;         /* UART4 module disable */
    UART4_IBRD_R = 104;      /* for 9600 baud rate, integer = 104 */
    UART4_FBRD_R = 11;       /* for 9600 baud rate, fractional = 11*/
    UART4_CC_R = 0;          /*select system clock*/
    UART4_LCRH_R = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
    UART4_CTL_R = 0x301;     /* Enable UART4 module, Rx and Tx */
    /* UART4 TX4 and RX4 use PK1 and PK0. Configure them digital and enable alternate function */
    GPIO_PORTA_AHB_DEN_R   |= 0x0C;      /* set PA2 and PA3 as digital */
    GPIO_PORTA_AHB_AFSEL_R |= 0x0C;    /* Use PK0,PK1 alternate function */
    GPIO_PORTA_AHB_AMSEL_R  = 0;    /* Turn off analg function*/
    GPIO_PORTA_AHB_PCTL_R  |= 0x00001100;     /* configure PK0 and PK1 for UART */
}

void Registers_init(void){

    SYSCTL_RCGCGPIO_R |= 0x201;  /* Enable clock to PORTA and K */
    while((SYSCTL_PRGPIO_R & 0x210)==0);

    UART_init();
    ADC_init();
    HC05_init();
}


//FINGPS
