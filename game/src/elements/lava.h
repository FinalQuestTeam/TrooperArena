#ifndef LAVA_H
#define LAVA_H

#include <genesis.h>
#include "elements/elements.h"

// dano progressivo: começa em 5/s e sobe 5/s a cada segundo pisando, até 25/s
#define LAVA_DPS_MIN    5
#define LAVA_DPS_MAX    25
#define LAVA_DPS_STEP   5

// desenha o padrão do tile TILE_LAVA no buffer de tiles procedurais
void LAVA_buildTile(u32 *tileBuf);

// preenche os retângulos de lava no plano BG_A
void LAVA_draw(const TileRect *rects, u16 count);

// zera a progressão de dano (trocar de fase / reiniciar)
void LAVA_reset(void);

// aplica o dano progressivo ao jogador se ele estiver sobre a lava
void LAVA_update(const TileRect *rects, u16 count);

#endif // LAVA_H
