#ifndef BLOOD_H
#define BLOOD_H

#include <genesis.h>

// Desenha uma poça de sangue (verde) 16x16 no BG_A, centrada em (cx, cy) px.
// Decalque persistente (sobre o chão, sob os sprites); some quando a fase é
// redesenhada. Chamado ao matar um inimigo (enemies.c).
void BLOOD_splat(s16 cx, s16 cy);

#endif // BLOOD_H
