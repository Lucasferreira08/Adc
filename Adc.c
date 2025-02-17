#include "adc_libs.h"
 
 // =============================================================================
 // VARIÁVEIS GLOBAIS (algumas são voláteis pois são alteradas em ISR)
 // =============================================================================
 volatile bool pwm_enabled = true;       // flag que indica se os LEDs PWM estão ativos
 volatile bool led_green_on = false;       // estado atual do LED Verde
 volatile uint32_t last_js_btn_press = 0;  // timestamp do último acionamento do botão do joystick
 volatile uint32_t last_btn_a_press = 0;   // timestamp do último acionamento do botão A
 volatile int border_style = 0;            // índice do estilo da borda do display (alterna a cada pressionamento)

 ssd1306_t ssd;
 
 // =============================================================================
 // FUNÇÕES RELACIONADAS À INTERRUPÇÃO DOS BOTÕES
 // =============================================================================
 void gpio_callback(uint gpio, uint32_t events) {
     // Obtenção do tempo atual (ms)
     uint32_t current_time = to_ms_since_boot(get_absolute_time());
 
     if (gpio == PIN_JOYSTICK_BTN) {
         // Debounce: se o tempo decorrido for menor que o definido, ignora
         if (current_time - last_js_btn_press < DEBOUNCE_DELAY_MS) return;
         last_js_btn_press = current_time;
 
         // Alterna o estado do LED Verde
         led_green_on = !led_green_on;
         gpio_put(PIN_LED_GREEN, led_green_on);
 
         // Altera o estilo da borda do display (por exemplo, alterna entre 2 estilos)
         border_style = (border_style + 1) % 2;
     }
     else if (gpio == PIN_BTN_A) {
         if (current_time - last_btn_a_press < DEBOUNCE_DELAY_MS) return;
         last_btn_a_press = current_time;
 
         // Alterna a ativação dos LEDs PWM
         pwm_enabled = !pwm_enabled;
         if (!pwm_enabled) {
             // Se desativados, força o desligamento dos LEDs PWM
             pwm_set_gpio_level(PIN_LED_RED, 0);
             pwm_set_gpio_level(PIN_LED_BLUE, 0);
         }
     }
 }

 void pinos_config() 
 {
    // ---------------------------
     // Configuração dos pinos de LED
     // ---------------------------
     // LED Azul (PWM)
     gpio_init(PIN_LED_BLUE);
     gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);
     // LED Vermelho (PWM)
     gpio_init(PIN_LED_RED);
     gpio_set_dir(PIN_LED_RED, GPIO_OUT);
     // LED Verde (digital – ligado/desligado)
     gpio_init(PIN_LED_GREEN);
     gpio_set_dir(PIN_LED_GREEN, GPIO_OUT);
     gpio_put(PIN_LED_GREEN, led_green_on);   // estado inicial desligado

     // init button joystick
     gpio_init(PIN_JOYSTICK_BTN);
     gpio_set_dir(PIN_JOYSTICK_BTN, GPIO_IN);
     gpio_pull_up(PIN_JOYSTICK_BTN);

     // init button joystick
     gpio_init(PIN_BTN_A);
     gpio_set_dir(PIN_BTN_A, GPIO_IN);
     gpio_pull_up(PIN_BTN_A);

     // ---------------------------
     // Configuração dos botões com interrupção (IRQ)
     // ---------------------------
     // Registra a callback para todos os GPIOs
     gpio_set_irq_enabled_with_callback(PIN_JOYSTICK_BTN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
     gpio_set_irq_enabled_with_callback(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

     // ---------------------------
     // Configuração do ADC para o joystick
     // ---------------------------
     adc_init();
     adc_gpio_init(PIN_JOYSTICK_X);   // ADC canal 0 para eixo X
     adc_gpio_init(PIN_JOYSTICK_Y);   // ADC canal 1 para eixo Y
 }

 void pwm_main_config() 
 {
         // ---------------------------
     // Configuração dos LEDs PWM
     // ---------------------------
     // Configura LED Azul (GPIO 11)
     uint slice_blue = pwm_gpio_to_slice_num(PIN_LED_BLUE);
     uint channel_blue = pwm_gpio_to_channel(PIN_LED_BLUE);
     pwm_set_wrap(slice_blue, 4095);  // define valor de wrap (PERÍODO PWM)
     pwm_set_chan_level(slice_blue, channel_blue, 0);  // inicia com brilho 0
     pwm_set_enabled(slice_blue, true);
     gpio_set_function(PIN_LED_BLUE, GPIO_FUNC_PWM);
 
     // Configura LED Vermelho (GPIO 12)
     uint slice_red = pwm_gpio_to_slice_num(PIN_LED_RED);
     uint channel_red = pwm_gpio_to_channel(PIN_LED_RED);
     pwm_set_wrap(slice_red, 4095);
     pwm_set_chan_level(slice_red, channel_red, 0);
     pwm_set_enabled(slice_red, true);
     gpio_set_function(PIN_LED_RED, GPIO_FUNC_PWM);
 }

 void display_config() 
 {
    // ---------------------------
     // Configuração do I2C para o display SSD1306
     // ---------------------------
     i2c_init(I2C_PORT, 400 * 1000);  // I2C a 400 kHz
     gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(PIN_I2C_SDA);
     gpio_pull_up(PIN_I2C_SCL);
 
     // Inicializa o display SSD1306 (assumindo que a library do display esteja integrada)
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, SSD1306_ADDR, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
 }
 
 // =============================================================================
 // MAIN
 // =============================================================================
 int main() {
     // Inicializações básicas
     stdio_init_all();

     pinos_config();
     pwm_main_config();
     display_config();
 
     // =============================================================================
     // LOOP PRINCIPAL
     // =============================================================================
     while (1) {
         // --- Leitura dos valores do joystick (ADC) ---
         // Leitura do eixo X (botão controla o LED Vermelho)
        //  adc_select_input(0);
        //  uint16_t adc_x = adc_read();
 
        //  // Leitura do eixo Y (botão controla o LED Azul)
        //  adc_select_input(1);
        //  uint16_t adc_y = adc_read();

        adc_select_input(0);
         uint16_t adc_y = adc_read();
 
         adc_select_input(1);
         uint16_t adc_x = adc_read();
 
         // --- Cálculo dos níveis PWM para os LEDs ---
         // A ideia é que, quando o joystick estiver no centro (2048), o LED fique apagado.
         // Quanto maior o desvio (seja para mais ou para menos), maior a intensidade.
         uint16_t diff_x = (adc_x > ADC_CENTER) ? (adc_x - ADC_CENTER) : (ADC_CENTER - adc_x);
         uint16_t diff_y = (adc_y > ADC_CENTER) ? (adc_y - ADC_CENTER) : (ADC_CENTER - adc_y);
         
         // Mapeia a diferença para o intervalo de PWM (0 a 4095)
         uint32_t duty_red  = ((uint32_t) diff_x * 4095) / ADC_CENTER;
         uint32_t duty_blue = ((uint32_t) diff_y * 4095) / ADC_CENTER;
 
         if (pwm_enabled) {
             pwm_set_gpio_level(PIN_LED_RED, duty_red);
             pwm_set_gpio_level(PIN_LED_BLUE, duty_blue);
         }
         else {
             // Se os PWM estiverem desativados, força o valor 0
             pwm_set_gpio_level(PIN_LED_RED, 0);
             pwm_set_gpio_level(PIN_LED_BLUE, 0);
         }
 
         // --- Cálculo da posição do quadrado no display ---
    // Mapeia os valores ADC para a posição dentro da área útil do display
    uint8_t square_x = (adc_x * (SSD1306_WIDTH - SQUARE_SIZE)) / ADC_MAX;
    uint8_t square_y = ((ADC_MAX - adc_y) * (SSD1306_HEIGHT - SQUARE_SIZE)) / ADC_MAX;

    // --- Atualização do display SSD1306 ---
    // Limpa o buffer do display
    ssd1306_fill(&ssd, false);

    // Desenha a borda de acordo com o estilo atual (alternado pelo botão do joystick)
    if (border_style == 0) {
    // Estilo 0: borda completa (retângulo vazio)
        ssd1306_rect(&ssd, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, 1, 0);
    }
    else if (border_style == 1) {
        // Estilo 1: borda pontilhada (desenha pixels alternados)
        for (int x = 0; x < SSD1306_WIDTH; x += 2) {
            ssd1306_pixel(&ssd, x, 0, 1);
            ssd1306_pixel(&ssd, x, SSD1306_HEIGHT - 1, 1);
        }
        for (int y = 0; y < SSD1306_HEIGHT; y += 2) {
            ssd1306_pixel(&ssd, 0, y, 1);
            ssd1306_pixel(&ssd, SSD1306_WIDTH - 1, y, 1);
        }
}

// Desenha o quadrado móvel representando a posição do joystick
// Nota: A função ssd1306_rect espera (top, left, width, height),
// portanto, square_y é o 'top' e square_x é o 'left'
    ssd1306_rect(&ssd, square_y, square_x, SQUARE_SIZE, SQUARE_SIZE, 1, 1);

// Atualiza o display (envia o buffer via I2C)
    ssd1306_send_data(&ssd);

    // Pequeno atraso para não sobrecarregar a CPU (e ajustar taxa de atualização)
    sleep_ms(50);

        }
 
     return 0;
 }