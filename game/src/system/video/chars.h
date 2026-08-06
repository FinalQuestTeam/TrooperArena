#ifndef CHARS_H
#define CHARS_H

#include <genesis.h>
#include "system/video/sprites/tiles.h"        // NUM_TILES
#include "system/video/sprites/chars_data.h"   // CHARS_COUNT, CHARS_NAME

// Personagens (mechs) recriados de img/char.jpg com a técnica de char-map dos
// itens, em 32x32 (o maior sprite de hardware = 4x4 tiles). Cada personagem é
// gerado nas 8 direções (rotações) e carregado na VRAM logo após os tiles do jogo.

#define CHARS_DIR_TILES 16              // 32x32 = 16 tiles por direção
#define CHARS_TILE_BASE (BOSS_ROT_BASE + BOSS_ROT_COUNT)  // após inimigos + chefe

#define PLAYER_CHAR     3              // verm_p (vermelho pequeno): o mech do jogador

// tile base da direção d (0=cima, horário) do personagem carregado
#define CHARS_DIR_TILE(d) (CHARS_TILE_BASE + (d) * CHARS_DIR_TILES)

// gera os 8 rotacionados do personagem `which` na VRAM e carrega sua paleta (PAL2)
void CHARS_load(u8 which);

#endif // CHARS_H
