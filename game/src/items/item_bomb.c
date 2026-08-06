// Item carga de bomba — +1 carga (teto de 2). Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"
#include "actors/bomb.h"

void ITEMBOMB_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_BOMB, ITEM_BOMB_ART, ITEM_BOMB_LEG, sizeof(ITEM_BOMB_LEG) / sizeof(TilePixel));
}

void ITEMBOMB_apply(void)
{
    BOMB_add();
}
