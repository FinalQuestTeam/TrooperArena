// Tela de pausa (separada do main): desenha as infos da fase e as opções, e
// redesenha a cena ao retomar. As transições de estado ficam em core/flow.c.

#include "system/video/screens/pause.h"
#include "core/game.h"
#include "levels/levels.h"
#include "levels/test.h"
#include "system/video/screens/hud.h"
#include "actors/player.h"      // hp
#include "actors/bomb.h"

void PAUSE_show(void)
{
    if (currentLevel == 0)
    {
        TEST_drawPauseText();       // "PAUSA - FASE TESTE n"
    }
    else
    {
        char buf[16];
        sprintf(buf, "PAUSA - FASE %d", currentLevel);
        VDP_drawText(buf, 13, 5);
        LEVEL_drawInfo(currentLevel, 8, 7);     // inclui os sprites dos inimigos
    }
    VDP_drawText("START PARA CONTINUAR", 10, 19);
    VDP_drawText("B PARA VOLTAR AO TITULO", 8, 21);
}

void PAUSE_resume(u16 kills)
{
    LEVEL_draw(currentLevel);
    if (currentLevel == 0) TEST_draw();     // porta é redesenhada com a arena
    HUD_drawKills(kills, LEVEL_current()->killTarget);
    HUD_drawLife(hp);
    HUD_drawInventory();
}
