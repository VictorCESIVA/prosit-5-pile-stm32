#include "stm32l4xx_hal.h"

#include <stdint.h>

enum { NB_CAPTEURS = 3, NB_VAL = 10 };

typedef struct {
    int16_t valeurs[NB_VAL];
    uint8_t prochain;
    uint16_t erreurs;
} Capteur;

/* Add_Val : écrit dans la fenêtre puis revient au début après la 10e case. */
static void ajouter_valeur(Capteur *capteur, int16_t mesure)
{
    capteur->valeurs[capteur->prochain] = mesure;
    if (++capteur->prochain == NB_VAL) {
        capteur->prochain = 0;
    }
}

/* Lecture : remplace provisoirement les vrais capteurs par des valeurs fixes. */
static void lecture(Capteur capteurs[NB_CAPTEURS], unsigned cycle)
{
    for (unsigned i = 0; i < NB_CAPTEURS; ++i) {
        int16_t mesure = (int16_t)(200u + 10u * i + cycle % 10u);
        if (i == 2u && cycle % 3u == 0u) {
            if (capteurs[i].erreurs < UINT16_MAX) {
                ++capteurs[i].erreurs;
            }
        } else {
            ajouter_valeur(&capteurs[i], mesure);
        }
    }
}

int main(void)
{
    /* Cet état doit survivre aux passages dans la boucle. */
    static Capteur capteurs[NB_CAPTEURS];
    GPIO_InitTypeDef led = {0};
    unsigned cycle = 0;

    HAL_Init();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    led.Pin = GPIO_PIN_5;
    led.Mode = GPIO_MODE_OUTPUT_PP;
    led.Pull = GPIO_NOPULL;
    led.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &led);

    while (1) {
        lecture(capteurs, cycle);
        /* Point d'arrêt : inspecter capteurs et le registre MSP. */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(1000);
        ++cycle;
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
