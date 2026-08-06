#ifndef FLOOR_H
#define FLOOR_H

#include <genesis.h>
#include "system/video/sprites/tiles.h"
#include "system/video/chars.h"    // CHARS_TILE_BASE / CHARS_DIR_TILES

// Chão da arena (pedra basketweave, recriado de img/floor.webp com a técnica de
// char-map). Tile 32x32 (16 tiles) ladrilhado no plano de fundo BG_B; paleta PAL3.
// Fica atrás de tudo (borda/HUD/elementos em BG_A, sprites por cima).

// base na VRAM: após os 8 rotacionados do mech do jogador (CHARS_TILE_BASE + 128)
#define FLOOR_TILE_BASE  (CHARS_TILE_BASE + 8 * CHARS_DIR_TILES)
#define FLOOR_TILES      16    // 32x32

void FLOOR_init(void);              // gera os tiles + carrega a paleta PAL3 (boot)
void FLOOR_draw(void);              // preenche o BG_B com o chão (início da fase)
void FLOOR_clear(void);             // limpa o BG_B (voltar ao título / preparação)
void FLOOR_setBright(u16 mix);      // 0..16: mistura o chão com branco (flash da bomba)

#endif // FLOOR_H
