// Item botinha — +10% de velocidade, até +50%. Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"
#include "actors/player.h"

void ITEMSPEED_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_BOOT, ITEM_SPEED_ART, ITEM_SPEED_LEG, sizeof(ITEM_SPEED_LEG) / sizeof(TilePixel));
}

void ITEMSPEED_apply(void)
{
    PLAYER_addSpeed();
}
