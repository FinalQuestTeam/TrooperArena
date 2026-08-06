#ifndef PAUSE_H
#define PAUSE_H

#include <genesis.h>

// Tela de pausa: mostra as condições/infos da fase e as opções (continuar/sair).
// A máquina de estados (em core/flow.c) cuida das transições; aqui só desenha.

// desenha a tela de pausa da fase atual (usa currentLevel)
void PAUSE_show(void);

// redesenha a fase e o HUD ao retomar (sem zerar o progresso dos elementos)
void PAUSE_resume(u16 kills);

#endif // PAUSE_H
