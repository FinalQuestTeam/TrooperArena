// Item fogo — vai para o inventário; ao ATIVAR (C), liga o modo de tiro de fogo
// (FIRE_CHARGES acertos que aplicam queimadura). Arte 16x16 em items_data.h.

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "actors/shots.h"

void ITEMFIRE_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_FIRE, ITEM_FIRE_ART, ITEM_FIRE_LEG,
                      sizeof(ITEM_FIRE_LEG) / sizeof(TilePixel));
}

void ITEMFIRE_apply(void)   // ativação (via inventário)
{
    SHOTS_setMode(SHOTMODE_FIRE, FIRE_CHARGES);
}
