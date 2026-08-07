#ifndef CHARS_H
#define CHARS_H

#include <genesis.h>
#include "system/video/sprites/tiles.h"        // NUM_TILES
#include "system/video/sprites/chars_data.h"   // CHARS_COUNT, CHARS_NAME

// Personagens (mechs) recriados de img/char.jpg com a técnica de char-map dos
// itens, em 32x32 (o maior sprite de hardware = 4x4 tiles). Só 3 direções são
// geradas (N/NE/E); as outras 5 saem por flip H/V de hardware — igual aos
// inimigos (ver enemygfx.c). Carregados na VRAM logo após os tiles do jogo.

#define CHARS_DIR_TILES 16              // 32x32 = 16 tiles por direção
#define CHARS_GEN_DIRS  3              // direções GERADAS (N/NE/E); o resto é flip
#define CHARS_TILES     (CHARS_GEN_DIRS * CHARS_DIR_TILES)  // 48 tiles por mech
#define CHARS_TILE_BASE (BOSS_ROT_BASE + BOSS_ROT_COUNT)  // após inimigos + chefe

#define PLAYER_CHAR     3              // verm_p (vermelho pequeno): o mech do jogador

// tile base + bits de flip da direção d (0=cima, horário) do personagem carregado.
// Combine dirTile com flipH/flipV ao montar o sprite (as 5 direções não geradas
// reaproveitam N/NE/E espelhadas).
u16 CHARS_dirTile(u8 dir);
u8  CHARS_dirFlipH(u8 dir);
u8  CHARS_dirFlipV(u8 dir);

// gera os rotacionados do personagem `which` na VRAM e carrega sua paleta (PAL2)
void CHARS_load(u8 which);

#endif // CHARS_H
