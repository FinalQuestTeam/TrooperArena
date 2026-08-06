# MegaDrive — ambiente de desenvolvimento SGDK

Ambiente completo para desenvolver jogos de Sega Mega Drive em C com o [SGDK 2.11+](https://github.com/Stephane-D/SGDK) no macOS (Apple Silicon).

## Estrutura

```
MegaDrive/
├── docs/
│   ├── sgdk-guia.html      # Guia completo da API em português (abra no navegador)
│   └── fragmentos/         # Fragmentos HTML por módulo (fonte do guia)
├── SGDK/                   # Clone oficial do SGDK (lib compilada em lib/libmd.a)
├── game/                   # Projeto do jogo
│   ├── src/main.c
│   ├── res/                # Recursos (.res) compilados pelo rescomp
│   ├── inc/
│   └── Makefile
└── README.md
```

## Ferramentas instaladas

| Ferramenta | Origem | Uso |
|---|---|---|
| `m68k-elf-gcc` 16.1 (+binutils) | Homebrew | Cross-compiler 68000 |
| OpenJDK 26 | Homebrew (keg-only) | rescomp, sizebnd, lz4w (jars do SGDK) |
| `sjasm`, `bintos`, `xgmtool` | compilados de `SGDK/tools` para `SGDK/bin` | Assembler Z80 e conversores |
| CMake | Homebrew | Build das tools do SGDK |
| ares | Homebrew cask (`ares-emulator`) | Emulador principal (nativo arm64, assinado) — `open -a ares out/rom.bin` |
| OpenEmu | Homebrew cask | Alternativa; roda via Rosetta (build Intel sem assinatura) e pode crashar no macOS 26 |

Variáveis no `~/.zshrc` (bloco `SGDK / Mega Drive`): `GDK` aponta para o clone do SGDK e o `PATH` inclui o OpenJDK e `$GDK/bin`.

## Compilar o jogo

```sh
cd game
make            # gera out/rom.bin
make clean
```

Abra `out/rom.bin` no OpenEmu (ou outro emulador, ex.: BlastEm baixado de retrodev.com, que tem debugger).

## Recompilar a biblioteca (só após atualizar o SGDK)

O make apaga os intermediários dos drivers Z80 antes de os `.c` consumirem os headers gerados; pré-gere-os antes:

```sh
cd $GDK
for f in $(find src -name "*.s80"); do
  base=${f%.s80}
  mkdir -p "out/release/$(dirname $base)"
  sjasm -isrc -iinc -iinc/snd -iout/release "$f" "out/release/$base.o80" out/release/out.lst
  bintos "out/release/$base.o80" "out/release/$base.s"
done
java -jar bin/rescomp.jar res/libres.res out/release/res/libres.rs
make -f makelib.gen
```

Se atualizar as tools nativas: `cmake -S . -B build -DCMAKE_SYSTEM_NAME=Generic && cmake --build build` (ignore o passo de arquivamento da lib do CMake no macOS — ele corrompe o `libmd.a`; use sempre o `makelib.gen` acima).

## Documentação

- **Guia completo em português**: `docs/sgdk-guia.html` — referência função por função de todos os headers, introdução, build e rescomp. Também publicado em: https://claude.ai/code/artifact/669a50c5-a0ce-4c2f-9e12-d300db1b4971
- Doxygen oficial: http://stephane-d.github.io/SGDK/
- Wiki oficial: https://github.com/Stephane-D/SGDK/wiki
- Recursos: leia `SGDK/bin/rescomp.txt` para declarar sprites, imagens, música (.res)
