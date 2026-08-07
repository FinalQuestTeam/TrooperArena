// Gera os tiles do mech (32x32) a partir dos char-maps. Só 3 direções são
// geradas — N (0), NE (1), E (2) — por rotação (cardeais a 90°, diagonais a 45°).
// As outras 5 saem por espelhamento de hardware do sprite (H/V-flip), pois são
// reflexões dessas — mesma técnica dos inimigos (enemygfx.c). Sem divisão no laço.

#include "system/video/chars.h"
#include "system/video/sprites/chars_data.h"

// seno/cosseno × 256 para dir*45° (0 = cima, sentido horário)
static const s16 COS8[8] = { 256, 181,   0, -181, -256, -181,    0,  181 };
static const s16 SIN8[8] = {   0, 181, 256,  181,    0, -181, -256, -181 };

// direção 0..7 → slot gerado (N=0/NE=1/E=2) + flips (idem enemygfx.c)
//   dir:      N   NE  E   SE  S   SW  W   NW
static const u8 DIR_SLOT [8] = { 0,  1,  2,  1,  0,  1,  2,  1 };
static const u8 DIR_HFLIP[8] = { 0,  0,  0,  0,  0,  1,  1,  1 };
static const u8 DIR_VFLIP[8] = { 0,  0,  0,  1,  1,  1,  0,  0 };

u16 CHARS_dirTile(u8 dir)  { return CHARS_TILE_BASE + DIR_SLOT[dir] * CHARS_DIR_TILES; }
u8  CHARS_dirFlipH(u8 dir) { return DIR_HFLIP[dir]; }
u8  CHARS_dirFlipV(u8 dir) { return DIR_VFLIP[dir]; }

// pinta o char-map 32x32 rotacionado por `dir` em 16 tiles (ordem coluna),
// a partir de baseTile. Cada caractere hex do mapa é um índice de cor (0 = vazio).
static void paint32(u32 *buf, u16 baseTile, const char *const art[32], u8 dir)
{
    const s16 c = COS8[dir], s = SIN8[dir];

    for (u16 oy = 0; oy < 32; oy++)
    {
        for (u16 ox = 0; ox < 32; ox++)
        {
            // amostra a origem rotacionando o pixel de saída (×2 p/ meio-pixel do centro)
            const s16 dx = (s16) (2 * ox - 31);
            const s16 dy = (s16) (2 * oy - 31);
            const s16 sx = (s16) ((((dx * c + dy * s) >> 8) + 31) >> 1);
            const s16 sy = (s16) (((-(dx * s) + dy * c) >> 8) + 31) >> 1;
            if ((sx < 0) || (sx > 31) || (sy < 0) || (sy > 31)) continue;

            const char ch = art[sy][sx];
            if (ch == '0') continue;
            const u16 col = (ch <= '9') ? (u16) (ch - '0') : (u16) (ch - 'a' + 10);

            const u16 tile = baseTile + (ox / 8) * 4 + (oy / 8);   // 32x32: colTiles = 4
            buf[tile * 8 + (oy % 8)] |= (u32) col << ((7 - (ox % 8)) * 4);
        }
    }
}

void CHARS_load(u8 which)
{
    const u16 count = CHARS_TILES;              // 48 tiles (3 direções × 16)
    u32 *buf = MEM_alloc(count * 8 * (u16) sizeof(u32));
    memset(buf, 0, count * 8 * (u16) sizeof(u32));

    for (u8 d = 0; d < CHARS_GEN_DIRS; d++)     // gera N/NE/E; o resto é flip
        paint32(buf, d * CHARS_DIR_TILES, CHARS_ART[which], d);

    VDP_loadTileData(buf, TILE_USER_INDEX + CHARS_TILE_BASE, count, DMA);
    MEM_free(buf);

    PAL_setColors(PAL2 * 16, CHARS_PAL[which], 16, DMA);
}
