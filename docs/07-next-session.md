# 07 - Next session

Where session 1 left it: the disc identified and read (three programs, a
TBIOS in modules, the start's script decrypted), the programs' surface
mapped, discovery clean on all three, the kit able to take them (the bound
stub, the composed TBIOS, the far calls told from switches, signed bounds),
the text read to its last byte and counted, the oracle running the game and
its opening's CD-DA -- and the reason for MAME's "Runs too fast" found: the
game's pace is its CPU's.

Goal: **the start of OPNG, recompiled and drawn** (`06-attack-plan.md`).

1. **The first instruction of OPNG** on `fmtowns -ramsize 4M`
   (`tools/oracle.py --dump-write`, as 4D Driving's session 3): the
   registers, the descriptors, the segment's limit; the TBIOS in memory
   against `python -m fmtownskit.tbiosld build/disc/TBIOS/TBIOS.INI --check`
   at that moment (only the work areas should differ, fewer of them); the
   runtime's profile for this RUN386.
2. **Recompile OPNG and TBIOS** (`python -m fmtownskit.recomp`; TBIOS from
   the composed image, `--raw` with its table 0x20-0x200): the overlapping
   instructions; the self-test in slices.
3. **The runtime's first frames**: the Family Soft logo (20 s), the title
   and its menu (25 s), against the oracle's snapshots pixel for pixel.
4. **The pace, first look**: the clocks the runtime counts for the logo's
   and the title's waits (are those timed by the YM's timer, or by the CPU
   too?), against the oracle's seconds.

Worth doing on the way: the oracle to the first dungeon (the only gameplay
video starts there; the user compares by eye how it moves, for
`05-open-questions.md` 1) -- the inputs that get there, written down; what
MAME answers at ports 0xE71 and 0x4A1.

Tools: `tools/oracle.py` (`fmtowns` with 4 MB by default; `--at`, `--input
T0-T1:BUTTONS` -- the title: `30-30.2:A,38-38.2:A` for CD PLAY, GAME START;
`26-26.15:Down,27-27.15:Down,28-28.15:Down,29.5-29.7:A` for OPENING --,
`--dump`, `--dump-write`, `--wav`, `--sound-log`, `--lua`, `--debug` for a
Lua script's debugger commands), `tools/abelfiles.py` (text, maps, TIFFs).
Kit work runs from `D:\Homebrew6` (the submodule is the last commit, 2b785bc):
`python -m fmtownskit.exp|x86map|tbiosld|recomp.discover ...`.
