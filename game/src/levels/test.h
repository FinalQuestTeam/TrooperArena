#ifndef LEVELS_TEST_H
#define LEVELS_TEST_H

#include <genesis.h>

// Fase teste (currentLevel == 0): lógica própria do sandbox, mantida fora do
// main.c. O estágio atual fica em testStage (declarado em levels.h).

// desenha os elementos próprios da fase teste (a porta) e arma o timer de
// aparição dos alvos — chamar logo após LEVEL_draw/LEVEL_load
void TEST_draw(void);

// atualização por quadro: porta, inimigos do estágio e a caixa de infos.
// devolve o estágio a (re)carregar se o jogador cruzou a porta, ou 0 se ficar.
u8 TEST_update(void);

// texto da tela de pausa da fase teste (conforme o estágio atual)
void TEST_drawPauseText(void);

#endif // LEVELS_TEST_H
