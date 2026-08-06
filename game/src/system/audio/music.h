#ifndef MUSIC_H
#define MUSIC_H

#include <genesis.h>

// música de fundo via driver PCM (canal DAC do YM2612), independente dos
// efeitos sonoros no PSG. Intro (0-15s) toca uma vez, depois o trecho de
// loop (16-45s) repete indefinidamente.

void MUSIC_init(void);      // carrega o driver PCM — chamar uma vez no boot
void MUSIC_start(void);     // inicia a intro
void MUSIC_update(void);    // ao fim da intro, engata o loop — chamar no loop

#endif // MUSIC_H
