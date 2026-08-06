#ifndef FLASH_H
#define FLASH_H

#include <genesis.h>

// Flash de tela: a cor de fundo (índice 0) pisca branco e decai até o preto,
// aparecendo por todos os tiles transparentes do interior da arena (impacto/bomba).
void FLASH_trigger(void);   // dispara o flash
void FLASH_update(void);    // decaimento por quadro — chamar sempre no loop
void FLASH_stop(void);      // apaga imediatamente (troca de fase)

#endif // FLASH_H
