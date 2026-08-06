// Item bala — +1 disparo (leque), até 5. Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"
#include "actors/shots.h"

void ITEMSHOTS_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_BULLET, ITEM_SHOTS_ART, ITEM_SHOTS_LEG, sizeof(ITEM_SHOTS_LEG) / sizeof(TilePixel));
}

void ITEMSHOTS_apply(void)
{
    SHOTS_addPower();
}
