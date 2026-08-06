// Terrenos reutilizáveis pelas fases (em tiles). Os layouts COMBO ficam em
// cantos opostos e NÃO se sobrepõem, então lava e lama podem coexistir.

#include "levels/levels.h"

const TileRect LAVA_PAIR[LAYOUT_RECTS]  = { {  5,  6, 10, 6 }, { 25, 15, 10, 6 } };
const TileRect MUD_PAIR[LAYOUT_RECTS]   = { {  6, 15, 10, 6 }, { 24,  5, 10, 6 } };
const TileRect COMBO_LAVA[LAYOUT_RECTS] = { {  4,  4,  8, 5 }, { 28, 18,  8, 5 } }; // UL+LR
const TileRect COMBO_MUD[LAYOUT_RECTS]  = { { 28,  4,  8, 5 }, {  4, 18,  8, 5 } }; // UR+LL
