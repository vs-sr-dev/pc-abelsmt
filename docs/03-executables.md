# 03 - The executables

## Three programs, each bound behind a stub

`OP\OPNG.EXE`, `ABEL.EXE` and `ED\ENDG.EXE` (run in that order by `START`,
`01-disc-layout.md`) are each an MZ program of 0x850 bytes -- the same in
all three, byte for byte: Fujitsu's **"exp2exe"** stub ("exp2exe : DOS
Extender が見つかりません."), which looks for `EXE386.EXE` or `RUN386.EXE` on
the PATH and runs it on its own file -- with a Phar Lap **P3** right behind
it, where the MZ header's page count ends (5 pages, 80 bytes in the last).
Every offset in the P3 counts from its own start. `fmtownskit.exp` reads
such a file since this session (`10-fmtownskit.md`):

`python -m fmtownskit.exp build/disc/ABEL/ABEL.EXE` (and the others):

| | `OPNG.EXE` | `ABEL.EXE` | `ENDG.EXE` |
|---|---|---|---|
| file | 302,576 | 651,063 | 172,827 |
| image (at P3 + 0x200, loaded at 0) | 299,936 (0x493A0) | 648,423 (0x9E4E7) | 170,187 (0x298CB) |
| entry | 0x27D14 | 0x40D20 | 0x11444 |
| initial ESP | 0x693A0 | 0xBE4E8 | 0x498CC |
| extra memory, minimum / maximum | 0x20000 / 0xFFFFFFFF | 0x20001 / 0xFFFFFFFF | 0x20001 / 0xFFFFFFFF |
| stack size (header 0x7C) | 0x20000 | 0x20000 | 0x20000 |

Image + minimum = the initial ESP on all three, as on the other two discs.
Level 1, no relocations, no segment table, **no symbol table**: the file ends
where the header says. All three are **MetaWare High C** ("MetaWare High C
Run-time Library", "MetaWare Incorporated" in each), with Fujitsu's libraries
("Copyright (C) FUJITSU LIMITED 1991").

## ABEL.EXE

### Layout

| Range | What |
|---|---|
| 0x00000-0x48199 | compiled code: the game from 0, MetaWare's library and the Towns' libraries after it (the start at 0x40D20, `int 21h` wrappers, the TBIOS calls from 0x453B2, the sound and MIDI drivers, to 0x48199) |
| 0x48199-0x9CF08 | data: the tables, the menus' and the items' text (`02-data-formats.md`) |
| 0x9CF08-0x9E3DC | hand-written code (*inferred*: an assembly module linked last): `inp`/`outp`-style helpers that take the port and value from the stack (0x9CF08 `in al, dx`, 0x9CF1B `out dx, al`...), the CRTC and palette routines, the sprite RAM and VRAM blitters (selectors 0x114, 0x104, 0x10C; the VRAM write mask at 0x458-0x45B) |
| 0x9E3DC-0x9E4E7 | data |
| to 0xBE4E8 | the uninitialized data and the 128 KB stack |

### Discovery, read

`python -m fmtownskit.recomp.discover build/disc/ABEL/ABEL.EXE --report`,
from the entry alone (no symbols to seed it):

| | functions | instructions | bytes of code | jump tables (bounded) | indirect left | refused | conflicts |
|---|---|---|---|---|---|---|---|
| `OPNG.EXE` | 605 | 20,232 | 66,588 | 20 (15) | 16 | 0 | 0 |
| `ABEL.EXE` | 1,522 | 81,184 | 253,315 | 199 (192) | 12 | 0 | 4 |
| `ENDG.EXE` | 451 | 14,582 | 48,511 | 14 (9) | 18 | 0 | 0 |

* **The four conflicts are the code's own.** At 0x22A09 a seven-byte copy
  is `movsd; movsw; movsb` (`A5`, `66 A5`, `A4`), and another path jumps to
  0x22A0E, inside `66 A5`, where `A5` alone is a `movsd`: a five-byte copy,
  `movsd; movsb`, sharing the tail (MetaWare's merged string moves; four
  such, at 0x22A0D, 0x22C89, 0x22EE1, 0x23149). Both readings are real
  code, and the recompiler must emit both (`05-open-questions.md` 4).
* **Two tables are bounded by signed compares**, `and esi, esi; jle ...;
  cmp esi, 0x14; jg ...` at 0xCB2D and 0xCB58: 21 handlers each, the second
  table right after the first (0x699D8, 0x69A2C = 0x699D8 + 21 x 4). Read
  with no bound before this session, they ran into each other and into the
  data (45 and 24 entries); the kit reads `jg`/`jge` now (`10-fmtownskit.md`).
* MetaWare's `switch` is a near `jmp cs:[reg*4 + table]` with its table
  placed right after the jump (194 such in `ABEL.EXE`); x86map had listed
  them as far calls (the pre-analysis' "call far cs:[ip+8]"). They are
  jump tables, read and bounded by their `cmp`/`ja`.
* The image starts with code: the function at 0 is called five times
  (`call 0` at 0xDD5, 0xE1C, 0x104C, 0x9A6E, 0x9B8D). With the image at base
  0, a table read with no bound can take small numbers for addresses: the
  table at 0x8CCB4 (`call [eax*4 + 0x8CCB4]` at 0x1F03D, the index an item's
  number) has 49 handlers, then data (1, 1, 1, 0x39...), and discovery keeps
  52 of them, a "function" at 0x1 among them (harmless: code that no one
  calls; `05-open-questions.md` 4).
* The 12 indirect transfers left: `call eax`, `call edi`, calls through the
  arguments (`[ebp + 8]`: MetaWare's `qsort`-style callbacks, *inferred*) and
  through a variable (`[0x9C2EC]`, in the MIDI driver).

### What it touches

`python -m fmtownskit.x86map build/disc/ABEL/ABEL.EXE` (1,560 functions
from the entry):

| What | Where |
|---|---|
| DOS (int 21h) | 06h, 09h, 19h, 1Ah, 2Ah, 30h, 42h, 4Ah, 4Eh, EDh (AL=00h); everything else through MetaWare's library, AH in a register |
| Phar Lap (21h AH=25h) | 02h/03h get and **06h** set the protected/real-mode vectors of **int 4Dh** (IRQ 13: the sound chips' interrupt, to TBIOS's handler at fs:[0x1A8], `SND_init`'s way, as 4D Driving), **44h and 45h** (IRQ 4 and 5: the MIDI driver, below); **07h** to put them back |
| int 8Eh | AX=CF00h, four sites (KimG, as 4D Driving's `KimG_*`) |
| int 93h | AH=02h only: **no CD-DA in the game** (the opening's and the ending's programs have 50h, 52h-56h and 14h) |
| int 97h | AH=01h, 02h (FORRBIOS.NSD answers it, `01-disc-layout.md`; what it is, `05-open-questions.md` 7) |
| TBIOS, far calls | fs:[0x20] the graphic module, AH 00h-07h, 0Ah, 0Ch, 0Eh, 18h, 19h, 20h, 24h, 25h, 2Bh, 46h, 60h; fs:[0x80] the sound module; fs:[0x1A0] the warikomi (interrupts) module, 03h, 04h, 06h, 07h; fs:[0x1C0] the System slot, 02h, 30h, 31h. Not the mouse (0x40), the sprite (0x60) or the font (0xA0) modules: the sprites by sprite RAM directly (selector 0x114) |
| ports | the pad's 0x4D0/0x4D2 and its mask 0x4D6 (through the helpers: the pad routine at 0x3920 writes 0x3F to 0x4D6 and reads both pads); the CRTC 0x440/0x442; the palette 0xFD90-0xFD96; the VRAM write mask 0x458/0x45A/0x45B; the master PIC's mask (0x02); **0xE70/0xE71 and 0x4A0/0x4A1** (below) |
| selectors | 0x14 its data, 0x104 VRAM layered, 0x10C VRAM single-layer, 0x110 TBIOS, 0x114 sprite RAM, 0xF8 (MetaWare's Weitek probe, as on 4D Driving) |

**The MIDI driver** (around 0x45CB0-0x45FE1, and 0x473C6): it hooks IRQ 4 and 5,
unmasks IRQ 4 (bit 0x10 of the master PIC's mask), and sends bytes from a
ring buffer to port **0xE70**, with 0xE71 for control and status -- and to
0x4A0/0x4A1 as well when a flag says so (*inferred*: a MIDI card, two
models or two ports). MAME's Towns has nothing at those ports. Its parser
of the score's status bytes (0x474B4: 0x80-0xEF by channel, `(b - 0x80) >>
4` an index; 0xF0 and up apart; 0xFD, 0xFE skipped) is the Euphony
player's (*inferred*). On the oracle, while walking, the program reads 0xE71
about 470 times a second.

### The start and the main loop, on the oracle

RUN386 puts the program (`--dump 66`, in the game; `fmtownskit.mamedump`):
selectors 0x0C/0x14 at linear **0x408000** (physical 0x4F000), limit
0x377FFF -- 4D Driving's place on the same machine, its physical address
0x6000 higher (a larger IO.SYS, the TBIOS loader, GDS.SYS); FORRBIOS.NSD at
0x400000 (0x4C/0x54, physical 0x3F8000, 32 KB); TBIOS at 0x100000 (0x110,
limit 0x16FFF); selector 0x40 the RAM to 0x3F7FFF.

**The game's pace is its CPU's** (*inferred*, from the oracle's instruction
traces: MAME's debugger, `tools/oracle.py --debug --lua`, loops folded).
Standing still 70 s in, the main loop (0xE65-0x10DB) reads the pad (0x3920)
and compares the position, about 32,000 times a second, waiting on nothing.
Walking (a second's trace), the time goes to drawing -- the map's tiles by
rows (0x9E005: a tile's 16 rows into VRAM through FS, 256 bytes apart), TBIOS's graphic module's
block moves (`rep movsd` at TBIOS 0xC34F) -- and no loop waits on a timer, a
counter or the vertical blank (no read of 0xFDA0 or of the CRTC's status).
The YM3438's timer interrupt runs TBIOS's sound handler about 290 times a
second (0x4D8, 0x4E9 read), the music's clock. So the walk, the scrolling
and the text's speed are as fast as the CPU draws: the reason, *inferred*,
for MAME's "Runs too fast" (`05-open-questions.md` 1).

## TBIOS, in modules

CONFIG.SYS loads `\TBIOS\TBIOSLD.SYS`, which reads `TBIOS.INI` and lays the
modules out in the segment of selector 0x110 (base 0x100000, as the other
discs' one-file TBIOS). Every module but the first is a Phar Lap
relocatable image, **"MQ"**: an MZ header with MQ for its signature, u32
relocations (bit 31: a 32-bit fixup; all of them here). Composed from the
disc's files by `fmtownskit.tbiosld` and held against the TBIOS in the
oracle's memory 66 s into the game (`mamedump --read 0x100000:0x17000`):

| INI | File | Module (its header) | At | Exports (slot = entry) / imports |
|---|---|---|---|---|
| BIN | `TBIOSTOP.BIN` (768) | V31L35 93/10/15 "tbios": the entry table, its defaults (small `retf` stubs at 0x2E0-0x2F0) | 0x00000 | -- |
| EGB | `EGB_MR.BIN` (72,993) | V31L31 94/02/03 graphic | 0x00300 | 0x20 = 0x400, 0x1C0 |
| SND | `SND_STD.BIN` (14,557) | V31L31 19931018 sound | 0x10E30 | 0x80 = 0x11270, 0x1C0 |
| INT | `INT_4D.BIN` (2,312) | V31L35 19931019 int 04dh | 0x14310 | 0x1A0 = 0x14460, 0x1A8 = 0x146DC (the int 4Dh handler), 0x1C0; imports 0x80 |
| MOS | `MOS_STD.BIN` (3,747) | V31L35 19940112 mouse | 0x14A20 | 0x40, 0x48, 0x1C0 = 0x15564; imports 0x20, 0x1A0 |
| MIO | `MIO_STD.BIN` (1,392) | V31L35 19930805 mouseI/O | 0x156D0 | 0x50; imports 0x1C0 |
| SPR | `SPR_STD.BIN` (1,504) | V31L35 93/10/23 sprite | 0x15A40 | 0x60 |
| FNT | `FNT_STD.BIN` (1,344) | V31L35 93/07/14 font | 0x15E20-0x16160 | 0xA0 |

A module's header: 'V31Lnn', its date, 'Townes', its name; +0x24 a pointer
to its data; +0x28 its code and data selectors (0x110, 0x118); +0x2E a count
of records at +0x30, `u32 slot, u32 offset` -- an **export** sets the entry
table's slot to (offset, 0x110) and leaves the slot's old pointer in the
record (a module chains to what it replaced: four modules export 0x1C0, the
System slot -- graphic, sound, int 04dh, mouse -- and the last, the mouse's,
is what the program calls); an
**import** (bit 31) leaves the slot's pointer in the record. At 0x200 of the
table, the data pointers of the five modules that have one. The composed
image is the memory's **byte for byte in every module's code**; of its
90,464 bytes, 454 differ: slot 0x98 (the game writes 0014:00050138 there) and
the modules' work areas, each between its header and its first code. So the
port needs no TBIOS read from the oracle: it composes it from the disc, as
TBIOSLD does.

The slots are the one-file TBIOS's: 0x20 graphic, 0x40/0x48 mouse, 0x50
mouse I/O, 0x60 sprite, 0x80 sound, 0xA0 font ("ま～くん" on 4D Driving's), 0x1A0/0x1A8
warikomi, 0x1C0 System, 0x1E0/0x1E8 the font ROM (selector 0x138). The
modules are older or newer builds of what the first two games' `TBIOS.BIN`
(V31L31 92/11/11) holds; how far they differ is for the recompiler's
sessions.

## OPNG.EXE and ENDG.EXE

The same libraries as `ABEL.EXE` (the same TBIOS calls, the same int 4Dh,
44h, 45h hooks, int 8Eh and 97h), and the CD BIOS's audio (int 93h AH=50h,
52h-56h, 14h) with its library's messages ("繰り返し演奏を開始しました"...).
`ENDG` is run with one argument (429266408).
