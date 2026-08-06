#ifndef CHAINFX_H
#define CHAINFX_H

#include <genesis.h>

// Efeito visual do item raio: arcos elétricos em ziguezague que ligam o ponto
// do tiro a cada inimigo atingido pela reação em cadeia. Vive poucos quadros.

// apaga todos os arcos (início/troca de fase)
void CHAINFX_reset(void);

// registra um arco de (x0,y0) até (x1,y1) — em pixels de tela
void CHAINFX_add(s16 x0, s16 y0, s16 x1, s16 y1);

// decai a vida dos arcos (um passo por quadro desenhado)
void CHAINFX_update(void);

// acrescenta os sprites dos arcos vivos à lista (a partir do índice n);
// devolve o novo n
u16 CHAINFX_draw(u16 n);

#endif // CHAINFX_H
