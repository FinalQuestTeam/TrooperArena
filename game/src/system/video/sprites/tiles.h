#ifndef TILES_H
#define TILES_H

// Mapa dos tiles procedurais na VRAM (a partir de TILE_USER_INDEX). É o "índice
// de assets" do jogo: cada bloco é gerado por system/video/tilegen.c e referenciado
// pelos módulos que desenham (sprites, HUD, elementos, itens, inimigos).
// Mantido separado de core/game.h por ser um concern de vídeo/gráficos.

#define TILE_PLAYER     0   // 4 tiles (16x16, ordem coluna)
#define TILE_ENEMY      4   // 4 tiles (vermelho: atirador)
#define TILE_ENEMY2     8   // 4 tiles (amarelo: perseguidor)
#define TILE_PSHOT      12  // 1 tile (8x8)
#define TILE_ESHOT      13  // 1 tile
#define TILE_WALL       14  // 1 tile
#define TILE_ENEMY3     15  // 16 tiles (32x32, roxo: tanque perseguidor)
#define TILE_LAVA       31  // 1 tile
#define TILE_MUD        32  // 1 tile
#define TILE_HEART      33  // 4 tiles (16x16, item de vida)
#define TILE_BOMB       37  // 4 tiles (16x16, item de carga de bomba)
#define TILE_SKULL      41  // 1 tile (8x8, caveira do contador de mortes)
#define TILE_BOMB_ICON  42  // 1 tile (8x8, bomba do HUD)
#define TILE_ICON_RED   43  // 1 tile (8x8, ícone da cor vermelha)
#define TILE_ICON_YEL   44  // 1 tile (8x8, ícone da cor amarela)
#define TILE_ICON_PUR   45  // 1 tile (8x8, ícone da cor roxa)
// tiles dos inimigos — gerados proceduralmente por forma+cor (tilegen.c)
#define TILE_SQ_ORG     46  // 4 tiles (16x16, quadrado laranja)
#define TILE_TRI_RED    50  // 4 tiles (16x16)
#define TILE_TRI_YEL    54  // 4
#define TILE_TRI_PUR    58  // 4
#define TILE_TRI_ORG    62  // 4
#define TILE_PEN_RED    66  // 9 tiles (24x24)
#define TILE_PEN_YEL    75  // 9
#define TILE_PEN_PUR    84  // 16 tiles (32x32)
#define TILE_PEN_ORG    100 // 9
#define TILE_ICON_ORG   109 // 1 tile (8x8, ícone da cor laranja)
#define TILE_BULLET     110 // 4 tiles (16x16, item de disparos)
#define TILE_DUMMY      114 // 9 tiles (24x24, inimigo dummy de teste)
#define TILE_BOOT       123 // 4 tiles (16x16, item de velocidade)
#define TILE_SHIELD     127 // 4 tiles (16x16, item de invencibilidade)
#define TILE_CHAIN      131 // 4 tiles (16x16, item raio)
#define TILE_BOLT       135 // 1 tile (8x8, indicador de raio ativo no HUD)
#define TILE_DOOR       136 // 1 tile (8x8, painel da porta entre as fases teste)
#define TILE_SPARK      137 // 1 tile (8x8, nó do raio em cadeia)
#define TILE_SHADOW     138 // 4 tiles (16x16, sombra pequena sob o jogador)
#define TILE_ICE        142 // 4 tiles (16x16, item de gelo)
#define TILE_FIRE       146 // 4 tiles (16x16, item de fogo)
#define TILE_FROST      150 // 16 tiles (bloco 4x4 idêntico: overlay ciano de "congelado")
#define TILE_ICON_SHIELD 166 // 1 tile (8x8, ícone do escudo no inventário)
#define TILE_ICON_ICE   167 // 1 tile (8x8, ícone do gelo no inventário)
#define TILE_ICON_FIRE  168 // 1 tile (8x8, ícone do fogo no inventário)
#define TILE_BLOOD      169 // 1 tile (8x8, quarto de poça; espelhado = poça 16x16)
#define NUM_TILES       170

// Região das rotações dos inimigos: cada tipo tem 8 direções (como o mech),
// geradas no boot por system/video/enemygfx.c, logo após os tiles base.
#define ENEMY_ROT_BASE  NUM_TILES
#define ENEMY_ROT_COUNT 632 // pools de slots por tamanho (5×32 + 3×72 + 2×128):
                            // cada tipo gerado 1×, reaproveitado, e o slot de um
                            // tipo "morto" é reciclado (enemygfx.c). Boot instantâneo.

// região dedicada do CHEFE (48x48): 8 direções × (48/8)² = 8 × 36 = 288 tiles.
// Grande demais para os pools; fica logo após a região dos inimigos.
#define BOSS_ROT_BASE   (ENEMY_ROT_BASE + ENEMY_ROT_COUNT)
#define BOSS_ROT_COUNT  288
#define BOSS_DIR_TILES  36  // (48/8)² tiles por direção

#endif // TILES_H
