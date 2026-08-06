# RedRex — Game Design Document (GDD)

> **Plataforma:** Sega Mega Drive / Genesis · **Engine:** SGDK 2.11 (C, m68k-elf)
> **Gênero:** shooter de arena top-down, sobrevivência por fases
> **Documento gerado a partir do código-fonte** (`game/src/`) — os valores refletem a implementação real.
> Tempos convertidos assumindo **60 fps (NTSC)**; em PAL (`fps` = 50) as cadências se ajustam automaticamente.

---

## Sumário

1. [Visão geral, controles, fluxo e HUD](#1-visão-geral-controles-fluxo-e-hud)
2. [Jogador, tiros e bomba](#2-jogador-tiros-e-bomba)
3. [Inimigos](#3-inimigos)
4. [Itens](#4-itens)
5. [Fases e elementos de terreno](#5-fases-e-elementos-de-terreno)
6. [Áudio e gráficos (técnico)](#6-áudio-e-gráficos-técnico)

---

# 1. Visão geral, controles, fluxo e HUD

## Visão geral

**RedRex** é um jogo de arena *top-down* de sobrevivência/shooter para Sega Mega Drive, desenvolvido em C com o SGDK. O jogador controla um personagem em uma arena vista de cima, atirando e usando bombas para eliminar inimigos que surgem em ondas. O objetivo de cada fase é **matar N inimigos** (a meta `killTarget` definida por fase); ao atingir a meta, a fase é concluída e avança-se para a próxima.

A arena ocupa a **tela inteira**. A primeira linha de tiles (linha 0) é reservada para o HUD, e a arena começa na linha 1.

| Dimensão | Valor (tiles) | Valor (pixels) |
|---|---|---|
| Largura da arena (`ARENA_TILES_W`) | 40 | 320 |
| Altura da arena (`ARENA_TILES_H`) | 27 | 216 |
| Origem da arena (`ARENA_TILE_X`, `ARENA_TILE_Y`) | (0, 1) | (0, 8) |

Os limites internos de movimento (dentro da borda de parede) são, em pixels:

| Limite | Constante | Valor (px) |
|---|---|---|
| X mínimo | `INNER_MIN_X` | 8 |
| Y mínimo | `INNER_MIN_Y` | 16 |
| X máximo | `INNER_MAX_X` | 311 |
| Y máximo | `INNER_MAX_Y` | 215 |

> Observação: o Mega Drive roda a arena em resolução 320x216 px (H40). A tela-título usa outro modo (256 px de largura), conforme detalhado abaixo.

## Controles

O jogo lê apenas o Joypad 1 (`JOY_1`). O mapeamento de botões é:

| Botão | Ação |
|---|---|
| D-pad | Move o jogador (nas 8 direções, dentro dos limites da arena) |
| A | Atira / Confirma o início da fase na tela de preparação |
| B | Usa a bomba (elimina inimigos em área; as mortes contam para a meta) |
| C | **(Debug)** Pula para a próxima fase |
| START | Pausa / Continua o jogo e confirma o Game Over (recomeça a fase) |

Notas:
- Vários comandos usam **detecção de borda** (só disparam no instante em que o botão é pressionado), comparando o estado atual do joypad com o anterior (`prevJoy`).
- O botão **C** faz `startPhase((currentLevel % LEVEL_COUNT) + 1)`, ou seja, avança ciclicamente entre as fases (volta à fase 1 após a última).

## Fluxo e estados do jogo

O laço principal opera como uma máquina de estados com cinco estados (a tela-título roda *antes* do laço, em `title.c`):

| Estado | Descrição |
|---|---|
| `ST_PREP` | Tela de preparação: mostra "FASE n", as informações/condições da fase e "APERTE A PARA COMECAR". Aguarda o botão A. |
| `ST_PLAY` | Jogo ativo. Inclui a contagem regressiva congelada de início de fase. |
| `ST_PAUSE` | Pausa: mostra "PAUSA - FASE n", as infos da fase e "START PARA CONTINUAR". |
| `ST_OVER` | Game Over: aguarda START para recomeçar a fase atual. |
| `ST_DONE` | Fase concluída: exibe "FASE CONCLUIDA!" por 3 s (cena congelada) antes da próxima tela de preparação. |

### Fluxo geral

1. **Tela-título** (cubo 3D) → aperte **START** para começar.
2. **Preparação** (`ST_PREP`): mostra a fase e as condições → aperte **A** para começar.
3. **Contagem "PREPARE-SE"** (congelamento de 3 s): a fase carrega, mas o jogo fica congelado exibindo "PREPARE-SE 3", "PREPARE-SE 2", "PREPARE-SE 1" no topo. O jogador só assume o controle após a contagem.
4. **Jogo ativo** (`ST_PLAY`): o jogador atira, usa bombas e elimina inimigos.
5. A partir daí, dois desfechos:
   - **Fase concluída** (`ST_DONE`): ao atingir a meta de mortes, "FASE CONCLUIDA!" aparece por **3 s** e o jogo vai para a **Preparação da próxima fase** (ciclando com `(currentLevel % LEVEL_COUNT) + 1`).
   - **Game Over** (`ST_OVER`): se o jogador morre, aguarda **START** para **recomeçar a mesma fase**.

### Temporizações (a 60 fps NTSC; `fps` = 60 NTSC / 50 PAL)

| Evento | Valor no código | Segundos (NTSC) |
|---|---|---|
| Congelamento de início ("PREPARE-SE") | `fps * 3` | 3 s |
| Primeiro inimigo da fase (`FIRST_SPAWN_S`) | `FIRST_SPAWN_S * fps` = `1 * fps` | 1 s após o congelamento acabar |
| Mensagem "FASE CONCLUIDA!" (`doneTimer`) | `fps * 3` | 3 s |
| Cadência de tiro inimigo (`ENEMY_CD`) | `fps * 2` | 2 s |
| Intervalo entre ondas de spawn | `def->spawnSeconds * fps` | `spawnSeconds` s (por fase) |

> Detalhe importante: `spawnTimer` é inicializado em `FIRST_SPAWN_S * fps` (1 s), mas essa contagem só é decrementada durante o jogo ativo (após o congelamento de 3 s). Assim, o primeiro inimigo surge cerca de **1 s depois** do fim da contagem "PREPARE-SE". As ondas seguintes obedecem ao `spawnSeconds` da fase, e cada onda pode gerar `spawnCount` inimigos (mínimo 1).

### Persistência de vida e bombas

- **Entre fases**: a **vida (HP) e as bombas persistem** — `startPhase()` chama `PLAYER_reset()` (posiciona o jogador) mas **não** zera o HP nem recarrega as bombas.
- **No Game Over**: ao apertar START em `ST_OVER`, o jogo chama `PLAYER_fullHp()` e `BOMB_reset()`, **restaurando vida e bombas ao valor inicial** antes de reexibir a preparação da fase.
- A cura é limitada ao **teto de 100 de HP** (`healPlayer` respeita o limite de 100).

## Tela-título

Antes do laço de jogo, `TITLE_run()` roda uma tela de abertura própria, usando o **modo bitmap** do SGDK (`BMP_init`) em largura de **256 px** (`VDP_setScreenWidth256()`), com a API 3D `maths3D` do SGDK:

- Um **cubo 3D wireframe azul** girando continuamente. O cubo tem 8 vértices e 12 arestas, desenhado com `BMP_drawLine` sobre linhas projetadas em 2D.
  - Rotação por quadro: `+1.2` em X e `+1.8` em Y (graus), com *wrap* em 360°.
  - Câmera a distância `FIX16(15)`, cubo transladado em Z para `FIX16(22)`, iluminação desligada.
  - Cor 14 = azul do cubo (`0x3060F0`); cor 15 = branco dos textos (`0xF0F0F0`).
- Texto **"R E D R E X"** fixo no topo e **"APERTE START"** piscando a cada ~0,5 s (alternado por `(frame >> 5) & 1`, ou seja, liga/desliga a cada 32 quadros).
- Aperte **START** para sair da tela-título e entrar no jogo. Ao sair, o modo bitmap é encerrado (`BMP_end`) e o VDP volta para 320 px, com os planos limpos.

> **A música NÃO toca na tela-título.** O modo bitmap faz DMA pesado que interrompe o DAC (canal PCM). A música de fundo (intro + loop) só é inicializada e iniciada (`MUSIC_init()` / `MUSIC_start()`) depois da tela-título, já no modo normal do VDP, ao entrar no jogo.

## HUD

O HUD ocupa a **linha 0** (topo da tela), acima da arena. É desenhado/atualizado por funções dedicadas (`HUD_drawKills`, `HUD_drawLife`, `HUD_drawBombs`) e contém:

| Elemento | Conteúdo | Fonte |
|---|---|---|
| Ícone caveira + contador de MORTES | Caveira (tile `TILE_SKULL`) seguida do progresso `kills / killTarget` (mortes atuais e meta da fase) | `HUD_drawKills(kills, killTarget)` |
| FASE n | Número da fase atual | HUD |
| Ícone bomba + cargas | Bomba (tile `TILE_BOMB_ICON`) seguida do número de cargas disponíveis (`BOMB_count()`) | `HUD_drawBombs(BOMB_count())` |
| HP | Vida atual do jogador (teto de 100) | `HUD_drawLife(hp)` |

Os tiles de ícone do HUD são: caveira (`TILE_SKULL` = 41) e bomba (`TILE_BOMB_ICON` = 42), ambos 8x8. Existem ainda ícones 8x8 dos tipos de inimigo (`TILE_ICON_RED` = 43, `TILE_ICON_YEL` = 44, `TILE_ICON_PUR` = 45) definidos em `game.h`.

> A contagem de mortes é atualizada tanto pelos tiros do jogador quanto pelas mortes causadas pela bomba (ambos incrementam `kills` e redesenham o contador via `HUD_drawKills`).


---

# 2. Jogador, tiros e bomba

## O jogador

O jogador é representado por um **círculo ciano de 16x16 pixels**. Ele se move livremente nas **4 direções cardinais**, com **diagonais** permitidas (movimento simultâneo em X e Y). O deslocamento é calculado em **fixed-point 26.6** (a posição interna `pfx`/`pfy` é a posição em pixels deslocada 6 bits à esquerda), o que garante suavidade e permite frações de pixel por frame.

### Movimento

- **Velocidade base:** `PLAYER_SPEED = 2` px/frame. Internamente vira `speedFp = 2 << 6 = 128` unidades 26.6 por frame.
- **Diagonais:** ao pressionar duas direções perpendiculares, aplica-se a mesma velocidade em cada eixo (sem normalização), resultando em movimento diagonal mais rápido (~2,83 px/frame).
- **Contido pela arena:** a posição é limitada (clamp) entre `INNER_MIN_X`/`INNER_MIN_Y` e `INNER_MAX_X - 15`/`INNER_MAX_Y - 15`, mantendo os 16 px do sprite dentro da área interna jogável.
- **Lentidão da lama (`playerSlowPct`):** quando há um percentual de lentidão ativo, a velocidade é reduzida proporcionalmente:

  `speedFp = speedFp * (100 - playerSlowPct) / 100`

  Ou seja, `playerSlowPct` é uma redução percentual direta sobre a velocidade daquele frame. Com `playerSlowPct = 0` não há penalidade; com valores maiores o jogador fica mais lento sobre a lama.

### Vida

- **Vida inicial:** `START_HP = 100` (definida por `PLAYER_fullHp`, chamada no início do jogo e após o game over).
- **Teto de vida:** 100 (a cura nunca ultrapassa `START_HP`).
- A vida (`hp`) é do tipo `s16`; quando chega a `<= 0` o jogo entra em **game over** (`gameOver = TRUE`, exibe "GAME OVER" e "APERTE START").

### Dano e invencibilidade

Ao levar dano, o jogador fica **invencível por ~0,3 s piscando**. O temporizador de invencibilidade é:

`invTimer = (fps * 3) / 10`

A 60 fps (NTSC): `invTimer = 180 / 10 = 18 frames = 0,30 s`.

Durante esse período o sprite pisca: `PLAYER_hidden()` retorna verdadeiro quando `(invTimer >> 2) & 1`, ou seja, o sprite **some e aparece a cada 4 frames** (alterna a cada ~0,067 s).

Existem duas rotinas distintas de perda de vida:

| Função | Respeita invencibilidade? | Uso típico |
|---|---|---|
| `damagePlayer(dmg)` | **Sim** — se `invTimer` estiver ativo, o dano é ignorado por completo | Dano de tiros inimigos e contato com inimigos |
| `loseHp(dmg)` | **Não** — subtrai `hp` diretamente, sempre | Dano de ambiente contínuo (ex.: lava) |

- `damagePlayer`: se não estiver invencível, chama `loseHp` e, se não for game over, arma o `invTimer` (0,3 s).
- `loseHp`: subtrai o dano, redesenha a vida no HUD e checa o game over. Ignora a invencibilidade de propósito, para que perigos de ambiente (lava) drenem a vida mesmo logo após um dano.
- `healPlayer(amount)`: soma vida com **teto em 100** (`if (hp > START_HP) hp = START_HP`) e atualiza o HUD.

### Direção de tiro

A direção do tiro (`faceX`, `faceY`) segue a **última direção cardinal pressionada**:

- Movendo só na horizontal: `faceX = ±1`, `faceY = 0`.
- Movendo só na vertical: `faceX = 0`, `faceY = ±1`.
- Movendo na diagonal: `faceX` e `faceY` recebem ambos os sentidos (tiro diagonal).

Quando nenhum botão direcional está pressionado, a direção anterior é mantida. O estado inicial (`PLAYER_reset`) é `faceX = 1`, `faceY = 0` (mirando à direita).

## Tiros do jogador

O disparo é feito com o **botão A** (o botão B é reservado para a bomba). Cada disparo respeita um **cooldown**:

`shotCooldown = (fps * 4) / 5`

A 60 fps: `shotCooldown = 240 / 5 = 48 frames = 0,80 s`, resultando em uma **cadência de 1,25 tiros/s**.

Características do projétil do jogador:

| Propriedade | Valor |
|---|---|
| Tamanho | 8x8 px |
| Velocidade (`PSHOT_SPEED`) | 3 px/frame (inteiro, sem fixed-point) |
| Origem | centro do jogador (`px + 4`, `py + 4`) |
| Direção | `faceX * 3`, `faceY * 3` (segue a mira atual) |
| Máx. simultâneos (`MAX_PSHOTS`) | 6 |

Regras de funcionamento:

- `SHOTS_firePlayer` procura o primeiro slot livre em `pshots[]`; se todos os 6 estiverem ativos, o tiro não sai (limita a cadência de fogo além do cooldown). Ao disparar, toca `SFX_playerShot`.
- Cada frame, os tiros avançam por `vx`/`vy`. Se saírem da arena (`INNER_MIN_X`/`INNER_MIN_Y` a `INNER_MAX_X - 7`/`INNER_MAX_Y - 7`), são desativados.
- **Colisão com inimigos:** testa `ENEMIES_damageBox(x, y, 8, 8)` — uma caixa AABB de 8x8. Em qualquer acerto (`hit != ENEMY_HIT_NONE`) o tiro é **consumido** (desativado), mesmo que o inimigo não morra.
- Se o acerto resultar em morte (`ENEMY_HIT_KILLED`), o contador de mortes do frame é incrementado e `SHOTS_updatePlayer` devolve esse total. **Inimigo morto por tiro conta para a meta** da fase.

## Tiros inimigos (mecânica de colisão contra o jogador)

Os projéteis inimigos (`eshots[]`) usam **fixed-point 26.6** para posição e velocidade (struct `FShot` com campos `s32`), permitindo direções normalizadas com precisão.

- **Mira no jogador:** ao disparar (`fireEnemyShot(ex, ey)`), calcula-se o vetor do centro do inimigo até o **centro do jogador** (`px + 8`, `py + 8`), obtém-se a distância aproximada (`getApproximatedDistance`) e o vetor é normalizado e escalado por `ESHOT_SPEED = 2` px/frame:

  `vx = dx * 2 << 6 / dist` e `vy = dy * 2 << 6 / dist`

  Assim o tiro segue em linha reta em direção à posição do jogador no instante do disparo. Toca `SFX_enemyShot`.
- **Máx. simultâneos (`MAX_ESHOTS`):** 16.
- **Saída da arena:** convertidos de volta para pixels (`>> 6`); saindo dos limites internos (`INNER_MIN_*` a `INNER_MAX_* - 7`), são desativados.
- **Colisão pelo círculo do jogador:** usa distância ao quadrado entre o centro do tiro (`sx + 4`, `sy + 4`) e o centro do jogador (`px + 8`, `py + 8`):

  `if (dx*dx + dy*dy <= 100)` → acerto.

  O limiar 100 corresponde a um raio de colisão de **10 px** (raio 7 do jogador + raio 3 do tiro). No acerto, o tiro é desativado e aplica-se `damagePlayer(HIT_DAMAGE)`.
- **Dano (`HIT_DAMAGE`):** 10 de vida por acerto (bloqueado se o jogador estiver invencível, pois passa por `damagePlayer`).

## Bomba

O jogador começa cada partida com **`BOMB_MAX = 2` cargas** de bomba.

- **Persistência:** as cargas **persistem entre as fases**, sempre limitadas ao teto de 2 (`BOMB_MAX`).
- **Reset:** `BOMB_reset` devolve as cargas ao máximo no início do jogo e no game over (não redesenha o HUD por conta própria).
- **Reposição:** o item de bomba chama `BOMB_add`, que soma +1 respeitando o teto de 2 e atualiza o HUD (`HUD_drawBombs`).

**Uso (botão B) — `BOMB_use`:**

| Condição | Efeito |
|---|---|
| Há pelo menos 1 carga | Consome 1 carga, atualiza o HUD, toca o som de explosão (`SFX_bombBlast`), dispara o flash de tela (`GFX_flash`) e mata **todos** os inimigos da tela (`ENEMIES_killAll`) |
| Sem carga (`bombs == 0`) | Nada acontece (retorna 0 imediatamente) |

Detalhes importantes:

- A bomba mata todos os inimigos **sem drop de itens** (`ENEMIES_killAll` não sorteia coletáveis, ao contrário da morte por tiro).
- As mortes causadas pela bomba **contam para a meta** da fase (`BOMB_use` devolve o número de inimigos mortos, repassado ao contador).
- `BOMB_count` expõe a quantidade atual de cargas, usada para (re)desenhar o HUD ao entrar/redesenhar a fase.


---

# 3. Inimigos

## Inimigos

Os inimigos são o principal desafio da arena. O jogo mantém até `MAX_ENEMIES` (**10**) inimigos ativos simultaneamente. Eles não existem desde o início: são criados por *spawn* ao longo da partida (`ENEMIES_trySpawn`), conforme o conjunto de tipos permitidos na fase corrente, e sempre a uma **distância mínima do jogador** (~56 px) para nunca surgirem em cima dele.

Cada inimigo é descrito por uma estrutura `Enemy` que carrega:

- **tipo** (`ENEMY_RED`, `ENEMY_YELLOW`, `ENEMY_PURPLE`);
- **vida** (`hp`), medida em número de tiros necessários para matá-lo;
- **tamanho** (lado do quadrado em px, dado por `ENEMY_SIZE`): 16x16 para vermelho e amarelo, 32x32 para o roxo;
- posição em ponto-fixo 26.6 (`fx`/`fy`) e velocidade por frame (`vx`/`vy`);
- temporizadores de tiro (`fireTimer`), de recálculo de rota (`retarget`) e de recuo pós-colisão (`stun`);
- lentidão atual por elementos da fase (`slowPct`, em %).

Todos os inimigos são quadrados coloridos. A cor identifica o comportamento.

| Tipo (cor) | Tamanho (px) | Vida (tiros p/ morrer) | Comportamento | Atira? | Dano de contato | Velocidade de perseguição |
|---|---|---|---|---|---|---|
| **Vermelho** (`ENEMY_RED`) | 16x16 | 1 | Torre fixa (não se move) | Sim — mirado no jogador a cada 2 s | 10 (`ENEMY_DMG_RED`) | — (imóvel) |
| **Amarelo** (`ENEMY_YELLOW`) | 16x16 | 1 | Persegue o jogador | Não | 5 (`ENEMY_DMG_YELLOW`) | `ENEMY_SPEED_FP` = 40/64 ≈ 0,63 px/frame (~37,5 px/s a 60 fps) |
| **Roxo** (`ENEMY_PURPLE`) | 32x32 | 5 | Persegue devagar (tanque) | Não | 15 (`ENEMY_DMG_PURPLE`) | `ENEMY_SPEED_FP`/2 = 20/64 ≈ 0,31 px/frame (~18,75 px/s a 60 fps) |

> Conversões assumem NTSC (60 fps). Em PAL (`fps` = 50) os tempos em segundos derivados de `fps` mudam proporcionalmente; as velocidades por *frame* são as mesmas, mas o deslocamento por segundo cai.

### Vermelho (ENEMY_RED)

Quadrado **vermelho 16x16**, com **1 de vida** (morre com um único tiro). É uma **torre FIXA**: não persegue nem se desloca — seu `update` (`enemy_red.c`) só cuida do tiro.

- **Atira mirado no jogador** a cada **2 segundos**. A cadência vem de `ENEMY_CD = fps*2` (120 frames a 60 fps). Quando `fireTimer` chega a zero, chama `fireEnemyShot(x, y)` (projétil apontado para a posição do jogador) e rearma o timer com `ENEMY_CD`.
- No *spawn*, o primeiro tiro é escalonado: `fireTimer = ENEMY_CD/2 + (random % fps)`, ou seja, entre **1,0 e ~2,0 s** — isso dessincroniza vários vermelhos e evita rajadas simultâneas.
- **Dano de contato**: `ENEMY_DMG_RED` = **10** (o maior por encostão entre os que ficam parados). Mesmo imóvel, ferir-se nele custa caro.
- **Não recua**: por estar fixo, o vermelho é o único que **não** sofre *knockback* ao colidir com o jogador (mantém a posição).

### Amarelo (ENEMY_YELLOW)

Quadrado **amarelo 16x16**, com **1 de vida**. É o **perseguidor** clássico: seu `update` (`enemy_yellow.c`) apenas chama `ENEMIES_chase(e, ENEMY_SPEED_FP)`.

- **Persegue o jogador** à velocidade `ENEMY_SPEED_FP` = **40 em 26.6 ≈ 0,63 px/frame** (~37,5 px/s a 60 fps), mais lento que o jogador — dá para escapar em linha reta, mas ele corta caminho.
- **Não atira**. Todo o perigo é o **dano de contato**: `ENEMY_DMG_YELLOW` = **5** (o menor). Individualmente é fraco, mas em número incomoda por cercar o jogador.

### Roxo (ENEMY_PURPLE)

Quadrado **roxo 32x32** (o **dobro** do lado dos demais), com **5 de vida** — o **tanque**: exige **5 tiros** para morrer. Seu `update` (`enemy_purple.c`) chama `ENEMIES_chase(e, ENEMY_SPEED_FP / 2)`.

- **Persegue à METADE da velocidade do amarelo**: `ENEMY_SPEED_FP/2` = **20 em 26.6 ≈ 0,31 px/frame** (~18,75 px/s a 60 fps). É lento e telegrafado, mas absorve muito dano.
- **Não atira**. O perigo é o **dano de contato**, o mais alto do jogo: `ENEMY_DMG_PURPLE` = **15**. Somado ao corpo grande (32x32), é difícil de desviar em espaços apertados.

## Mecânicas comuns

### Perseguição (`ENEMIES_chase`)

Rotina compartilhada por amarelo e roxo. A direção **não é recalculada todo frame**: a cada `RETARGET_FRAMES` = **16 frames (~0,27 s)** o inimigo re-mira o centro do jogador, normaliza o vetor pela distância aproximada e fixa `vx`/`vy` (em 26.6) proporcionais à `speedFp` recebida. Nos frames intermediários ele apenas **integra** essa velocidade em ponto-fixo (`fx`/`fy` += `vx`/`vy`), o que barateia o custo e dá um leve "atraso" ao movimento.

- A posição resultante é **contida na arena** (limites `INNER_MIN_*`/`INNER_MAX_*`, considerando o tamanho do quadrado), então inimigos nunca saem da área jogável.
- **Lentidão da lama** (`slowPct`): antes de mover, a velocidade efetiva é reduzida por `speedFp * (100 - slowPct) / 100`. Ao pisar em elementos lentificantes, o perseguidor desacelera na mesma proporção.

### Dano de contato + knockback

A cada frame, `ENEMIES_update` roda o comportamento do inimigo e depois testa colisão AABB entre o inimigo (tamanho por tipo) e o jogador (16x16). Ao **encostar no jogador**:

- aplica `damagePlayer(dmg)` com o dano do tipo (10 / 5 / 15), **respeitando a invencibilidade** do jogador (se estiver invencível, não sofre);
- se isso causar *game over*, o processamento para imediatamente.

Para evitar **dano contínuo** enquanto encostado, os **perseguidores RECUAM** após a colisão (o vermelho, fixo, **não** recua):

- entram em **stun** por `fps/3` = **20 frames (~0,33 s ≈ 1/3 de segundo)**, durante os quais só integram a velocidade de recuo (não re-miram);
- a **velocidade de recuo** é no sentido oposto ao jogador: **2x a de perseguição para o amarelo** (`ENEMY_SPEED_FP * 2` ≈ 1,25 px/frame) e **1x para o roxo** (`ENEMY_SPEED_FP` ≈ 0,63 px/frame — o tanque recua mais devagar);
- ao terminar o stun, `retarget` é zerado para que o inimigo **volte a mirar imediatamente**.

### Spawn (`ENEMIES_trySpawn`)

Recebe a lista de tipos permitidos da fase (`types`, `typeCount`) e tenta preencher **um** slot livre por chamada:

- sorteia o tipo entre os permitidos (`random % typeCount`);
- procura uma posição válida em até **12 tentativas**, exigindo distância aproximada **≥ 56 px** do jogador; se nenhuma servir neste frame, desiste e tenta no próximo período de spawn;
- ao posicionar, inicializa `hp` (**5** para roxo, **1** para os demais), zera velocidades/stun/lentidão e escalona o `fireTimer` inicial.

### Aplicar acerto de tiro (`ENEMIES_damageBox`)

Recebe uma caixa (x, y, w, h) e atinge o **primeiro** inimigo ativo em colisão:

- decrementa `hp`; se **sobrou vida**, retorna `ENEMY_HIT_DAMAGED`;
- se **zerou**, desativa o inimigo, tenta **dropar um item** onde ele morreu (`ITEMS_tryDrop`, no centro do inimigo) e retorna `ENEMY_HIT_KILLED`;
- se não acertou ninguém, retorna `ENEMY_HIT_NONE`.

### Matar todos / bomba (`ENEMIES_killAll`)

Desativa **todos** os inimigos ativos de uma vez (efeito da bomba) e retorna quantos morreram. Diferentemente do tiro, a bomba **não sorteia itens** — nenhum drop é gerado.

### Limpeza (`ENEMIES_clear`)

Zera o array inteiro de inimigos (usado ao reiniciar/estados de transição).


---

# 4. Itens

## Itens

Itens são recompensas que caem no cenário quando um inimigo é derrotado **por
tiro** do jogador. Inimigos eliminados pela bomba **não** derrubam item algum —
apenas mortes causadas por projétil disparam a rotina de drop (`ITEMS_tryDrop`).

Regras gerais do sistema de itens:

- **Limite no chão:** existem no máximo `MAX_ITEMS = 8` itens ativos
  simultaneamente. Cada slot livre é procurado ao gerar um novo drop; se todos
  os 8 slots estiverem ocupados, o drop é simplesmente perdido (nenhum item é
  criado).
- **Tempo de vida (TTL):** ao surgir, o item recebe `ttl = fps * 10`. A 60 fps
  (NTSC) isso equivale a **600 frames = 10 segundos** no chão.
- **Piscar antes de sumir:** nos **últimos 2 segundos** de vida
  (`ttl < fps * 2`, ou seja abaixo de 120 frames), o item começa a piscar para
  avisar que vai desaparecer. O piscar alterna aproximadamente a cada 4 frames
  (`(ttl >> 2) & 1`), fazendo o sprite ser omitido em metade dos ciclos.
- **Coleta:** o item é coletado automaticamente ao o jogador passar por cima
  dele. A detecção usa uma caixa de colisão (AABB) de **16x16** pixels do
  jogador sobreposta ao item de 16x16. Não é preciso nenhuma ação; basta encostar.
- **Posicionamento:** o item nasce centralizado na posição de morte do inimigo
  (`cx - 8`, `cy - 8`) e é fixado (clamp) dentro dos limites internos da arena
  (`INNER_MIN/MAX`) para nunca aparecer parcialmente fora da área jogável.

## Chance de drop por inimigo

Quando um inimigo morre por tiro, ocorre um **primeiro sorteio** que decide
apenas se **cai ou não** um item. A chance depende do **tipo do inimigo**,
conforme a tabela `dropChance[]`. Internamente é feito `random() % 100` e o
resultado é comparado com a chance do tipo: se o número sorteado for maior ou
igual à chance, nada cai.

| Tipo de inimigo | Constante      | Chance de derrubar item |
|-----------------|----------------|-------------------------|
| Vermelho        | `ENEMY_RED`    | 25%                     |
| Amarelo         | `ENEMY_YELLOW` | 30%                     |
| Roxo            | `ENEMY_PURPLE` | 75%                     |

Ou seja, inimigos mais fortes/raros (Roxo) recompensam com muito mais
frequência do que os básicos (Vermelho). Esse é apenas o sorteio de **"cai ou
não cai"** — o tipo de item ainda não foi decidido nesta etapa.

## Sorteio de qual item (peso)

Se o primeiro sorteio determinou que **vai cair** um item, um **segundo
sorteio ponderado** decide **qual** item aparece. Esse sorteio é feito pela
função `pickItemType()` usando os pesos da tabela `itemWeight[]` (a soma dos
pesos deve dar 100).

O funcionamento de `pickItemType()`: sorteia `r = random() % 100` (valor de 0 a
99) e percorre os tipos acumulando os pesos — se `r` for menor que o peso do
tipo atual, esse tipo é escolhido; caso contrário, subtrai o peso e passa ao
próximo. O último tipo funciona como fallback de segurança caso os pesos somem
menos de 100.

| Item             | Constante    | Peso (chance de ser o item escolhido) |
|------------------|--------------|----------------------------------------|
| Coração          | `ITEM_HEART` | 70%                                    |
| Carga de bomba   | `ITEM_BOMB`  | 30%                                    |

Importante: esse peso é **condicional** — só se aplica depois que o item já
"caiu". A chance final de um coração cair de um inimigo específico é o produto
das duas etapas (ex.: Roxo → 75% de cair × 70% de ser coração = 52,5%).

## Tipos de item

Cada tipo de item tem um ícone 16x16 desenhado **proceduralmente** em tempo de
execução (não é um asset importado, é gerado pixel a pixel em `buildTile`), e
usa a paleta `PAL1`.

### Coração (ITEM_HEART)

- **Ícone:** coração vermelho de 16x16, desenhado proceduralmente
  (`ITEMHEART_buildTile`) — dois lóbulos circulares no topo mais um corpo
  triangular embaixo, com um pequeno brilho (highlight) no lóbulo esquerdo.
- **Efeito:** cura **15 de vida** (`HEART_HEAL = 15`), aplicado via
  `healPlayer(HEART_HEAL)`. A cura é limitada ao **teto de 100** de vida — o
  excedente não é acumulado.

### Carga de bomba (ITEM_BOMB)

- **Ícone:** bomba redonda de 16x16, desenhada proceduralmente
  (`ITEMBOMB_buildTile`) — corpo circular escuro com brilho no canto superior
  esquerdo e um pavio com faísca laranja saindo do topo.
- **Efeito:** adiciona **+1 carga de bomba** (`BOMB_add()`), respeitando o
  **teto de 2 cargas**. Se o jogador já estiver com o número máximo de cargas,
  o item é **consumido mesmo assim, sem efeito** (a coleta acontece, mas a
  carga extra é descartada).

## Coleta

A coleta é resolvida em `ITEMS_update()`, executado a cada frame para todos os
itens ativos:

1. **Decremento do TTL:** cada frame decrementa `ttl`. Quando chega a **0**, o
   item é desativado (`active = FALSE`) e some do chão sem produzir efeito.
2. **Piscar:** enquanto `ttl < fps * 2` (últimos **2 segundos** / 120 frames), o
   sprite pisca (alternando a cada ~4 frames) para sinalizar que está prestes a
   desaparecer.
3. **Detecção de coleta:** testa a sobreposição AABB entre a caixa 16x16 do
   jogador e o item 16x16. Havendo colisão, aplica o efeito do item conforme o
   tipo (`apply()` → `ITEMHEART_apply` ou `ITEMBOMB_apply`), toca o som de
   coleta e desativa o item.

**Som de coleta:** ao coletar, toca `sfxItemPickup()` — um "blip" ascendente
gerado no chip PSG, dando o feedback sonoro clássico de item pego.

---
Resumo: seção de Itens do GDD escrita cobrindo drop por tiro, chances por inimigo (25/30/75%), sorteio ponderado do item (70% coração / 30% bomba), efeitos (cura 15/teto 100; +1 bomba/teto 2), TTL de 10s com piscar nos últimos 2s e coleta por AABB 16x16, com frames convertidos a 60 fps.


---

# 5. Fases e elementos de terreno

## Fases

O jogo é organizado em `LEVEL_COUNT` = **4 fases**, definidas cada uma em seu próprio arquivo (`level1.c` a `level4.c`) e registradas no dispatcher `levels.c`. Ao completar a última, o ciclo recomeça na Fase 1 (as fases são **cicladas**).

Toda fase é descrita por uma única estrutura `LevelDef` (`levels.h`), que reúne os elementos de terreno, as condições de vitória e as regras de aparição de inimigos:

- **Elementos de terreno** — ponteiros e contagens para os retângulos de terreno especial:
  - `lava` / `lavaCount`: lista de retângulos de lava (`NULL` se a fase não tiver lava).
  - `mud` / `mudCount`: lista de poças de lama (`NULL` se a fase não tiver lama).
  Cada retângulo é um `TileRect { x, y, w, h }` expresso em **tiles de 8x8 px** (`elements.h`).
- **Condição de vitória**:
  - `killTarget`: número de inimigos que o jogador precisa eliminar para completar a fase.
- **Regras de aparição (spawn)**:
  - `spawnSeconds`: intervalo entre aparições de inimigos, em segundos.
  - `spawnCount`: quantos inimigos surgem por aparição (`0` ou `1` = aparição simples; `2` = duplas; etc.).
  - `enemyTypes` / `enemyTypeCount`: os tipos de inimigo permitidos na fase (Vermelho, Amarelo, Roxo).

O dispatcher `LEVEL_current()` resolve a fase ativa a partir de `currentLevel` (1..4). `LEVEL_draw()` redesenha a arena e seus elementos (usado ao retomar do pause), `LEVEL_load()` desenha e ainda zera o estado dos elementos (começar/recomeçar) e `LEVEL_update()` roda a cada frame a lógica dos elementos daquela fase.

### Tabela das fases

| Fase | Meta (matar N) | Intervalo de spawn (s) | Inimigos por spawn | Tipos de inimigo | Elemento de terreno |
|------|----------------|------------------------|--------------------|------------------|---------------------|
| 1 | 10 | 5 | 1 (simples) | Vermelho, Amarelo | Nenhum (arena vazia) |
| 2 | 20 | 3 | 1 (simples) | Vermelho, Amarelo | **Lava** — 2 blocos |
| 3 | 25 | 5 | 1 (simples) | Vermelho, Amarelo, Roxo | **Lama** — 2 poças |
| 4 | 20 | 5 | **2 (duplas)** | Vermelho, Amarelo | Nenhum (arena vazia) |

Observações por fase (valores reais dos arquivos `levelN.c`):

- **Fase 1** (`level1.c`): arena vazia, sem elementos de terreno. Progressão de dificuldade mais suave (meta baixa, spawn lento).
- **Fase 2** (`level2.c`): introduz **lava** com 2 blocos, em tiles `{ 5, 7, 10x6 }` (bloco superior esquerdo) e `{ 25, 17, 10x6 }` (bloco inferior direito). É a fase de spawn mais rápido (3 s) e meta maior que a Fase 1.
- **Fase 3** (`level3.c`): introduz **lama** com 2 poças, em tiles `{ 7, 6, 9x6 }` (poça superior esquerda) e `{ 24, 16, 10x7 }` (poça inferior direita). É a única fase que libera o inimigo **Roxo** e tem a maior meta (25).
- **Fase 4** (`level4.c`): mesma arena vazia da Fase 1, mas os inimigos surgem em **duplas** (`spawnCount = 2`), aumentando a pressão sem mudar o cenário.

### Exibição dos inimigos nas telas de preparação/pause

Nas telas de informação da fase (`LEVEL_drawInfo`, usada na preparação e no pause) os tipos de inimigo **não são escritos como texto**: cada tipo é mostrado como um **quadradinho colorido (ícone)** desenhado diretamente no tilemap, à direita do rótulo "INIMIGOS:", espaçados de 2 colunas. O mapeamento é: Vermelho → `TILE_ICON_RED`, Amarelo → `TILE_ICON_YEL`, Roxo → `TILE_ICON_PUR`. As demais linhas dessa tela mostram a meta de mortes e o intervalo de spawn em texto.

## Elementos de terreno

Os elementos de terreno vivem em `src/elements/` e cada um é auto-contido (padrão de tile procedural, desenho, detecção de "estar em cima" e atualização por frame). A fase apenas lista, no `LevelDef`, quais retângulos de cada elemento existem — a lógica é compartilhada entre todas as fases que usarem aquele elemento.

### Lava

Área de **2 retângulos vermelhos** (padrão vermelho com respingos laranja, gerado proceduralmente em `LAVA_buildTile`). Aplica **dano progressivo** enquanto o **centro do jogador** (jogador de 16x16, testado em `px+8`, `py+8`) estiver sobre qualquer um dos retângulos:

- O dano começa em `LAVA_DPS_MIN` = **5 por segundo** e sobe `LAVA_DPS_STEP` = **5** a cada segundo pisando na lava, com teto em `LAVA_DPS_MAX` = **25 por segundo**. Ou seja: 5/s no 1º segundo, 10/s no 2º, 15/s no 3º, 20/s no 4º e 25/s do 5º segundo em diante.
- A taxa é implementada aplicando 1 de HP por "tick", com o intervalo do tick = `fps / dps` frames (a 60 fps, quanto maior o dps, mais frequente o dano).
- O dano **ignora a invencibilidade** do jogador — é considerado dano de ambiente, não de inimigo.
- Ao **sair** da lava, a progressão é **zerada** (`LAVA_reset`): o dano volta a começar em 5/s na próxima vez.
- Enquanto o jogador está na lava, toca o **som ambiente de fogo/queimando** (`SFX_AMB_LAVA`).

### Lama

Área de **2 poças marrons** (padrão marrom com torrões mais escuros, gerado em `MUD_buildTile`). Reduz a velocidade de quem pisa nela, de forma **progressiva**, tanto para o jogador quanto para os inimigos. Cada entidade acumula **seu próprio tempo** na lama (o jogador tem `playerMudTime`; cada inimigo tem seu `mudTime`), então a lentidão é individual.

- **Jogador**: a lentidão cresce `MUD_PLAYER_STEP` = **10% por segundo** pisando, até o teto `MUD_PLAYER_MAX` = **70%**. (A fórmula `slowFromTime` aplica um degrau imediato de STEP% ao entrar e soma +STEP% a cada segundo até o teto.)
- **Inimigos**: a lentidão cresce `MUD_ENEMY_STEP` = **3% por segundo** pisando, até o teto `MUD_ENEMY_MAX` = **30%**. O centro do inimigo é calculado conforme o seu tamanho (`ENEMY_SIZE(e) / 2`).
- Ao **sair** da lama, o tempo acumulado e a lentidão daquela entidade são **restaurados** a zero (o jogador volta à velocidade cheia; cada inimigo idem).
- Enquanto o jogador está na lama, toca o **som ambiente de passos** (`SFX_AMB_MUD`).

> Nota de precisão: o cabeçalho de comentário de `mud.c` cita "-5%/-65%" para o jogador, mas os valores reais vigentes são os definidos em `mud.h` — **-10% por segundo, teto -70%** (comentário desatualizado no `.c`).

### Arquitetura modular de fases e elementos

O acoplamento entre fases e elementos é intencionalmente **modular e orientado a dados**:

- Uma **fase é só dados** (`LevelDef`): ela declara quais retângulos de lava/lama existem e as regras de spawn/vitória, sem escrever nenhuma lógica.
- Um **elemento é código reutilizável** (`draw` / `update` / `reset` + tile procedural) que opera sobre a lista de `TileRect` que a fase fornecer.
- O dispatcher (`LEVEL_draw`/`LEVEL_update`/`LEVEL_reset`) apenas percorre os elementos presentes na fase atual (`if (def->lava) ...`, `if (def->mud) ...`), então **adicionar uma nova fase** é criar um `levelN.c` com sua `LevelDef` e registrá-lo no vetor `levels[]`; **adicionar um novo elemento de terreno** é criar seu par de arquivos em `elements/`, acrescentar seu ponteiro/contagem ao `LevelDef` e um par de linhas no dispatcher. Nenhuma fase existente precisa ser tocada.


---

# 6. Áudio e gráficos (técnico)

## Áudio

O áudio do RedRex é dividido em dois domínios de hardware totalmente independentes do Sega Mega Drive: todos os **efeitos sonoros** e o **som ambiente** são gerados diretamente no chip PSG (SN76489), enquanto a **música de fundo** toca no DAC do YM2612 via driver PCM. Como usam chips distintos, a música nunca corta os efeitos sonoros, e vice-versa.

### Efeitos sonoros (PSG)

Todos os SFX são sintetizados em tempo real por acesso direto ao PSG, sem amostras. Cada tipo de som tem um canal fixo, evitando que dois efeitos disputem a mesma voz:

| Função | Canal PSG | Comportamento |
|--------|-----------|---------------|
| `SFX_playerShot` | canal 1 (tom) | Tiro do jogador: começa em 1400 Hz e o pitch **cai** a cada frame (500 + `sfxShot`*90), com volume decaindo. Duração de 10 frames. |
| `sfxItemPickup` | canal 2 (tom) | Coleta de item: "blip" **ascendente** de recompensa (pitch sobe de 600 Hz até ~1400 Hz), 8 frames. Nome em minúsculo pois cumpre o contrato declarado em `game.h`. |
| `SFX_enemyShot` | canal de ruído (3) | Tiro inimigo: ruído branco curto (`PSG_NOISE_FREQ_CLOCK4`), 8 frames, envelope moderado. |
| `SFX_bombBlast` | canal de ruído (3) | Explosão da bomba: ruído branco mais grave (`PSG_NOISE_FREQ_CLOCK2`), mais longo (30 frames) e mais alto (`PSG_ENVELOPE_MAX`). Tem **prioridade** sobre o tiro inimigo — enquanto a explosão soa, ela zera `sfxNoise`, silenciando qualquer tiro inimigo no mesmo canal. |
| ambiente | canal 0 | Som ambiente do terreno (ver abaixo). |

O decaimento de todos os efeitos é processado por `SFX_update`, que deve ser chamado a cada frame no loop principal: ele reduz os contadores de duração e recalcula frequência/envelope até o silêncio. `SFX_silence` zera todos os contadores e força envelope mínimo nos quatro canais.

### Som ambiente (canal 0)

O canal 0 é dedicado ao som contínuo do elemento de terreno sob o jogador, dirigido por `SFX_ambient(kind)`:

- **`SFX_AMB_LAVA`** — crepitar de fogo agudo: frequência alta e trêmula (~900 Hz + tremor aleatório `random() & 0x1FF`) com volume baixo flutuante (8 + `random() & 3`), simulando o "crackle" das chamas.
- **`SFX_AMB_MUD`** — passos na lama: um "squelch" grave e curto a cada ~1/3 s (período = `fps / 3`), com frequência baixa subindo levemente (100 + t*25) e volume baixo que decai; entre os passos, silêncio.

O tipo ambiente é reaplicado a cada quadro pelos elementos de fase. Na prática o loop reseta o ambiente para `SFX_AMB_NONE` e a lava/lama voltam a religá-lo enquanto o jogador está sobre elas. Se nenhum elemento reativa, o canal 0 volta ao envelope mínimo (silêncio). Nas telas de pausa, preparação e game over o ambiente é silenciado.

### Música de fundo (PCM)

A trilha toca no **DAC do YM2612** através do driver PCM do SGDK (`SND_PCM_loadDriver`), separado do PSG — por isso não interfere nos efeitos sonoros. É áudio PCM **13,4 kHz, mono, 8-bit** (`SOUND_PCM_RATE_13400`, `SOUND_PAN_CENTER`), a partir do recurso `res/bgm.res` (`mus_intro[]` e `mus_loop[]`).

A reprodução é uma máquina de estados de três posições — `MUS_IDLE`, `MUS_INTRO`, `MUS_LOOP`:

1. `MUSIC_start` toca a **intro** (0–15 s) **uma única vez** (flag de loop `FALSE`) e passa para `MUS_INTRO`.
2. `MUSIC_update`, chamado a cada frame, detecta o fim da intro (`!SND_PCM_isPlaying()`) e engata o **trecho de loop** (16–45 s) com repetição infinita (flag `TRUE`), indo para `MUS_LOOP`.

A música só começa ao **entrar no jogo**, e não na tela-título: a tela-título usa modo bitmap, que interrompe o DAC e impediria a reprodução PCM.

## Gráficos

### Tiles procedurais

**Todos** os gráficos do jogo são gerados por código em tempo de execução — não há assets de imagem externos. A rotina `buildTiles` (em `gfx.c`) preenche um buffer de tiles 4bpp (`tileBuf`, um `u32` por linha de 8 pixels) e o envia à VRAM com `VDP_loadTileData(... DMA)`. São gerados: o **jogador** (círculo de raio 7,5, 16x16), os **inimigos** (quadrados de borda clara — vermelho e amarelo 16x16, roxo/tanque 32x32), os **projéteis** (discos 8x8), a **parede** (bloco sólido com linha de brilho), **lava** e **lama** (delegados a `LAVA_buildTile`/`MUD_buildTile`), o **coração** e a **bomba** (itens, via `ITEMS_buildTiles`), a **caveira** do contador de mortes, o ícone de **bomba** do HUD e os três **ícones de inimigo** (vermelho/amarelo/roxo) para as telas de fase e pausa.

**Layout de sprite (ordem coluna).** Os sprites do Mega Drive armazenam os tiles em **ordem de coluna**: percorre-se a coluna inteira antes de passar à próxima. O helper `setPix(baseTile, x, y, color, colTiles)` traduz uma coordenada de pixel para o tile e bit corretos:

```c
u16 tile = baseTile + (x / 8) * colTiles + (y / 8);
tileBuf[tile * 8 + (y % 8)] |= (u32) color << ((7 - (x % 8)) * 4);
```

`colTiles` é a altura do sprite em tiles (2 para 16x16, 4 para 32x32), de modo que avançar 8 px em X salta uma coluna inteira de tiles. O deslocamento `(7 - (x%8)) * 4` posiciona o nibble de 4 bits da cor: o pixel mais à esquerda ocupa os bits altos do `u32` da linha. Tiles 8x8 simples (caveira, bomba, ícones) são desenhados a partir de mapas de caracteres com `buildTile8`.

### Paleta

São usadas duas paletas: **PAL0** para texto/HUD e **PAL1** para os objetos do jogo. A PAL1 é definida em `initPalette` (índices de hardware 17–27, correspondendo aos índices 1–11 dentro da paleta):

| Índice (PAL1) | Cor (RGB) | Uso |
|---------------|-----------|-----|
| 1 | 0xF0F0F0 | Contorno claro (bordas de sprites e ícones) |
| 2 | 0x20C0E0 | Jogador (ciano) |
| 3 | 0xD02020 | Inimigo vermelho |
| 4 | 0xF0E040 | Inimigo amarelo / tiro do jogador |
| 5 | 0xF08020 | Tiro inimigo |
| 6 | 0x506070 | Parede (corpo) |
| 7 | 0x8098A8 | Parede (brilho) |
| 8 | 0x9040D0 | Inimigo roxo (tanque) |
| 9 | 0x8A5A28 | Lama (marrom) |
| 10 | 0x5A3A14 | Lama (torrões escuros) |
| 11 | 0x282830 | Corpo da bomba |

O índice **0** é a cor de fundo compartilhada; é ele que o flash da bomba manipula (ver "Flash de tela").

### Sprites de hardware

A lista de sprites é **reconstruída do zero a cada frame** por `GFX_drawSprites`, encadeada por link e enviada por DMA (`VDP_updateSprites(n, DMA_QUEUE)`). Cada sprite recebe como próximo link o índice seguinte (`n + 1`), e o último fecha a cadeia com link 0 (`VDP_setSpriteLink(n - 1, 0)`). A ordem de montagem é:

1. **Jogador** (sprite 0; sai da tela em `y = -32` quando pisca durante a invencibilidade).
2. **Inimigos** ativos — tile e tamanho conforme o tipo (roxo 32x32, vermelho/amarelo 16x16).
3. **Itens** (`ITEMS_draw`).
4. **Projéteis** — tiros do jogador e depois tiros inimigos (estes com coordenadas em ponto fixo, deslocadas `>> 6`).

Nas telas sem ação (título, pausa, game over) usa-se `GFX_hideSprites`, que envia uma lista mínima de um único sprite fora da tela.

### Flash de tela

`GFX_flash` dispara um clarão de tela inteira usado na explosão da bomba, com duração de **18 frames (~0,3 s)**. Ele age sobre a **cor de fundo (índice 0)**, visível através de todos os tiles transparentes do interior da arena. `GFX_updateFlash`, chamado por frame, faz a cor **piscar branco e decair em degraus até o preto**: 0xF0F0F0 → 0xA0A0A0 → 0x505050 → 0x000000. `GFX_stopFlash` cancela o efeito e restaura o fundo preto imediatamente.

