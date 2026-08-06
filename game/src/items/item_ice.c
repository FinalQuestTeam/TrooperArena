// Item gelo — vai para o inventário; ao ATIVAR (C), liga o modo de tiro de gelo
// (ICE_CHARGES acertos que congelam o inimigo). Arte 16x16 em items_data.h.

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "actors/shots.h"

void ITEMICE_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_ICE, ITEM_ICE_ART, ITEM_ICE_LEG,
                      sizeof(ITEM_ICE_LEG) / sizeof(TilePixel));
}

void ITEMICE_apply(void)   // ativação (via inventário)
{
    SHOTS_setMode(SHOTMODE_ICE, ICE_CHARGES);
}
