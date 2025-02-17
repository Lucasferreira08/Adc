#ifndef ADC_LIBS_H
#define ADC_LIBS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "display.h"

// =============================================================================
// DEFINIÇÕES DE CONSTANTES
// =============================================================================
#define DEBOUNCE_DELAY_MS    200   // atraso para debouncing (ms)

// Mapeamento dos pinos
#define PIN_LED_BLUE         12    // LED Azul (PWM) – controlado pelo eixo Y do joystick
#define PIN_LED_RED          13    // LED Vermelho (PWM) – controlado pelo eixo X do joystick
#define PIN_LED_GREEN        11    // LED Verde (digital) – acionado pelo botão do joystick
#define PIN_JOYSTICK_BTN     22    // Botão do joystick
#define PIN_BTN_A            5     // Botão A
#define PIN_JOYSTICK_X       26    // Joystick eixo X (ADC0)
#define PIN_JOYSTICK_Y       27    // Joystick eixo Y (ADC1)

// Pinos do I2C para o display SSD1306
#define PIN_I2C_SDA          14
#define PIN_I2C_SCL          15

// Definições ADC
#define ADC_CENTER           2048      // valor central (joystick solto)
#define ADC_MAX              4095      // resolução 12 bits (0-4095)

// Parâmetros do display SSD1306
#define SSD1306_WIDTH         128
#define SSD1306_HEIGHT         64
#define SQUARE_SIZE             8      // tamanho do quadrado que representa o joystick

#define I2C_PORT i2c1       // Controlador I²C0 do RP2040
#define SSD1306_ADDR 0x3C   // Endereço I²C do display (alternativo: 0x3C)

#define ADC_DEADZONE 200           // Dead zone around joystick center (adjust as needed)

#endif