# pc-abelsmt

A native PC port of **ABEL - Shin Mokushiroku Taisen** (ＡＢＥＬ 真・黙示録大戦,
Family Soft, 1995; FM Towns) by static recompilation of its 386 code to C++
on fmtownskit (a submodule at `fmtownskit/`; `git clone --recursive`), and
then its translation into English. **BYOA**: nothing from the disc is in
this repository; you bring your own.

Status: **session 2** -- the opening's program (OPNG), the game's (ABEL) and
TBIOS recompiled to C++ and run one after another as the disc's START runs
them: the logo, the title and its menu and the game's first room the same,
pixel for pixel, as MAME's; the whole opening plays. Not yet: the ending's
program, the sound checked, the game's pace (MAME marks it "Runs too fast":
its pace is its CPU's, and the port's aim is the real machine's).

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
build/rt/abel                          # the window: arrows, X = A, Z = B, Enter = Run
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
