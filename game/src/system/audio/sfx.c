// Efeitos sonoros: acesso direto ao PSG (SN76489), um canal por tipo de som.

#include "system/audio/sfx.h"
#include "core/game.h"

static u16 sfxShot;
static u16 sfxNoise;
static u16 sfxPick;
static u16 sfxBoom;
static u16 sfxHurt;

static u8  ambientKind;     // som ambiente atual (canal 0)
static u16 ambientPhase;    // fase do padrão ambiente (passos da lama)

void SFX_playerShot(void)
{
    sfxShot = 10;                       // frames de duração
    PSG_setFrequency(1, 1400);
    PSG_setEnvelope(1, 2);
}

void SFX_enemyShot(void)
{
    sfxNoise = 8;
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK4);
    PSG_setEnvelope(3, 4);
}

// contrato de game.h (chamado pelos itens na coleta)
void sfxItemPickup(void)
{
    sfxPick = 8;
    PSG_setFrequency(2, 600);
    PSG_setEnvelope(2, 2);
}

// dano SOFRIDO pelo jogador (contato/projétil de inimigo): tom grave descendente,
// bem distinto do "blip" agudo da coleta. O dano da lava fica só com o crepitar
// ambiente (updateAmbient) — quem chama é damagePlayer, nunca loseHp.
void SFX_playerHit(void)
{
    sfxHurt = 14;                       // frames de duração
    PSG_setFrequency(2, 300);
    PSG_setEnvelope(2, 1);
}

void SFX_bombBlast(void)
{
    sfxBoom = 30;
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK2);
    PSG_setEnvelope(3, PSG_ENVELOPE_MAX);
}

void SFX_ambient(u8 kind)
{
    ambientKind = kind;
}

void SFX_silence(void)
{
    sfxShot = 0;
    sfxNoise = 0;
    sfxPick = 0;
    sfxBoom = 0;
    sfxHurt = 0;
    ambientKind = SFX_AMB_NONE;
    ambientPhase = 0;
    PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
    PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
    PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
    PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
}

// canal 0: som ambiente do terreno sob o jogador
static void updateAmbient(void)
{
    if (ambientKind == SFX_AMB_LAVA)
    {
        // crepitar: frequência alta e trêmula, volume baixo flutuante (crackle)
        PSG_setFrequency(0, 900 + (random() & 0x1FF));
        PSG_setEnvelope(0, (u8) (8 + (random() & 3)));
        ambientPhase = 0;
    }
    else if (ambientKind == SFX_AMB_MUD)
    {
        // passo: "squelch" grave e curto a cada ~1/3 s, silêncio entre eles
        const u16 period = fps / 3;
        const u16 t = ambientPhase % period;
        if (t < 5)
        {
            PSG_setFrequency(0, 100 + t * 25);
            PSG_setEnvelope(0, (u8) (8 + t));       // volume mais baixo, decai
        }
        else PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
        ambientPhase++;
    }
    else
    {
        PSG_setEnvelope(0, PSG_ENVELOPE_MIN);       // sem ambiente
        ambientPhase = 0;
    }
}

void SFX_update(void)
{
    if (sfxShot)
    {
        sfxShot--;
        // pitch cai e volume decai até silenciar
        PSG_setFrequency(1, 500 + sfxShot * 90);
        PSG_setEnvelope(1, sfxShot ? (u8) (12 - sfxShot) : PSG_ENVELOPE_MIN);
    }
    if (sfxBoom)
    {
        // explosão: mais longa e mais alta, tem prioridade sobre o tiro inimigo
        sfxBoom--;
        sfxNoise = 0;
        PSG_setEnvelope(3, sfxBoom ? (u8) (15 - (sfxBoom >> 1)) : PSG_ENVELOPE_MIN);
    }
    else if (sfxNoise)
    {
        sfxNoise--;
        PSG_setEnvelope(3, sfxNoise ? (u8) (12 - sfxNoise) : PSG_ENVELOPE_MIN);
    }
    if (sfxHurt)
    {
        // dor: pitch grave caindo; tem prioridade sobre a coleta (mesmo canal 2)
        sfxHurt--;
        sfxPick = 0;
        PSG_setFrequency(2, 110 + sfxHurt * 14);
        PSG_setEnvelope(2, sfxHurt ? (u8) (15 - sfxHurt) : PSG_ENVELOPE_MIN);
    }
    else if (sfxPick)
    {
        sfxPick--;
        // pitch sobe: "blip" de recompensa
        PSG_setFrequency(2, 1400 - sfxPick * 100);
        PSG_setEnvelope(2, sfxPick ? (u8) (11 - sfxPick) : PSG_ENVELOPE_MIN);
    }

    updateAmbient();
}
