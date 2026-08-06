// Gera os tiles dos personagens (mechs 32x32) a partir dos char-maps, nas 8
// direções. As cardeais (0/2/4/6) são rotações exatas de 90°; as diagonais
// (1/3/5/7) são rotação de 45° por vizinho-mais-próximo. Sem divisão no laço.

#include "system/video/chars.h"
#include "system/video/sprites/chars_data.h"

// seno/cosseno × 256 para dir*45° (0 = cima, sentido horário)
static const s16 COS8[8] = { 256, 181,   0, -181, -256, -181,    0,  181 };
static const s16 SIN8[8] = {   0, 181, 256,  181,    0, -181, -256, -181 };

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
    const u16 count = 8 * CHARS_DIR_TILES;      // 128 tiles (8 direções × 16)
    u32 *buf = MEM_alloc(count * 8 * (u16) sizeof(u32));
    memset(buf, 0, count * 8 * (u16) sizeof(u32));

    for (u8 d = 0; d < 8; d++)
        paint32(buf, d * CHARS_DIR_TILES, CHARS_ART[which], d);

    VDP_loadTileData(buf, TILE_USER_INDEX + CHARS_TILE_BASE, count, DMA);
    MEM_free(buf);

    PAL_setColors(PAL2 * 16, CHARS_PAL[which], 16, DMA);
}
