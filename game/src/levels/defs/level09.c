// Fase 9: vazia — entram os pentágonos (elite).

#include "levels/levels.h"

static const u8 roster[] = { ENEMY_PEN_RED, ENEMY_PEN_YEL, ENEMY_TRI_YEL };

const LevelDef LEVEL09_def =
{
    .killTarget = 24, .spawnSeconds = 4, .spawnCount = 1,
    LVL_ROSTER(roster),
};
