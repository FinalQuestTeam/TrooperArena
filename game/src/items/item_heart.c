// Item coração — cura 15 de vida (teto de 100). Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "actors/player.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"

#define HEART_HEAL  15

void ITEMHEART_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_HEART, ITEM_HEART_ART, ITEM_HEART_LEG,
                      sizeof(ITEM_HEART_LEG) / sizeof(TilePixel));
}

void ITEMHEART_apply(void)
{
    healPlayer(HEART_HEAL);
}
