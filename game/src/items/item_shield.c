// Item escudo — invencibilidade por 3 s. Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"
#include "actors/player.h"

void ITEMSHIELD_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_SHIELD, ITEM_SHIELD_ART, ITEM_SHIELD_LEG, sizeof(ITEM_SHIELD_LEG) / sizeof(TilePixel));
}

void ITEMSHIELD_apply(void)
{
    PLAYER_grantShield();
}
