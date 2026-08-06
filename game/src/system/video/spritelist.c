// Montagem da lista de sprites de hardware a cada quadro, encadeada por link.
// Ordem: jogador → inimigos (com rotação de flicker) → itens → projéteis →
// arcos do raio → sombra do jogador (prioridade baixa, fica sob os demais).

#include "system/video/spritelist.h"
#include "system/video/sprites/tiles.h"
#include "system/video/chars.h"
#include "system/video/enemygfx.h"
#include "system/video/chainfx.h"
#include "core/game.h"
#include "actors/player.h"
#include "actors/shots.h"
#include "enemies/enemies.h"
#include "items/items.h"

// direção do mech do jogador (0..7, horário a partir de cima) a partir de
// faceX/faceY (ambos em {-1,0,1}); índice [faceY+1][faceX+1]
static const u8 FACE_DIR[3][3] =
{
    { 7, 0, 1 },    // faceY = -1: cima-esq, cima, cima-dir
    { 6, 0, 2 },    // faceY =  0: esquerda, (parado), direita
    { 5, 4, 3 },    // faceY = +1: baixo-esq, baixo, baixo-dir
};

void SPRITES_draw(void)
{
    static u16 frame;
    u16 n = 0;
    frame++;

    CHAINFX_update();       // decai os arcos do raio (um passo por quadro)

    // jogador: mech vermelho pequeno (32x32, PAL2) centrado na hitbox 16x16 e
    // virado conforme a direção. Some fora da tela ao piscar (invencibilidade).
    const u8 dir = FACE_DIR[faceY + 1][faceX + 1];
    const s16 pyShown = PLAYER_hidden() ? -64 : (s16) (py - 8);
    VDP_setSpriteFull(n, (s16) (px - 8), pyShown, SPRITE_SIZE(4, 4),
        TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, TILE_USER_INDEX + CHARS_DIR_TILE(dir)), n + 1);
    n++;

    // rotação de flicker: a cada quadro varre os inimigos a partir de um índice
    // diferente. Quando uma linha de varredura estoura o limite do VDP (20 sprites
    // / 320 px), o hardware descarta os sprites de maior link (os últimos da lista);
    // girando a ordem, o descarte se distribui entre os inimigos (pisca uniforme)
    // em vez de sumir sempre os mesmos. O jogador fica fixo no topo (link 0).
    for (u16 k = 0; k < MAX_ENEMIES; k++)
    {
        const u16 i = (k + frame) & (MAX_ENEMIES - 1);  // MAX_ENEMIES é potência de 2
        if (!enemies[i].active) continue;

        // blindado: pisca para sinalizar "não adianta atirar agora"
        if (enemies[i].shielded && ((frame >> 2) & 1)) continue;

        // tamanho pela config; a direção (8) encara o jogador
        const EnemyDef *d = ENEMY_DEF(&enemies[i]);
        const s16 half = d->size / 2;
        const u8 edir = ENEMYGFX_faceDir((px + 8) - (enemies[i].x + half),
                                         (py + 8) - (enemies[i].y + half));
        // queimando: pisca (feedback do dano por tempo)
        if (enemies[i].burn && ((frame >> 2) & 1)) continue;

        // CHEFE (48x48): 4 quadrantes 24x24 (limite de 32px do hardware)
        if (d->size == 48)
        {
            const u16 base = TILE_USER_INDEX + ENEMYGFX_dirTile(enemies[i].type, edir);
            for (u16 q = 0; q < 4; q++)
            {
                const s16 qx = (s16) ((q & 1) * 24), qy = (s16) ((q >> 1) * 24);
                VDP_setSpriteFull(n, enemies[i].x + qx, enemies[i].y + qy, SPRITE_SIZE(3, 3),
                    TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, base + q * 9), n + 1);
                n++;
            }
            if (enemies[i].frozen)      // gelo: overlay ciano nos 4 quadrantes
                for (u16 q = 0; q < 4; q++)
                {
                    const s16 qx = (s16) ((q & 1) * 24), qy = (s16) ((q >> 1) * 24);
                    VDP_setSpriteFull(n, enemies[i].x + qx, enemies[i].y + qy, SPRITE_SIZE(3, 3),
                        TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, TILE_USER_INDEX + TILE_FROST), n + 1);
                    n++;
                }
            continue;
        }

        const u16 size = (d->size == 32) ? SPRITE_SIZE(4, 4)
                       : (d->size == 24) ? SPRITE_SIZE(3, 3)
                                         : SPRITE_SIZE(2, 2);

        VDP_setSpriteFull(n, enemies[i].x, enemies[i].y, size,
            TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE,
                TILE_USER_INDEX + ENEMYGFX_dirTile(enemies[i].type, edir)), n + 1);
        n++;

        // congelado: overlay xadrez ciano por cima (tom azulado)
        if (enemies[i].frozen)
        {
            VDP_setSpriteFull(n, enemies[i].x, enemies[i].y, size,
                TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, TILE_USER_INDEX + TILE_FROST), n + 1);
            n++;
        }
    }

    n = ITEMS_draw(n);

    for (u16 i = 0; i < MAX_PSHOTS; i++)
    {
        if (!pshots[i].active) continue;
        VDP_setSpriteFull(n, (s16) (pshots[i].x >> 6), (s16) (pshots[i].y >> 6), SPRITE_SIZE(1, 1),
            TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, TILE_USER_INDEX + TILE_PSHOT), n + 1);
        n++;
    }
    for (u16 i = 0; i < MAX_ESHOTS; i++)
    {
        if (!eshots[i].active) continue;
        VDP_setSpriteFull(n, (s16) (eshots[i].x >> 6), (s16) (eshots[i].y >> 6), SPRITE_SIZE(1, 1),
            TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, TILE_USER_INDEX + TILE_ESHOT), n + 1);
        n++;
    }

    n = CHAINFX_draw(n);            // arcos do raio em cadeia (por cima)

    // sombra do jogador (destaca do chão): círculo pequeno de prioridade baixa
    // (atrás dos demais sprites, sobre o chão), centrado sob o mech e um pouco abaixo
    if (!PLAYER_hidden())
    {
        VDP_setSpriteFull(n, px, (s16) (py + 4), SPRITE_SIZE(2, 2),
            TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + TILE_SHADOW), n + 1);
        n++;
    }

    VDP_setSpriteLink(n - 1, 0);    // fecha a cadeia (último link = 0)
    VDP_updateSprites(n, DMA_QUEUE);
}

void SPRITES_hide(void)
{
    VDP_setSpriteFull(0, 0, -32, SPRITE_SIZE(1, 1),
        TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + TILE_PSHOT), 0);
    VDP_updateSprites(1, DMA_QUEUE);
}
