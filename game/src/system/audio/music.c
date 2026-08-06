// Música de fundo — TEMPORARIAMENTE DESLIGADA.
//
// Histórico: o driver XGM (FM) corrompe VRAM e não sincroniza a DMA neste
// ambiente (até música comprovada falha). O driver PCM/DAC tocava de verdade,
// mas a DMA de vídeo (que trava o Z80) embolava a reprodução. Enquanto a
// abordagem definitiva não é decidida, o jogo roda sem BGM (os SFX em PSG
// funcionam normalmente). Ver core/flow.c (MUSIC_init/start após a DMA do boot).

#include "system/audio/music.h"

void MUSIC_init(void)   { }
void MUSIC_start(void)  { }
void MUSIC_update(void) { }
