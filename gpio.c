#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "./ws2812.pio.h"
#include "buzzer.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 22

// Pinos do teclado
#define ROW1 5
#define ROW2 6
#define ROW3 7
#define ROW4 8
#define COL1 28
#define COL2 2
#define COL3 3
#define COL4 4

// Estrutura para cor RGB
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_Color;

// Estado global do PIO
PIO pio;
uint sm;

// Protótipos das funções
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw);
void init_ws2812(void);
void put_pixel(uint32_t pixel_grb);
void set_all_pixels(RGB_Color color);
char read_keypad(void);
void animation_1(void);
void animation_2();
void animation_3(); // Animação de círculos concêntricos

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

// Adicione esta função antes da função main
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw)
{
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    float div = clock_get_hz(clk_sys) / (freq * 2.5f);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

int main()
{
    stdio_init_all();
    init_ws2812();
    init_buzzer();

    // Configuração dos pinos do teclado
    gpio_init(ROW1);
    gpio_init(ROW2);
    gpio_init(ROW3);
    gpio_init(ROW4);
    gpio_init(COL1);
    gpio_init(COL2);
    gpio_init(COL3);
    gpio_init(COL4);

    gpio_set_dir(ROW1, GPIO_OUT);
    gpio_set_dir(ROW2, GPIO_OUT);
    gpio_set_dir(ROW3, GPIO_OUT);
    gpio_set_dir(ROW4, GPIO_OUT);
    gpio_set_dir(COL1, GPIO_IN);
    gpio_set_dir(COL2, GPIO_IN);
    gpio_set_dir(COL3, GPIO_IN);
    gpio_set_dir(COL4, GPIO_IN);

    gpio_pull_down(COL1);
    gpio_pull_down(COL2);
    gpio_pull_down(COL3);
    gpio_pull_down(COL4);

    while (1)
    {
        char key = read_keypad();
        if (key != 0)
        {
            printf("Tecla: %c\n", key);

            switch (key)
            {
            case '1':
                animation_1();
                play_tone(440, 100);
                set_all_pixels((RGB_Color){0, 0, 0}); // Desliga LEDs após a animação
                break;
            case '2': // Nova animação no botão 2
                animation_2();
                play_tone(523, 100);
                set_all_pixels((RGB_Color){0, 0, 0}); // Desliga LEDs após a animação
                break;
            case '3': // Animação com o botão 3 cria efeito de círculos concêntricos
                animation_3();
                play_tone(500, 100);
                set_all_pixels((RGB_Color){0, 0, 0}); // Desliga LEDs após a animação
                break;
            case 'B':
                set_all_pixels((RGB_Color){0, 0, 255}); // Azul 100%
                break;

            case 'C':
                set_all_pixels((RGB_Color){204, 0, 0}); // Vermelho 80%
                break;
            case 'D':
                set_all_pixels((RGB_Color){0, 128, 0}); // Verde 50%
                break;
            case '#':
                set_all_pixels((RGB_Color){255, 255, 255}); // Branco 100%
                break;
            case '*':
                watchdog_enable(1, 1);
                while (1)
                    ;
                break;
            }
        }
        sleep_ms(100);
    }
}

void init_ws2812()
{
    pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
}

void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void set_all_pixels(RGB_Color color)
{
    uint32_t rgb = ((uint32_t)(color.r) << 16) | ((uint32_t)(color.g) << 8) | (uint32_t)(color.b);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        put_pixel(rgb);
    }
}

char read_keypad()
{
    const char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};

    for (int row = 0; row < 4; row++)
    {
        gpio_put(ROW1, row == 0);
        gpio_put(ROW2, row == 1);
        gpio_put(ROW3, row == 2);
        gpio_put(ROW4, row == 3);

        sleep_us(100);

        if (gpio_get(COL1))
            return keys[row][0];
        if (gpio_get(COL2))
            return keys[row][1];
        if (gpio_get(COL3))
            return keys[row][2];
        if (gpio_get(COL4))
            return keys[row][3];
    }
    return 0;
}

void animation_1()
{
    // Definição do FPS e tempo entre frames
    const int FPS = 30;
    const int frame_delay = 1000 / FPS; // Delay em ms para atingir o FPS desejado

    // Cores base para a animação
    RGB_Color cores[] = {
        {255, 0, 0},   // Vermelho
        {255, 127, 0}, // Laranja
        {255, 255, 0}, // Amarelo
        {0, 255, 0},   // Verde
        {0, 0, 255},   // Azul
        {75, 0, 130},  // Índigo
        {148, 0, 211}  // Violeta
    };
    const int num_cores = 7;

    // Mapeamento da matriz 5x5 para facilitar a animação
    const uint8_t matriz[5][5] = {
        {0, 1, 2, 3, 4},
        {5, 6, 7, 8, 9},
        {10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19},
        {20, 21, 22, 23, 24}};

    // Executa a animação por 3 ciclos completos
    for (int ciclo = 0; ciclo < 3; ciclo++)
    {
        // Cria efeito de onda diagonal
        for (int offset = -5; offset < 10; offset++)
        {
            for (int y = 0; y < 5; y++)
            {
                for (int x = 0; x < 5; x++)
                {
                    int pos = matriz[y][x];
                    int wave_pos = (x + y - offset) % num_cores;
                    if (wave_pos < 0)
                        wave_pos += num_cores;

                    // Calcula a intensidade baseada na posição
                    float intensidade = 1.0f;
                    if (x + y - offset < 0 || x + y - offset > 9)
                    {
                        intensidade = 0.0f;
                    }

                    // Aplica a cor com a intensidade calculada
                    RGB_Color cor = cores[wave_pos];
                    put_pixel(urgb_u32(
                        (uint8_t)(cor.r * intensidade),
                        (uint8_t)(cor.g * intensidade),
                        (uint8_t)(cor.b * intensidade)));
                }
            }
            sleep_ms(frame_delay);
        }
    }

    // Fade out final
    for (int i = 100; i >= 0; i -= 5)
    {
        float fade = i / 100.0f;
        for (int led = 0; led < NUM_PIXELS; led++)
        {
            RGB_Color cor = cores[led % num_cores];
            put_pixel(urgb_u32(
                (uint8_t)(cor.r * fade),
                (uint8_t)(cor.g * fade),
                (uint8_t)(cor.b * fade)));
        }
        sleep_ms(frame_delay);
    }
}

void animation_2()
{
    // Definição do FPS e tempo entre frames
    const int FPS = 20;
    const int frame_delay = 1000 / FPS; // Delay em ms para atingir o FPS desejado

    // Cores base para a animação
    RGB_Color cores[] = {
        {255, 0, 0},   // Vermelho
        {0, 255, 0},   // Verde
        {0, 0, 255},   // Azul
        {255, 255, 0}, // Amarelo
        {0, 255, 255}, // Ciano
    };
    const int num_cores = 5;

    // Mapeamento da matriz 5x5 para facilitar a animação
    const uint8_t matriz[5][5] = {
        {0, 1, 2, 3, 4},
        {5, 6, 7, 8, 9},
        {10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19},
        {20, 21, 22, 23, 24}};

    // Vetores para controle da espiral
    int dx[] = {0, 1, 0, -1}; // Direções (direita, baixo, esquerda, cima)
    int dy[] = {1, 0, -1, 0};
    int x = 0, y = 0, dir = 0;

    // Cria o efeito de espiral
    for (int ciclo = 0; ciclo < 3; ciclo++)
    {
        int passos = 0;
        int limite = 1;
        int incremento = 0;

        for (int passo = 0; passo < 25; passo++)
        {
            int cor_idx = passo % num_cores;
            RGB_Color cor = cores[cor_idx];

            // Define o pixel atual com a cor
            int pos = matriz[x][y];
            put_pixel(urgb_u32(cor.r, cor.g, cor.b));

            // Atualiza a direção da espiral
            x += dx[dir];
            y += dy[dir];
            passos++;

            if (passos == limite)
            {
                passos = 0;
                dir = (dir + 1) % 4;
                incremento++;
                if (incremento % 2 == 0)
                {
                    limite++;
                }
            }

            sleep_ms(frame_delay);
        }

        // Reseta os LEDs antes do próximo ciclo
        set_all_pixels((RGB_Color){0, 0, 0});
    }
}

void animation_3()
{
    // Definição do FPS e tempo entre frames
    const int FPS = 20;
    const int frame_delay = 1000 / FPS; // Delay em ms para atingir o FPS desejado

    // Cores base para a animação
    RGB_Color cores[] = {
        {255, 0, 0},   // Vermelho
        {0, 255, 0},   // Verde
        {0, 0, 255},   // Azul
        {255, 255, 0}, // Amarelo
        {0, 255, 255}, // Ciano
    };
    const int num_cores = 5;

    // Mapeamento da matriz 5x5 para facilitar a animação
    const uint8_t matriz[5][5] = {
        {0, 1, 2, 3, 4},
        {5, 6, 7, 8, 9},
        {10, 11, 12, 13, 14},
        {15, 16, 17, 18, 19},
        {20, 21, 22, 23, 24}};

    // Cria o efeito de círculos concêntricos
    for (int ciclo = 0; ciclo < 3; ciclo++)
    {
        for (int raio = 0; raio <= 2; raio++)
        {
            int cor_idx = raio % num_cores;
            RGB_Color cor = cores[cor_idx];

            // Limpa todos os LEDs
            set_all_pixels((RGB_Color){0, 0, 0});

            // Desenha um círculo de raio específico
            for (int i = -raio; i <= raio; i++)
            {
                for (int j = -raio; j <= raio; j++)
                {
                    int x = 2 + i;
                    int y = 2 + j;
                    if (x >= 0 && x < 5 && y >= 0 && y < 5)
                    {
                        int dist = abs(i) + abs(j);
                        if (dist == raio)
                        {
                            int pos = matriz[x][y];
                            put_pixel(urgb_u32(cor.r, cor.g, cor.b));
                        }
                    }
                }
            }

            sleep_ms(frame_delay);
        }
    }
}