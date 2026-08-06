// FASES TESTE (dados) — sandbox. A lógica fica em levels/test.c.
//   estágio 1: arena com lava + um dummy (testa tiros/lava).
//   estágio 2: 3 alvos frágeis, sem lava (testa a reação em cadeia do raio).

#include "levels/levels.h"

static const u8 eTest[]  = { ENEMY_DUMMY };
static const u8 eTest2[] = { ENEMY_TARGET };
static const u8 eTest3[] = { ENEMY_BOSS, ENEMY_TRI_YEL };   // chefe + lacaios

const LevelDef TEST_def =
{
    .killTarget = 99, .spawnSeconds = 99, .spawnCount = 0,
    .lava = LAVA_PAIR, .lavaCount = LAYOUT_RECTS,   // para testar o dano da lava
    LVL_ROSTER(eTest),
};

const LevelDef TEST2_def =
{
    .killTarget = 99, .spawnSeconds = 99, .spawnCount = 0,
    LVL_ROSTER(eTest2),
};

// estágio 3: arena do CHEFE (Besouro) — acessível pela porta à direita da teste 2
const LevelDef TEST3_def =
{
    .killTarget = 99, .spawnSeconds = 99, .spawnCount = 0,
    LVL_ROSTER(eTest3),
};
