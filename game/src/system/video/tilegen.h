#ifndef TILEGEN_H
#define TILEGEN_H

#include <genesis.h>

// Fábrica de assets: gera proceduralmente todos os tiles do jogo (jogador,
// inimigos por forma+cor, projéteis, HUD, porta, raio) e monta a paleta PAL1.
// Roda uma única vez no boot; o mapa de índices está em system/video/sprites/tiles.h.
void TILEGEN_init(void);

// só a paleta PAL1 (o fundo do título a usa para a arte e depois a devolve)
void TILEGEN_palette(void);

#endif // TILEGEN_H
