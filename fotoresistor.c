#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"

int i = 0;
int j;
int cuenta=0;
int secuencia[4] = {0x9, 0xc, 0x6, 0x3};
int velocidad = 1000000; // Velocidad predeterminada del LED

void IntPJ1(void){
    cuenta=cuenta+1;
    velocidad=velocidad/2;
    GPIO_PORTJ_AHB_ICR_R = 0X2; //LIMPIA INTERRUPCIONES
}

int main(void)
{
    // Configuracion de los puertos
    SYSCTL_RCGCGPIO_R |= 0x0B00; // Habilitar los relojes de los puertos J y K

    // Configuracion del puerto J para el boton de velocidad
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

    // Configuracion del puerto M para el boton de direccion
        GPIO_PORTM_DIR_R = 0x0; // Pin 0 como entrada
        GPIO_PORTM_PUR_R |= 0x01; // Habilitar resistencia pull-up en el pin 0
        GPIO_PORTM_DEN_R |= 0x01; // Habilitar el pin 0 como entrada digital


    // Configuracion del puerto K para los LEDs
    GPIO_PORTK_DIR_R |= 0x0F; // Pines 0-3 como salidas
    GPIO_PORTK_DEN_R |= 0x0F; // Habilitar los pines 0-3 como salidas digitales

    while (cuenta <=5)
    {
        if (!(GPIO_PORTJ_AHB_DATA_R & 0x02)) // Si el botón de velocidad está presionado
                {
                    velocidad = velocidad / 2; // Cambiar la velocidad de los LEDs
                }
      /*  if (GPIO_PORTM_DATA_R) // Si el boton de direccion esta presionado
        {
            i = (i + 1) % 4; // Cambiar al siguiente LED en la secuencia
        }
        else
        {
            i = (i + 3) % 4; // Cambiar al LED anterior en la secuencia
        }*/

        //GPIO_PORTK_DATA_R = secuencia[i]; // Cambiar la salida del puerto K al LED correspondiente

        // Esperar antes de cambiar al siguiente LED
        for (j = 0; j < velocidad; j++)
        {
            // Si el boton de velocidad esta presionado, salir del bucle de espera antes
            if (GPIO_PORTJ_AHB_DATA_R & 0x02 >= (velocidad / 2))
            {
                break;
            }
        }
    }


    return 0;
}
