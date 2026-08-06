#ifndef SPRITES_H
#define SPRITES_H

#include <genesis.h>

// Lista de sprites de hardware, reconstruída a cada quadro (jogador, inimigos,
// itens, projéteis e os arcos do raio), encadeada por link.
void SPRITES_draw(void);    // monta e envia a lista do quadro
void SPRITES_hide(void);    // esconde todos (telas de preparação/pausa)

#endif // SPRITES_H
