#ifndef TITLE_H
#define TITLE_H

#include <genesis.h>

// opção escolhida na tela inicial (TITLE_HELP e TITLE_CHARS são tratadas internamente)
#define TITLE_NEW_GAME  0
#define TITLE_TEST      1
#define TITLE_HELP      2
#define TITLE_CHARS     3

// roda a tela inicial (título + menu) e devolve a opção escolhida
u8 TITLE_run(void);

#endif // TITLE_H
