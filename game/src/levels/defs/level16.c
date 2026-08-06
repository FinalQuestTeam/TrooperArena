// Fase 16 — CHEFE (Lacraia). Só o chefe (que invoca seus próprios lacaios).
// O chefe é gerado no início da fase (core/flow.c) e derrotá-lo encerra o jogo
// (tela de fim → título). Sem spawn regular; TRI_YEL entra no roster só para a
// arte dos lacaios ficar carregada na VRAM.

#include "levels/levels.h"

static const u8 roster[] = { ENEMY_BOSS, ENEMY_TRI_YEL };

const LevelDef LEVEL16_def =
{
    .killTarget = 1, .spawnSeconds = 99, .spawnCount = 0,
    LVL_ROSTER(roster),
};
