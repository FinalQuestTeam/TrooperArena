// Comportamento COR VERMELHA — atirador: dispara mirado no jogador.
// Quadrado: fixo. Triângulo: persegue enquanto atira (agressivo).
// Pentágono: leque de tiros (extra SPREAD). Cadência vem de def->timerA.

#include "enemies/enemies.h"
#include "actors/shots.h"
#include "core/game.h"

void ENEMYRED_update(Enemy *e, const EnemyDef *def)
{
    if (def->speedFp > 0) ENEMIES_chase(e, def->speedFp);   // formas que se movem

    if (e->fireTimer) e->fireTimer--;
    else
    {
        if (def->extra == EXTRA_SPREAD) fireEnemySpread(e->x, e->y, 3);
        else fireEnemyShot(e->x, e->y);
        e->fireTimer = def->timerA * fps / 10;              // décimos de s -> frames
    }
}
