// Fase 5: vazia — triângulos atirador/caçador.

#include "levels/levels.h"

static const u8 roster[] = { ENEMY_TRI_RED, ENEMY_TRI_YEL, ENEMY_SQ_ORG };

const LevelDef LEVEL05_def =
{
    .killTarget = 18, .spawnSeconds = 4, .spawnCount = 1,
    LVL_ROSTER(roster),
};
