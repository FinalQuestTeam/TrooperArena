// Flash de tela (impacto/bomba): clareia a paleta do chão (PAL3) até o branco e
// decai. Como o chão (BG_B) cobre a arena, isso dá o efeito de tela piscando.

#include "system/video/flash.h"
#include "system/video/floor.h"

static u16 flashTimer;

void FLASH_trigger(void)
{
    flashTimer = 18;            // ~0,3 s de flash decaindo
}

void FLASH_update(void)
{
    if (flashTimer == 0) return;

    flashTimer--;
    u16 mix;                                    // 0 = normal, 16 = branco
    if (flashTimer >= 12)     mix = 16;
    else if (flashTimer >= 8) mix = 10;
    else if (flashTimer >= 4) mix = 5;
    else                      mix = 0;
    FLOOR_setBright(mix);
}

void FLASH_stop(void)
{
    flashTimer = 0;
    FLOOR_setBright(0);
}
