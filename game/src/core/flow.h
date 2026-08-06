#ifndef FLOW_H
#define FLOW_H

#include <genesis.h>

// Fluxo do jogo: inicializa os subsistemas (áudio, splash, gráficos) e roda a
// máquina de estados (preparação → jogo → pausa → game over → concluída) num
// laço infinito. Chamado pelo main; nunca retorna.
void FLOW_run(void);

#endif // FLOW_H
