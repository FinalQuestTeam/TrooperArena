// Poça de sangue (verde) ao matar inimigos: decalque 16x16 no BG_A, montado por
// 4 cópias espelhadas do tile TILE_BLOOD (quarto de disco), com o canto denso
// voltado para o centro. Fica sobre o chão (BG_B) e sob os sprites.

#include "system/video/blood.h"
#include "system/video/sprites/tiles.h"
#include "core/game.h"

void BLOOD_splat(s16 cx, s16 cy)
{
    // canto superior-esquerdo (em tiles) da poça 16x16 centrada em (cx, cy)
    s16 tx = (s16) ((cx - 8) / 8);
    s16 ty = (s16) ((cy - 8) / 8);

    // mantém dentro do interior da arena (não pinta borda/HUD)
    if (tx < ARENA_TILE_X + 1) tx = ARENA_TILE_X + 1;
    if (tx > ARENA_TILE_X + ARENA_TILES_W - 3) tx = ARENA_TILE_X + ARENA_TILES_W - 3;
    if (ty < ARENA_TILE_Y + 1) ty = ARENA_TILE_Y + 1;
    if (ty > ARENA_TILE_Y + ARENA_TILES_H - 3) ty = ARENA_TILE_Y + ARENA_TILES_H - 3;

    const u16 t = TILE_USER_INDEX + TILE_BLOOD;
    // TILE_ATTR_FULL(pal, prio, flipV, flipH, index) — prio FALSE: fica sob os sprites
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, FALSE, TRUE,  TRUE,  t), tx,     ty);      // sup-esq
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, FALSE, TRUE,  FALSE, t), tx + 1, ty);      // sup-dir
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, TRUE,  t), tx,     ty + 1);  // inf-esq
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, t), tx + 1, ty + 1);  // inf-dir
}
