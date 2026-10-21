# 06 - The attack plan

## What the port is

As the first two games': a native PC program that runs **the game's own
code and TBIOS's own code, recompiled from 386 to C++** by fmtownskit, on
the kit's runtime standing in for the machine under them (VRAM and CRTC,
the sprite controller, the YM3438 and RF5C68, the CD and its CD-DA, the
interrupts, the pad). DOS, Phar Lap and the BIOS interrupts are answered at
their boundary, from the disc image. Everything shared goes into the kit;
what is this game's stays here. Then a translation, from one phrase table
(below).

The machine is 4D Driving's: **the FM Towns with 4 MB, the disc's TBIOS**
(MAME's `fmtowns -ramsize 4M`, the oracle's default; profile `towns4m+nsd`
*to be confirmed* for this disc's RUN386 L51 and its larger IO.SYS: the
program sits at the same linear 0x408000, physical 0x4F000 against 0x49000).

What is new, and why the shape still holds:

* **Three programs, not one.** OPNG, ABEL and ENDG are three P3 images,
  each recompiled as a module of its own (the kit's modules are keyed by
  code selector, and the three run one after another under the same 0x0C:
  the runtime loads one image at a time, as RUN386 does). The script that
  chains them (`START`) is five lines of logic: the runtime's DOS can answer
  EXEC and the errorlevel, or the port can run the chain itself.
* **The largest program yet**: `ABEL.EXE` 1,522 functions and 81,000
  instructions (4D Driving: 1,289 and 68,800), and **no symbols** -- the
  C++'s names are addresses, the seeds what discovery and the oracle find.
  Discovery already runs with nothing refused; its four conflicts are real
  overlapping code the emitter must take.
* **TBIOS in modules**: composed from the disc's files by the kit
  (`fmtownskit.tbiosld`), byte for byte the memory's code -- the recompiler
  takes that image as it took the one-file `TBIOS.BIN` (`--raw`, its far-pointer
  table at 0x20-0x200), no oracle dump needed.
* **The game's pace is its CPU's** (`05-open-questions.md` 1): the runtime
  must run the guest at the real machine's speed, not as fast as it can and
  not as fast as MAME. The kit already counts the 386's clocks per block
  and runs the machine on them; what is missing is the calibration, and it
  needs a reference from a real Towns. This is the first thing that makes
  ABEL "work as it should" where MAME does not.
* **FM music**: the first game of the kit that plays the YM3438 for real
  (Euphony's scores), and a MIDI card to answer as absent.

## The road

1. **The start, read on the oracle** (OPNG's and ABEL's done, session 2;
   ENDG's to do): `--dump-write` at each program's
   first instruction (OPNG's, then ABEL's: the registers RUN386 hands over,
   the descriptors, the segment's limit, the composed TBIOS against memory
   at that moment); the profile for this RUN386.
2. **Recompile** (OPNG, ABEL and TBIOS done, session 2: 64,131 self-test
   vectors, 0 failures; ENDG to do) OPNG first (605 functions: the logo, the title, the
   prologue, the CD-DA), then ABEL, then ENDG; TBIOS from `tbiosld`'s
   image; the self-test in slices; the overlapping copies.
3. **The runtime, headless** (done to the throne room, session 2, but the
   CD-DA's WAV and the prologue's frames): the Family Soft logo, the title and its menu,
   the prologue pixel for pixel against the oracle (`tools/framematch.py`
   from 4D Driving); the CD-DA of OPENING against MAME's WAV (track 02 at
   53.9 s); then ABEL from GAME START to the throne room (60 s), held
   against the oracle frame by frame where the oracle's speed is not what
   decides (a still room, a dialogue's page).
4. **The pace**: the clock calibrated against a real machine's reference
   (`05-open-questions.md` 1), with the user's eyes; MAME's speed kept as a
   switch for comparison.
5. **The sound**: the FM and PCM music (Euphony) against MAME's WAV by
   replaying the oracle's sound log, then by ear; the MIDI ports answered
   as "no card".
6. **The window and the pad**, the save (`ABEL LOAD`), the ending (ENDG with
   its argument): **playable, "works until proven otherwise"**.
7. **The translation** (the user directs it: the guides to the characters
   and the story; the translating is Claude's), from one phrase table:
   * the table: every record of `02-data-formats.md` by its id
     (`FILE:ID.K`, `tools/abelfiles.py text --tsv`), `ABEL.EXE`'s strings by
     their offsets, the pictures' text by file -- Japanese and English side
     by side, the only source;
   * **the port's output**: the text substituted where the game loads it
     or draws it, with no length limits and a Western font of its own;
   * **the disc patch** (for whoever runs the game on an emulator): the
     records written back into the `.MES` and the maps' `.BIN`/`.IDX`
     (their offsets are 32-bit, the chunks' sizes too: a map grows freely),
     `ABEL.EXE`'s strings where their pointers allow, the pictures redrawn,
     the data track rebuilt and an xdelta of it; its limits -- the window's
     lines, the half-width font, the strings' room -- read before a word is
     translated.

## What is new here for the kit

| Need | Where |
|---|---|
| a P3 behind an MZ stub | `exp` (done, session 1) |
| a TBIOS in "MQ" modules, laid out by TBIOSLD's rules | `tbiosld` (done, session 1); the recompiler's `--raw` over its image (next) |
| MetaWare's switches told from far calls; a compare's bound dropped when the index is rewritten; signed bounds | `x86map`, `recomp.discover` (done, session 1) |
| overlapping instructions in one function's body | `recomp.emit` (compiled and self-tested, session 2) |
| three programs one after another under one code selector | the port's main runs START's script; the kit's `run386_load`, `run386_end`, `x86_park` (done for OPNG and ABEL, session 2) |
| the guest's clock rate against real time, calibrated | `runtime/towns_cpu`, `towns.h` (the profile's clock) |
| ports 0xE70/0xE71, 0x4A0/0x4A1 as MAME answers them | `runtime/towns_io`: 0xFF where MAME has no reader (done, session 2) |
| the YM3438 played for real (FM music) | `runtime/towns_sound` (held against MAME's WAV) |

## The risks

* **The calibration has no oracle.** MAME is the oracle for everything but
  speed, and for speed it is wrong. A reference recording of a real Towns,
  or documented wait states, is needed; failing both, the user's judgement
  of "how it should feel".
* **Size**: 81,000 instructions of C++ and three programs; build time.
* **No symbols**: every indirect target the runtime meets is found by
  running, and named by address.
* **The MIDI driver** may wait on a card that is not there in a way MAME
  hides (its unmapped ports' answer).
