// Reabastecimento dos itens na FASE TESTE: mantém uma fileira com 1 de cada tipo
// no chão; ao coletar, o item some por 2 s antes de reaparecer (dá tempo de testar
// a evolução das balas/velocidade sem recoletar na hora).

#include "items/items.h"
#include "core/game.h"

#define TEST_SPACING    32
#define TEST_ROW_Y      176

static u16  testCd[ITEM_TYPES];     // frames restantes até reabastecer o tipo t
static bool testWas[ITEM_TYPES];    // o tipo t estava presente no quadro anterior?

void ITEMS_testReset(void)
{
    memset(testCd, 0, sizeof(testCd));
    memset(testWas, 0, sizeof(testWas));
}

void ITEMS_testEnsure(void)
{
    // fileira CENTRALIZADA na tela (centro em x=160), espaçada TEST_SPACING px
    const s16 startX = 160 - 8 - (s16) (ITEM_TYPES - 1) * (TEST_SPACING / 2);

    for (u8 t = 0; t < ITEM_TYPES; t++)
    {
        bool present = FALSE;
        for (u16 i = 0; i < MAX_ITEMS; i++)
            if (items[i].active && items[i].type == t) { present = TRUE; break; }

        if (testWas[t] && !present) testCd[t] = 2 * fps;    // acabou de ser coletado
        testWas[t] = present;

        if (!present)
        {
            if (testCd[t] > 0) testCd[t]--;
            else ITEMS_spawnAt(t, startX + t * TEST_SPACING, TEST_ROW_Y);
        }
    }
}
