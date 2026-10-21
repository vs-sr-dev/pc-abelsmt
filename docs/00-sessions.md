# 00 - Sessions

## Session 1 (2026-10-21): the disc, the programs' surface, the plan

* **The disc identified**: Redump's "Abel - Shin Mokushiroku Taisen
  (Japan)", MTC-1137, Family Soft, 1995; every track and the cue sheet match
  MAME's software list (`abel`, 21 of 21), and the CHD the oracle makes has
  the list's SHA-1 (`01-disc-layout.md`). The list's "partial" has its
  reason on the line above the entry: **"Runs too fast"**.
* **The boot**: a newer system than the first two discs' (IO.SYS 3.58 of
  1994, RUN386 L51, another FORRBIOS.NSD), **TBIOS in modules** loaded by
  `TBIOSLD.SYS`, and `START.EXE`, a compiled batch that decrypts its own
  script: **three programs** in turn, `OPNG` (logo, opening, title),
  `ABEL` (the game, or `ABEL LOAD`), `ENDG` (the ending, with a number).
* **The programs** (`03-executables.md`): each a P3 behind Fujitsu's
  "exp2exe" DOS stub, MetaWare High C, no symbols. Discovery from the
  entries: 605, 1,522 and 451 functions, nothing refused; `ABEL.EXE`'s four
  conflicts are real overlapping copies. Their surface: TBIOS's graphic,
  sound, warikomi and System slots, int 4Dh for the sound's interrupt, a
  **MIDI driver** for a card at ports 0xE70/0x4A0 that MAME does not have,
  the pads read directly; the CD-DA in OPNG and ENDG only.
* **TBIOS composed from the disc's files**: eight Phar Lap "MQ" modules
  laid out and linked as TBIOSLD does (`fmtownskit.tbiosld`), the memory's
  code byte for byte; the port needs no TBIOS read from the oracle.
* **Why it runs too fast** (*inferred*, from MAME's instruction traces): the
  main loop waits on nothing, walking and scrolling are as fast as the CPU
  draws; MAME's 386 has no wait states, and the kit's clocks are the same
  datasheet's -- the port must be calibrated against a real Towns
  (`05-open-questions.md` 1). The map scrolls by 16-pixel tiles, on MAME a
  step every 1.5 frames; a YouTube video the user watched looks normal and
  "fluid". The 11-minute intro is the CD's tracks 2-10, one per scene: its
  length is the same on any machine.
* **The text, for the translation** (`02-data-formats.md`): one format
  (`u32 speaker`, Shift-JIS, NUL; an index of id, count, offset) in the
  four `.MES` files and in 95 of the 107 maps' chunks, read to the last byte:
  **3,519 lines, 68,278 characters**, 89 speakers, the control codes
  listed; about 4,900 characters more in `ABEL.EXE` (menus, items,
  monsters); the pictures with drawn text listed (`OP01`, `OPTP_S01`-`S15`,
  `ED07A`, the gallery's pages). `tools/abelfiles.py` reads the text, the
  maps and the 4-bit TIFFs.
* **The oracle**: `tools/oracle.py` from 4D Driving, for this disc
  (`fmtowns -ramsize 4M`), with `--debug` for a Lua script's debugger
  commands; the game reached (CD PLAY, GAME START), the opening's CD-DA
  (track 02) held against the disc.
* **fmtownskit** (`10-fmtownskit.md`): the bound stub (`exp`), `tbiosld`,
  far calls told from MetaWare's switches and bounds of rewritten indices
  (`x86map`), signed bounds (`x86map`, `recomp.discover`) -- the first two
  games' results unchanged but for two false far calls.
* The plan (`06-attack-plan.md`): the three programs recompiled on the
  Towns with 4 MB and the composed TBIOS, OPNG first; the pace calibrated;
  then the translation from one phrase table, to the port and to a patch
  of the disc.

The repository put under git, fmtownskit a submodule at 2b785bc (the
session's kit commit, on e4c5334).
