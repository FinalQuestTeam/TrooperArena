// Comportamento COR AMARELA — perseguidor: vai ao encontro do jogador (contato).
// Compartilhado por todas as formas amarelas; a velocidade vem do EnemyDef.

#include "enemies/enemies.h"
#include "core/game.h"

void ENEMYYELLOW_update(Enemy *e, const EnemyDef *def)
{
    ENEMIES_chase(e, def->speedFp);
}
