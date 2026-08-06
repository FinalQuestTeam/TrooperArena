// Triângulo Amarelo — "Caçador": o perseguidor mais veloz (frágil).

#include "enemies/enemies.h"
#include "system/video/sprites/tiles.h"
#include "core/game.h"

const EnemyDef ENEMYDEF_tri_yellow =
{
    .name       = "Cacador",
    .shape      = SHAPE_TRIANGLE,
    .color      = COLOR_YELLOW,
    .size       = 16,
    .hp         = 1,
    .contactDmg = 8,
    .dropChance = 20,
    .speedFp    = 90,              // o mais rápido (ainda < jogador)
    .timerA     = 0,
    .timerB     = 0,
    .extra      = EXTRA_NONE,
    .tile       = TILE_TRI_YEL,
    .behavior   = ENEMYYELLOW_update,
};
