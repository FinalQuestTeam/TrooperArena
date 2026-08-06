#ifndef SPLASH_H
#define SPLASH_H

#include <genesis.h>

// Tela de abertura com o logo da desenvolvedora (FinalQuest). Mostra a arte
// centralizada por alguns segundos (ou até apertar um botão) e sai. Deve rodar
// antes do TILEGEN_init (usa a VRAM livre; depois a arte do jogo a sobrescreve).
void SPLASH_run(void);

#endif // SPLASH_H
