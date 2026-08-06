// Chão da arena: pinta o char-map 32x32 (16 tiles) e ladrilha o plano BG_B.
// O flash da bomba clareia a paleta do chão (FLOOR_setBright).

#include "system/video/floor.h"
#include "system/video/sprites/floor_data.h"
#include "core/game.h"     // ARENA_*

// pinta o char-map 32x32 em 16 tiles (ordem coluna, como os sprites)
static void paintFloor(u32 *buf)
{
    for (u16 y = 0; y < 32; y++)
    {
        for (u16 x = 0; x < 32; x++)
        {
            const char ch = FLOOR_ART[y][x];
            const u16 col = (ch <= '9') ? (u16) (ch - '0') : (u16) (ch - 'a' + 10);
            const u16 tile = (x / 8) * 4 + (y / 8);
            buf[tile * 8 + (y % 8)] |= (u32) col << ((7 - (x % 8)) * 4);
        }
    }
}

void FLOOR_init(void)
{
    u32 *buf = MEM_alloc(FLOOR_TILES * 8 * (u16) sizeof(u32));
    memset(buf, 0, FLOOR_TILES * 8 * (u16) sizeof(u32));
    paintFloor(buf);
    VDP_loadTileData(buf, TILE_USER_INDEX + FLOOR_TILE_BASE, FLOOR_TILES, DMA);
    MEM_free(buf);

    PAL_setColors(PAL3 * 16, FLOOR_PAL, 16, DMA);
}

void FLOOR_draw(void)
{
    // ladrilha só o INTERIOR da arena (sem passar por baixo do HUD nem da borda)
    const u16 x0 = ARENA_TILE_X + 1, x1 = ARENA_TILE_X + ARENA_TILES_W - 1;
    const u16 y0 = ARENA_TILE_Y + 1, y1 = ARENA_TILE_Y + ARENA_TILES_H - 1;

    for (u16 ty = y0; ty < y1; ty++)
        for (u16 tx = x0; tx < x1; tx++)
        {
            const u16 sub = (tx % 4) * 4 + (ty % 4);   // sub-tile 32x32 (ordem coluna)
            VDP_setTileMapXY(BG_B,
                TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_USER_INDEX + FLOOR_TILE_BASE + sub),
                tx, ty);
        }
}

void FLOOR_clear(void)
{
    VDP_clearPlane(BG_B, TRUE);
}

void FLOOR_setBright(u16 mix)
{
    u16 out[16];
    for (u16 i = 0; i < 16; i++)
    {
        const u16 c = FLOOR_PAL[i];
        u16 r = (c >> 1) & 7, g = (c >> 5) & 7, b = (c >> 9) & 7;
        r += ((7 - r) * mix) >> 4;      // aproxima do branco (7) por mix/16
        g += ((7 - g) * mix) >> 4;
        b += ((7 - b) * mix) >> 4;
        out[i] = (u16) ((r << 1) | (g << 5) | (b << 9));
    }
    PAL_setColors(PAL3 * 16, out, 16, CPU);
}
