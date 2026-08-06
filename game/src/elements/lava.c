// Lava: área que causa dano progressivo enquanto o jogador estiver em cima.
// O dano ignora a invencibilidade (é ambiente, não inimigo).

#include "elements/lava.h"
#include "actors/player.h"
#include "system/video/sprites/tiles.h"
#include "core/game.h"
#include "system/audio/sfx.h"

static u16 lavaTime;    // frames seguidos pisando na lava
static u16 lavaTick;    // frames até o próximo 1 de dano

void LAVA_buildTile(u32 *tileBuf)
{
    // vermelho com respingos laranja
    for (u16 y = 0; y < 8; y++)
    {
        for (u16 x = 0; x < 8; x++)
        {
            u16 c = (((x * 3 + y * 5 + (y >> 1)) & 7) < 2) ? 5 : 3;
            tileBuf[TILE_LAVA * 8 + y] |= (u32) c << ((7 - x) * 4);
        }
    }
}

void LAVA_draw(const TileRect *rects, u16 count)
{
    const u16 attr = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + TILE_LAVA);
    for (u16 i = 0; i < count; i++)
        VDP_fillTileMapRect(BG_A, attr, rects[i].x, rects[i].y, rects[i].w, rects[i].h);
}

static bool isOn(const TileRect *rects, u16 count, s16 cx, s16 cy)
{
    for (u16 i = 0; i < count; i++)
    {
        const s16 x = rects[i].x * 8, y = rects[i].y * 8;
        const s16 w = rects[i].w * 8, h = rects[i].h * 8;
        if ((cx >= x) && (cx < x + w) && (cy >= y) && (cy < y + h)) return TRUE;
    }
    return FALSE;
}

void LAVA_reset(void)
{
    lavaTime = 0;
    lavaTick = 0;
}

void LAVA_update(const TileRect *rects, u16 count)
{
    // usa o centro do jogador (16x16)
    if (isOn(rects, count, px + 8, py + 8))
    {
        SFX_ambient(SFX_AMB_LAVA);      // som de queimando

        u16 dps = LAVA_DPS_MIN + LAVA_DPS_STEP * (lavaTime / fps);
        if (dps > LAVA_DPS_MAX) dps = LAVA_DPS_MAX;

        if (lavaTime < 0xFFFF) lavaTime++;

        if (lavaTick) lavaTick--;
        else
        {
            loseHp(1);              // 1 de HP por tick, na taxa atual
            lavaTick = fps / dps;
        }
    }
    else LAVA_reset();
}
