// Item raio — os próximos 5 acertos fazem reação em cadeia (até 3 inimigos).
// Arte + efeito (dado em items/defs/).

#include "items/items.h"
#include "system/video/sprites/tiles.h"
#include "system/video/sprites/items_data.h"
#include "core/game.h"
#include "actors/shots.h"

void ITEMCHAIN_buildTile(u32 *tileBuf)
{
    ITEMS_paintTile16(tileBuf, TILE_CHAIN, ITEM_CHAIN_ART, ITEM_CHAIN_LEG, sizeof(ITEM_CHAIN_LEG) / sizeof(TilePixel));
}

void ITEMCHAIN_apply(void)
{
    SHOTS_grantChain();
}
