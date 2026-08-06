// Projéteis: vetores compartilhados e limpeza. A arma/física do jogador está em
// actors/shots_player.c; os projéteis dos inimigos em actors/shots_enemy.c.

#include "actors/shots.h"

FShot pshots[MAX_PSHOTS];
FShot eshots[MAX_ESHOTS];

void SHOTS_clear(void)
{
    memset(pshots, 0, sizeof(pshots));
    memset(eshots, 0, sizeof(eshots));
}
