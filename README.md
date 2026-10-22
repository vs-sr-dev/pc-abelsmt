# pc-abelsmt

A native PC port of **ABEL - Shin Mokushiroku Taisen** (ＡＢＥＬ 真・黙示録大戦,
Family Soft, 1995; FM Towns) by static recompilation of its 386 code to C++
on fmtownskit (a submodule at `fmtownskit/`; `git clone --recursive`), and
then its translation into English. **BYOA**: nothing from the disc is in
this repository; you bring your own.

Status: **session 3** -- the opening's program (OPNG), the game's (ABEL),
the ending's (ENDG) and TBIOS recompiled to C++ and run one after another as
the disc's START runs them: the logo, the title and its menu and the game's
first room the same, pixel for pixel, as MAME's; the whole opening and the
ending play; the sound right by ear. The pace: MAME marks the game "Runs
too fast" (its pace is its CPU's); the port's window runs the CPU at 35% of
MAME's speed, an estimate by eye against a video of the game, not a
specification (`--cpu`; headless runs at MAME's 100%). Not yet: the way to
the first dungeon held against MAME, the save, the translation.

## The translation

ABEL will be translated into English: the game's dialogue (its `.MES` and
`.ABM` records, about 89 speakers), its menus and the opening's and the
ending's text. One phrase table, two outputs: **the port**, which will draw
the English text itself, and **a patch for the disc** (xdelta), for MAME and
real machines. The text's census and where the game reads and draws it are in
`docs/02-data-formats.md` and `docs/05-open-questions.md` (12, 13).

## The disc

Redump's *Abel - Shin Mokushiroku Taisen (Japan)* (MTC-1137): a cue sheet
and twenty track files, in `disc/`. `docs/01-disc-layout.md` has how they
were checked. MAME's BIOS files go in `bios/` (`fmtowns.zip`, the software
list `fmtowns_cd.xml`; `fmtowns/fmt_fnt.rom` for the runtime later).

## Building and running

```sh
python -m fmtownskit.disc "disc/Abel - Shin Mokushiroku Taisen (Japan).cue" --extract build/disc
sh tools/recomp.sh                     # the programs and TBIOS to C++, in build/game
cmake -S . -B build/rt -G Ninja -DCMAKE_CXX_COMPILER=clang++
ninja -C build/rt
build/rt/abel                          # the window (CPU at 35%): arrows, X = A, Z = B, Enter = Run
build/rt/abel --record play.txt        # the play written down (--cpu N from the start: the game measures it)
build/rt/abel --headless --play play.txt --png-every 60
build/rt/abel --headless --frames 4800 --png-every 60 --input 840-852:a,1320-1332:a
```

`docs/11-runtime.md` has the options; the font ROM is read from
`bios/fmtowns/fmt_fnt.rom`.

## Tools

```sh
python -m fmtownskit.exp build/disc/ABEL/ABEL.EXE
python -m fmtownskit.x86map build/disc/ABEL/ABEL.EXE
python -m fmtownskit.recomp.discover build/disc/ABEL/ABEL.EXE --report
python -m fmtownskit.tbiosld build/disc/TBIOS/TBIOS.INI --out build/tbios.bin
python tools/abelfiles.py text build/disc/ABEL --tsv build/text.tsv   # the dialogue, checked
python tools/oracle.py --at 40-120/5 --input 30-30.2:A,38-38.2:A      # MAME, CHDMAN, FMT_ROMS
python tools/oracle.py --dump 66 --input 30-30.2:A,38-38.2:A           # CPU state and RAM
python tools/oracle.py --at 50-300/2 --lua tools/abelquit.lua --input 30-30.2:A,38-38.2:A   # the ending
python tools/framematch.py build/oracle/x/fmtowns build/frames --at 17,18,19   # oracle against port
```

The oracle runs MAME's `fmtowns` with 4 MB by default; at the title, A
twice is CD PLAY, then GAME START.

## Documents

`docs/00-sessions.md` (the log), `01-disc-layout.md`, `02-data-formats.md`,
`03-executables.md`, `04-curiosities.md`, `05-open-questions.md`,
`06-attack-plan.md`, `07-next-session.md`, `09-recompiler.md`,
`10-fmtownskit.md`, `11-runtime.md`.

## Licence

MIT (`LICENSE`). The game and its system software are their owners'.
