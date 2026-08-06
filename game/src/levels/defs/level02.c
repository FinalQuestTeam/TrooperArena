// Fase 2: vazia — entra o laranja (investida).

#include "levels/levels.h"

static const u8 roster[] = { ENEMY_SQ_RED, ENEMY_SQ_YEL, ENEMY_SQ_ORG };

const LevelDef LEVEL02_def =
{
    .killTarget = 12, .spawnSeconds = 5, .spawnCount = 1,
    LVL_ROSTER(roster),
};
