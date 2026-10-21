# 11 - The runtime

The programs' code and TBIOS's, recompiled (`09-recompiler.md`), running on
fmtownskit's FM Towns runtime, headless or in a window: `build/rt/abel` runs
the disc's programs as its `START` does, from OPNG's first instruction.

```sh
sh tools/recomp.sh                                   # 09-recompiler.md
cmake -S . -B build/rt -G Ninja -DCMAKE_CXX_COMPILER=clang++
ninja -C build/rt
build/rt/abel --headless --frames 4800 --png-every 60 --input 840-852:a,1320-1332:a
```

Options (`src/main.cpp`), 4D Driving's: `--cue`, `--headless`, `--frames N`,
`--png F,...`, `--png-every N`, `--out DIR`, `--input F-G:BUTTONS`, `--wav`,
`--mute`, `--trace-int`, `--trace-io`, `--trace-from F`, `--vram F`, `--ram F`,
`--crop`, `--save DIR`, `--font FILE`, `--replay-sound LOG --seconds S --wav
FILE`, `--scale`, `--fullscreen`. Frame F is the oracle's 16.002 + F/60
seconds (OPNG's first instruction, below). The recompiled modules are checked
against the disc at the start (each one's CRC32 against the file's, TBIOS's
against the image composed from its modules).

In the window: arrows, X = A, Z = B, Enter = Run, Right Shift or Backspace =
Select; a gamepad; F11 or Alt+Enter full screen, Tab held unpaced, Escape
quits (the kit's `towns_sdl`, as 4D Driving's).

## The start, read on the oracle

As 4D Driving's session 3: `tools/oracle.py --dump-write ADDR=VALUE` stops
MAME at a program's first write. OPNG's and ABEL's entries do not start with
a `call` but with a test of EAX for a loader's signature (0xABCDABCD) and a
byte written to their data: 2 when the signature is not there ([0x473C4] in
OPNG, [0x9B834] in ABEL, both 0 in the files). The physical address comes
from a dump taken a little later (`--dump`, `mamedump --pages`).

| | OPNG | ABEL |
|---|---|---|
| stopped at | `--dump-write 973c4=2`, **16.002 s** after power-on | `--dump-write 3bb834=2 --input 30-30.2:A,38-38.2:A`, **45.225 s** |
| registers | all 0, EFLAGS 0x202, ESP 0x693A0 (the header's) | all 0, EFLAGS 0x202, ESP 0xBE4E8 |
| segment 0x0C/0x14 | base 0x408000, limit **0x376FFF**, physical **0x50000** (384 KB, then pages from the top of RAM down) | base 0x408000, limit **0x377FFF**, physical **0x4F000** (388 KB) |
| image in RAM | the file's, byte for byte | the file's, byte for byte |
| past the image | zero but for RUN386's `iretd` frame below the stack top (EIP the entry, CS 0x0C, EFLAGS 0x202) | the same |
| TBIOS (0x100000) | **the composed image, 0 bytes differ** | 365 bytes differ in 28 runs, all in the sound and int 04dh modules' work areas: **OPNG's leftovers** (TBIOS stays in memory from one program to the next) |
| PSP (0x24, 0x04) | 0xBF890; command tail empty (length 0) | 0xBF890; tail empty (no LOAD) |
| environment (0x2C) | 0xBF820, limit 0x56: `COMSPEC=C:\COMMAND.COM`, `PROMPT=$p$g`, `PATH=Q:\;Q:\HCOPY;Q:\T_TOOL`, then `Q:\ABEL\OP\OPNG.exp` | limit 0x53, the same with `Q:\ABEL\ABEL.exp` |
| CRTC, video registers, palettes (`ports.txt`) | **4D Driving's at its start**, register for register | the same |
| PICs | master mask 0x3C; **slave 0xCE** (4D Driving's 0xEE: IRQ 13, the sound chips', unmasked) | the same |

So the machine is 4D Driving's profile `towns4m+nsd` with each program's own
physical base and limit (OPNG's one page higher: START's batch, the stub and
RUN386 below it, *inferred*). `src/main.cpp` declares the two profiles,
sets the differences from the kit's `townsos()` (4D Driving's three and the
slave PIC's mask), and writes each program's PSP tail and environment.

## The programs one after another

`START`'s script (`01-disc-layout.md`) is the port's: OPNG; ABEL, or `ABEL
LOAD` if OPNG's errorlevel is 1 or more; ENDG 429266408 if ABEL ends with 0
(not recompiled yet: the run stops there and says so). Between two programs
the machine stays as it is -- TBIOS with its work areas, VRAM, the chips, the
CD -- and the kit undoes what DOS and RUN386 undo (`run386_end`: the files
closed, the protected-mode vectors back to none, the timer BIOS's timers off);
the next program is loaded into a fresh segment (`run386_load`). Both run
under code selector 0x0C: the one not running is parked (`x86_park`), so the
runtime's lookups by selector find the right one. START's `cd \abel\op` and
`cd \abel` are the CD's current directory (`Machine::cd_dir`): OPNG opens
`FM_NOCOP.TIF` with no path.

With A at frames 840 (CD PLAY) and 1320 (GAME START), **OPNG exits with
errorlevel 0 at frame 1367** and ABEL starts: it writes `ABEL Ver 1.00`,
`System Loading`, then a dot per file to the console (shown on the host's
terminal: below), sets up the game and reaches **the throne room and its
main loop** (0xE65, the loop session 1 read on the oracle).

## What the programs asked for, and the answers

Read call by call on the oracle (`tools/oracle.py --trace-calls` at the `int`
and after it) where the runtime had no answer:

* **The timer BIOS, int 97h** (`05-open-questions.md` 7; FORRBIOS.NSD hands
  the call to the ROM's real-mode BIOS through its generic bridge at
  0054:026C). OPNG's library keeps eight slots, each with a far trampoline
  (0x2F937 + 10n) that calls the user's handler. The packet at DS:EDI:
  `[0]` the mode (2 here; the library frees the slot after a call when bit 0
  is set: once), `[2]` the callback's offset and `[6]` its selector, `[8]`
  and `[10]` a limit as two words, the high one first. AH=00h starts a timer
  and answers its number in AL (0 first); AH=01h stops timer AL; AH=02h
  writes timer AL's count of ticks to `[8]`/`[10]` of the buffer at DS:EDI.
  AH=0 is success. **The tick is 10 ms**: OPNG's waits poll 02h until the
  count reaches 5 (every 50 ms on the oracle: 0, 1, ... 5 over three frames),
  and its limits of 400 and 2000 ticks call back after 4 and 20 s -- the one
  at 400 ran 240 frames (4.00 s) after it was set. The kit now has it
  (`Machine::int97`), counting the guest's clock, the callback called at the
  first safe point with interrupts on.
* **The MIDI card** (`05-open-questions.md` 3): the driver probes ports
  0xE51, 0xE59, 0xE61, 0xE69, 0xE70/0xE71 and 0x4A0-0x4AD; **MAME has nothing
  there and reads 0xFF** (its io map's `unmap_value_high`, read from Lua with
  `tools/ioread.lua`), and the driver takes that for no card. The kit had
  answered 0, the driver found a card and waited for ever for its buffer to
  drain. The kit now answers 0xFF wherever MAME's io map has no reader --
  0x470 too, the odd bytes under a `umask` (0x5ED, 0x3001: 0xFF on MAME).
* **DOS**: 2Ah and 2Ch (the date and the time: the host's, as MAME's RTC is);
  44h AL=00h on the standard handles (the console, **80D3h**, ABEL's stdout
  on the oracle); 40h to handles 1 and 2 (the console: ABEL's loading
  messages). Names longer than 8.3 are cut as DOS cuts them (OPNG asks for
  `GSCAPTALUP.FMB` and gets `GSCAPTAL.FMB`).
* **The palette's colour registers**: MAME keeps one r, g, b register per
  entry (0xFD92-0xFD96), whichever palette is chosen, and a component written
  sets the chosen palette's whole colour from all three. ABEL writes its
  throne room's red with the 16-colour palette chosen after the other
  components went in with the 256-colour one: written component by component,
  the floor came out (0, 32, 48) where MAME shows (144, 32, 48). The kit now
  has the registers. Reading 0xFD93/0xFD95 gives 0, as on MAME.

## Held against the oracle

`tools/framematch.py` (from 4D Driving; frame F is the oracle's 16.002 + F/60 s):

* **OPNG, 17 to 44 s** (one snapshot a second; the runtime's every frame):
  the Family Soft logo, the black between, the title, its menu -- **28
  snapshots of 28 the same, pixel for pixel**.
* **ABEL, the throne room** (oracle 65 and 70 s with A at 30 and 38 s): **the
  same, pixel for pixel**. The snapshot at 60 s falls inside the room's fade
  in and differs in 5.2% of its pixels from the nearest runtime frame (a step
  of the fade, *not checked* which).
* **Not the same**: ABEL's loading screen. On the oracle the console's text
  (`ABEL Ver 1.00  System Loading....`) is on the screen, in the 800x440 frame
  of the system's text mode; here it is written to the host's terminal and the
  screen stays as OPNG left it (`05-open-questions.md`).
* Left alone, **OPNG plays its whole opening** -- the prologue's text, the
  laboratory and its dialogue, the ABEL Project Team's credits, the space
  battle, the gallery of characters, the last picture -- and goes back to the
  title and the logo, again and again: 15 minutes of the guest in 11 seconds
  headless, nothing missing at the runtime (`00-sessions.md`).

## The pace, first look

OPNG's waits are the timer BIOS's, real time, not the CPU's. Measured on
snapshots every 0.1 s on the oracle and every frame here:

| | oracle | runtime |
|---|---|---|
| the logo fades in | 16.4-17.1 s (0.7 s) | 16.20-16.90 s (0.7 s, a step every 3 frames: 50 ms) |
| the logo stays | 17.1-21.2 s (4.1 s) | 16.90-21.00 s (4.1 s) |
| the logo fades out | 21.2-21.9 s (0.7 s) | 21.00-21.70 s (0.7 s) |
| black, the title's files loaded | 21.9-23.4 s (1.5 s) | 21.70-21.83 s (0.13 s) |
| the title fades in | 23.4-24.3 s (0.9 s) | 21.83-22.65 s (0.8 s) |

The same pace; the runtime is ahead only where MAME reads its CD at the
drive's speed. ABEL's main loop is the CPU's (`03-executables.md`); its
calibration is `05-open-questions.md` 1.

## The window

`build/rt/abel` with no `--headless` opens the window (SDL3) and runs at the
guest's real time. Played by the user (session 2) from the title into the
game: the walk "very fast" against a video of the game (as on MAME), the
dialogue's text at the right speed.

## The first two games on this kit

Their C++ regenerated byte for byte the same. Run 60 s each on the published
runtime (their submodule, e4c5334) and on this one: The Queen of Duellist's
frames, sound and log the same; 4D Driving's 60 frames the same, its sound
different between 8.9 and about 20 s only (the RMS of the difference 45-239
against 1,973) and the same after: its TBIOS's sound module reads port 0x5ED
and, when bit 7 is clear, waits 48 us after writing a chip's register; the
kit answered 0 there, MAME answers 0xFF (read on the oracle), so the wait is
gone, as on MAME.
