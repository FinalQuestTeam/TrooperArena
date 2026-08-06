// Fase teste (sandbox): a porta entre os estágios, a manutenção dos inimigos de
// cada estágio e a caixa de infos do jogador. Fica aqui (e não em main.c) para
// seguir o padrão das demais fases — main.c cuida só da máquina de estados.
//
//   estágio 1: um dummy resistente que reaparece na hora (testa tiros/lava).
//   estágio 2: 3 alvos frágeis agrupados, que reaparecem a cada 2 s (testa a
//              reação em cadeia do item raio).

#include "levels/test.h"
#include "system/video/sprites/tiles.h"
#include "levels/levels.h"
#include "core/game.h"
#include "enemies/enemies.h"
#include "items/items.h"
#include "actors/shots.h"
#include "actors/player.h"

// posições fixas dos 3 alvos da teste 2 (canto sup. esq. do sprite 24x24)
static const struct { s16 x, y; } SPOTS[3] = { { 130, 64 }, { 160, 64 }, { 190, 64 } };

// portas (em tiles): à direita na teste 1 (→ teste 2), à esquerda na teste 2
#define DOOR_W          3
#define DOOR_H          4
#define DOOR_Y          10          // linha do topo da porta
#define DOOR1_X         35          // teste 1: porta à direita
#define DOOR2_X         2           // teste 2: porta à esquerda

#define TEST2_SPAWN_S   2           // intervalo de aparição dos alvos (teste 2)

static u16 spawnTimer;              // contagem p/ reaparecer os alvos (teste 2)

// o jogador (16x16 em px,py) encosta no retângulo de tiles [tx,ty]+[tw,th]?
static bool playerOnTiles(u16 tx, u16 ty, u16 tw, u16 th)
{
    const s16 x1 = tx * 8, y1 = ty * 8;
    const s16 x2 = (tx + tw) * 8, y2 = (ty + th) * 8;
    return (px < x2) && (px + 16 > x1) && (py < y2) && (py + 16 > y1);
}

void TEST_draw(void)
{
    const u16 attr = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USER_INDEX + TILE_DOOR);
    spawnTimer = TEST2_SPAWN_S * fps;   // 1ª leva de alvos após o intervalo

    if (testStage == 2)
    {
        VDP_fillTileMapRect(BG_A, attr, DOOR2_X, DOOR_Y, DOOR_W, DOOR_H);  // esq → teste 1
        VDP_drawText("<- TESTE 1", 2, 8);
        VDP_fillTileMapRect(BG_A, attr, DOOR1_X, DOOR_Y, DOOR_W, DOOR_H);  // dir → CHEFE
        VDP_drawText("CHEFE ->", 30, 8);
    }
    else if (testStage == 3)
    {
        VDP_fillTileMapRect(BG_A, attr, DOOR2_X, DOOR_Y, DOOR_W, DOOR_H);  // esq → teste 2
        VDP_drawText("<- TESTE 2", 2, 8);
        VDP_drawText("CHEFE: BESOURO", 13, 3);
    }
    else
    {
        VDP_fillTileMapRect(BG_A, attr, DOOR1_X, DOOR_Y, DOOR_W, DOOR_H);  // dir → teste 2
        VDP_drawText("TESTE 2 ->", 28, 8);
    }
}

u8 TEST_update(void)
{
    // portas: pisar na área de passagem troca de estágio de teste
    if (testStage == 1 && playerOnTiles(DOOR1_X, DOOR_Y, DOOR_W, DOOR_H)) return 2;
    if (testStage == 2)
    {
        if (playerOnTiles(DOOR2_X, DOOR_Y, DOOR_W, DOOR_H)) return 1;   // esq → teste 1
        if (playerOnTiles(DOOR1_X, DOOR_Y, DOOR_W, DOOR_H)) return 3;   // dir → CHEFE
    }
    if (testStage == 3 && playerOnTiles(DOOR2_X, DOOR_Y, DOOR_W, DOOR_H)) return 2;

    // inimigos do estágio
    if (testStage == 2)
    {
        // alvos frágeis: quando todos caem, reaparecem após o intervalo
        if (ENEMIES_activeCount() == 0)
        {
            if (spawnTimer) spawnTimer--;
            else
            {
                for (u16 i = 0; i < 3; i++)
                    ENEMIES_spawnAt(ENEMY_TARGET, SPOTS[i].x, SPOTS[i].y);
                spawnTimer = TEST2_SPAWN_S * fps;
            }
        }
    }
    else if (testStage == 3)
    {
        // arena do chefe: (re)aparece quando não há mais nada vivo
        if (ENEMIES_activeCount() == 0) ENEMIES_spawnAt(ENEMY_BOSS, 144, 48);
    }
    else
    {
        if (ENEMIES_activeCount() == 0) ENEMIES_spawnAt(ENEMY_DUMMY, 148, 80);
    }

    ITEMS_testEnsure();

    // caixa de infos do jogador (evolução de tiros/velocidade no sandbox)
    char buf[20];
    VDP_drawText("-- JOGADOR --", 2, 3);
    sprintf(buf, "BALAS:    %d", SHOTS_power());
    VDP_drawText(buf, 2, 4);
    sprintf(buf, "VELOC: +%3d%%", PLAYER_speedBonus());
    VDP_drawText(buf, 2, 5);
    return 0;
}

void TEST_drawPauseText(void)
{
    VDP_drawText(testStage == 3 ? "PAUSA - CHEFE (TESTE)"
               : testStage == 2 ? "PAUSA - FASE TESTE 2"
                                : "PAUSA - FASE TESTE 1", 10, 9);
}
