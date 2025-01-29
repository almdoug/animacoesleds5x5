## LINK DO VÍDEO: https://youtu.be/0B2Qc3kGA0U

# Projeto: Matriz de LEDs 5x5 com Raspberry Pi Pico W e Teclado Matricial 4x4

## Descrição
Este projeto implementa uma matriz de LEDs 5x5 controlada por um microcontrolador Raspberry Pi Pico W, utilizando um teclado matricial 4x4 como interface de entrada.

## Funcionalidades
- 4 animações diferentes acionadas pelas teclas numéricas (0-9).
- Controle de cores e luminosidade dos LEDs.
- Opções de configuração:
  - Tecla A: Desliga todos os LEDs.
  - Tecla B: Liga todos os LEDs em azul (100% de intensidade).
  - Tecla C: Liga todos os LEDs em vermelho (80% de intensidade).
  - Tecla D: Liga todos os LEDs em verde (50% de intensidade).
  - Tecla #: Liga todos os LEDs em branco (20% de intensidade).

## Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico W
- **Matriz de LEDs:** 5x5 WS2812 (Neopixel)
- **Teclado Matricial:** 4x4
- **Simulador:** Wokwi

## Como Funciona
1. O teclado matricial detecta qual tecla foi pressionada.
2. O microcontrolador interpreta a entrada e aciona a animação ou efeito luminoso correspondente.
3. As animações consistem em pelo menos 5 frames cada, garantindo fluidez visual.
4. A comunicação com os LEDs ocorre via protocolo WS2812, utilizando a biblioteca de controle PIO.

## Estrutura do Código
- **Inicialização:** Configura os pinos do teclado e da matriz de LEDs.
- **Leitura do Teclado:** Função `read_keypad()` verifica teclas pressionadas.
- **Execução de Comandos:** Funções de controle de LEDs acionam cores e animações.
- **Animações:** Cada tecla de 0 a 9 aciona um padrão luminoso distinto.

## Como Executar no Wokwi
1. Abra o simulador Wokwi.
2. Carregue o código no ambiente.
3. Execute a simulação e pressione as teclas para testar os efeitos.

## Autores
- [César Rebouças Costas] (https://github.com/cesarrcosta99)
- [Douglas Benevides Almeida] (https://github.com/almdoug)
 -[Andressa Silva Santos ] (https://github.com/Andressa-sstn)


