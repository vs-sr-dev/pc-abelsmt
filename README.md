# pc-abelsmt

A native PC port of **ABEL - Shin Mokushiroku Taisen** (ＡＢＥＬ 真・黙示録大戦,
Family Soft, 1995; FM Towns) by static recompilation of its 386 code to C++
on fmtownskit (a submodule at `fmtownskit/`; `git clone --recursive`), and
then its translation into English. **BYOA**: nothing from the disc is in
this repository; you bring your own.

Status: **session 1** -- the disc identified and read (three programs, a
TBIOS in modules, the start's script), the programs mapped and discovery
clean, the text read and counted (3,519 lines, 68,278 characters), the
oracle on MAME's FM Towns playing the game, and the plan. MAME marks the
game "Runs too fast": its pace is its CPU's, and the port's first aim is
to run it at the real machine's. Nothing runs yet.

## The disc

Redump's *Abel - Shin Mokushiroku Taisen (Japan)* (MTC-1137): a cue sheet
and twenty track files, in `disc/`. `docs/01-disc-layout.md` has how they
were checked. MAME's BIOS files go in `bios/` (`fmtowns.zip`, the software
list `fmtowns_cd.xml`; `fmtowns/fmt_fnt.rom` for the runtime later).

## Tools

```sh
python -m fmtownskit.disc "disc/Abel - Shin Mokushiroku Taisen (Japan).cue" --extract build/disc
python -m fmtownskit.exp build/disc/ABEL/ABEL.EXE
python -m fmtownskit.x86map build/disc/ABEL/ABEL.EXE
python -m fmtownskit.recomp.discover build/disc/ABEL/ABEL.EXE --report
python -m fmtownskit.tbiosld build/disc/TBIOS/TBIOS.INI --out build/tbios.bin
python tools/abelfiles.py text build/disc/ABEL --tsv build/text.tsv   # the dialogue, checked
python tools/oracle.py --at 40-120/5 --input 30-30.2:A,38-38.2:A      # MAME, CHDMAN, FMT_ROMS
python tools/oracle.py --dump 66 --input 30-30.2:A,38-38.2:A           # CPU state and RAM
```

The oracle runs MAME's `fmtowns` with 4 MB by default; at the title, A
twice is CD PLAY, then GAME START.

## Documents

`docs/00-sessions.md` (the log), `01-disc-layout.md`, `02-data-formats.md`,
`03-executables.md`, `04-curiosities.md`, `05-open-questions.md`,
`06-attack-plan.md`, `07-next-session.md`, `10-fmtownskit.md`.

## Licence

MIT (`LICENSE`). The game and its system software are their owners'.
