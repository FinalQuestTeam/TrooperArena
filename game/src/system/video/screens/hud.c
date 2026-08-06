// HUD: linha 0 (mortes, inventário — bomba na posição 0 —, modo de tiro e vida).

#include "system/video/screens/hud.h"
#include "system/video/sprites/tiles.h"
#include "core/game.h"
#include "core/inventory.h"
#include "items/items.h"
#include "actors/shots.h"

void HUD_drawKills(u16 kills, u16 target)
{
    char buf[16];
    VDP_setTileMapXY(BG_A,
        TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + TILE_SKULL), 1, 0);
    sprintf(buf, "%02d/%02d", kills, target);
    VDP_drawText(buf, 3, 0);
}

void HUD_drawLife(s16 hp)
{
    char buf[12];
    sprintf(buf, "HP:%3d", hp < 0 ? 0 : hp);
    VDP_drawText(buf, 33, 0);
}

// tile do ícone 8x8 de cada tipo de item de inventário
static u16 iconTile(u8 type)
{
    switch (type)
    {
        case ITEM_BOMB:   return TILE_BOMB_ICON;
        case ITEM_CHAIN:  return TILE_BOLT;
        case ITEM_SHIELD: return TILE_ICON_SHIELD;
        case ITEM_ICE:    return TILE_ICON_ICE;
        case ITEM_FIRE:   return TILE_ICON_FIRE;
    }
    return TILE_BOMB_ICON;
}

// desenha um char isolado (usado para cursor e contagem, sem apagar o tile ícone)
static void putc0(char c, u16 col)
{
    char s[2]; s[0] = c; s[1] = 0;
    VDP_drawText(s, col, 0);
}

// layout centralizado na linha 0: inventário (4 pos × 3 cols) no centro da tela
// (col 20), modo de tiro ativo logo à direita.
#define INV_HUD_X   14      // 4×3 = 12 cols → 14..25 (centro ≈ 20)
#define MODE_HUD_X  27

// assinatura do estado desenhado (cursor + tipos/contagens dos slots + modo/cargas).
// Serve para redesenhar o inventário só quando algo muda (evita ~15 escritas no VDP
// por quadro à toa). Guardada abaixo por HUD_drawInventory.
static u32 s_invSig;

static u32 hudInvSig(void)
{
    u32 s = (u32) INV_selected();
    for (u8 i = 0; i < INV_POS; i++)
    {
        u8 type, count;
        INV_view(i, &type, &count);
        s = s * 131u + ((u32) type << 8) + count;
    }
    s = s * 131u + ((u32) SHOTS_mode() << 8) + SHOTS_modeCharges();
    return s;
}

void HUD_drawInventory(void)
{
    // 4 posições (0=bomba, 1-3 itens), 3 cols cada: [cursor] [ícone] [contagem]
    for (u8 i = 0; i < INV_POS; i++)
    {
        const u16 base = INV_HUD_X + i * 3;
        u8 type, count;
        INV_view(i, &type, &count);

        putc0(i == INV_selected() ? '>' : ' ', base);
        if (type == INV_EMPTY)
        {
            putc0('-', base + 1);
            putc0(' ', base + 2);
        }
        else
        {
            VDP_setTileMapXY(BG_A,
                TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + iconTile(type)),
                base + 1, 0);
            putc0((char) ('0' + (count % 10)), base + 2);
        }
    }

    // modo de tiro ativo (raio/gelo/fogo): ícone + cargas
    const u8 mode = SHOTS_mode();
    if (mode != SHOTMODE_NONE && SHOTS_modeCharges())
    {
        const u16 t = (mode == SHOTMODE_CHAIN) ? TILE_BOLT
                    : (mode == SHOTMODE_ICE)   ? TILE_ICON_ICE : TILE_ICON_FIRE;
        VDP_setTileMapXY(BG_A,
            TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + t), MODE_HUD_X, 0);
        putc0((char) ('0' + (SHOTS_modeCharges() % 10)), MODE_HUD_X + 1);
    }
    else { putc0(' ', MODE_HUD_X); putc0(' ', MODE_HUD_X + 1); }

    s_invSig = hudInvSig();     // baseline p/ o gating por quadro
}

// versão por quadro: só redesenha quando o estado muda (a maioria dos quadros não
// mexe no inventário → economiza as escritas no VDP). Os pontos de montagem da fase
// (startPhase/startTest/pause) chamam HUD_drawInventory direto, que sempre desenha.
void HUD_drawInventoryIfDirty(void)
{
    if (hudInvSig() == s_invSig) return;
    HUD_drawInventory();
}
