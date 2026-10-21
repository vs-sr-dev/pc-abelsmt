# 10 - fmtownskit

The kit this port is built on, and what this port has brought to it. The
kit's own README lists every module and its checks; this file is the log.
fmtownskit was born with the first game (pc-thequeenofduellist) and grew
with the second (pc-4ddriving); this is its third.

## Session 1

The submodule was added at the kit's last commit, **e4c5334** (4D
Driving's session 4, the published kit); the changes below are the kit's
commit **2b785bc**, pulled into it at the session's end.

| Change | What this game asked of it |
|---|---|
| `exp` | **a P3 bound behind an MZ stub**: an MZ file whose header's page count ends where a P3 begins is read as that P3, every offset counted from it (`Exp.stub`, the stub's length; shown as "bound"). ABEL's three programs carry Fujitsu's "exp2exe" stub, 0x850 bytes |
| `tbiosld` (new) | **a TBIOS in modules**: TBIOS.INI's files laid out as TBIOSLD.SYS does -- the first as it is at 0, each Phar Lap "MQ" image after it at the next 16-byte boundary, relocated; each module's records linked into the entry table (an export sets its slot and keeps the old pointer, an import takes the slot's pointer); the data pointers at 0x200. `--out` writes the image, `--check` holds it against a TBIOS read from memory |
| `x86map` | **far calls told from switches**: a far transfer is FF /3 or FF /5 (capstone's operand size does not say), so MetaWare's `jmp cs:[reg*4 + table]` (FF /4) is no longer listed as a far call -- the pre-analysis' "dozens of `call far cs:[ip+8]`" were these; a far `jmp` listed as such. **A compare's bound is dropped when the index is rewritten** between the compare and the jump (`cmp al, 0xF0; jae; ...; sub ebx, 0x80; shr ebx, 4`: read with 240 entries into code, whence the "undecodable" addresses), a widening (`movzx`, `cwde`) aside |
| `x86map`, `recomp.discover` | **signed bounds**: `cmp idx, N; jg` (or `jge`) bounds a table from 0 as `ja` does (ABEL's two tables of 21 handlers, read before into each other and the data) |

Checked:

* `exp` on the three programs: images of 299,936, 648,423 and 170,187
  bytes, entries 0x27D14, 0x40D20, 0x11444, image + minimum = ESP on each;
  the image read from behind the stub the same, byte for byte, as the P3
  cut out by hand in the pre-analysis. 4D Driving's and the first game's
  .EXP read as before.
* `tbiosld` on ABEL's TBIOS.INI: 90,464 bytes, the modules where the
  oracle's memory has them; against that memory (66 s in), the entry table
  the same but for slot 0x98 (the game's), and the 449 other bytes that
  differ all in the modules' work areas, none in code.
* `x86map`: on `ABEL.EXE` 228 "far calls" before, 34 now, all through
  TBIOS's table at fs (194 switches through CS no longer among them); no
  undecodable addresses; 1,560 functions. On the first two games the same
  maps but for two MetaWare switches in 4D Driving's library, listed before
  as far calls.
* `recomp.discover`: every jump table of the first two games the same,
  entry for entry (a dump of all of them before and after); `ABEL.EXE`
  1,522 functions, 199 tables (192 bounded), 0 refused; the emitter's input
  for the first two games unchanged, so their C++ is.

## Session 2

The kit's commit **f5cc3cf**, on 2b785bc, pulled into the submodule at the session's end: what running OPNG and ABEL asked of it.

| Change | What this game asked of it |
|---|---|
| `recomp.discover` | **a table bounded by `cmp; ja; je`**: N itself goes elsewhere on the same flags, so the table has N entries (TBIOS's sprite module, 0x15B20: read as 3, its third entry code bytes) |
| `recomp.discover`, `recomp.emit` | **a call made by hand**, `push RET; jmp F`: F a function, RET the code that goes on; the C++ calls F, checks the return, goes on at RET (TBIOS's graphic module, five pairs; none in the other seven programs read). Before, F's `ret` unwound the program to its entry |
| `tbiosld --seeds` | the code a TBIOS's modules' records point at, for the recompiler: each export's entry, each old pointer a record keeps (a module chains through its record: the mouse module calls the int 04dh module's System entry) |
| `runtime`: `parse_p3`, `compose_tbios` | a P3 behind an MZ stub, read by the runtime as `exp` reads it; TBIOS composed from the disc's TBIOS.INI and modules, as `tbiosld` composes it (the same image, byte for byte, its CRC the recompiled module's) |
| `runtime`: `boot_towns`, `run386_load`, `run386_end`, `x86_park` | **programs one after another** on one machine: TBIOS loaded once and left with its work areas, each program in a fresh segment, what its end undoes (files, protected-mode vectors, timers); several modules under one code selector, the ones not running parked. `boot_run386` is the first two together, as before |
| `runtime`: `Machine::int97` | **the timer BIOS**, int 97h AH=00h-02h, read on the oracle: timers of 10 ms ticks with a limit and a far callback, called at a safe point with IF set |
| `runtime`: ports | **0xFF where MAME's io map has no reader** (its `unmap_value_high`), and at the odd bytes of a range under `umask32(0x00ff00ff)`: an absent MIDI card's ports, 0x470, 0x5ED, the CMOS's odd bytes -- read on MAME from Lua. 0xFD91-0xFD97's odd bytes read 0, as MAME's reader gives |
| `runtime`: the palette | **MAME's colour registers**: one r, g, b per entry whichever palette is chosen, a component written setting the chosen palette's whole colour (ABEL's throne room) |
| `runtime`: DOS | 2Ah, 2Ch (the host's date and time); 44h AL=00h on handles 0-2 (the console, 80D3h); 40h to handles 1 and 2 (shown on the host's terminal); names cut to 8.3; the CD's current directory (`cd_dir`) for relative names |

Checked:

* ABEL's three programs and TBIOS: discovery and the C++ (`09-recompiler.md`),
  64,131 self-test vectors, 0 failures; OPNG's logo and title, 28 snapshots
  of 28, and ABEL's throne room the oracle's pixel for pixel (`11-runtime.md`).
* The first two games: every jump table the same, entry for entry, in all
  eight programs the kit has read (but TBIOS's sprite table above); their C++
  regenerated byte for byte the same; 60 s of each on the published runtime
  and on this one: The Queen of Duellist's frames, sound and log the same, 4D
  Driving's frames the same and its sound different between 8.9 and about 20 s
  -- its TBIOS's sound module waits 48 us after a chip's register when port
  0x5ED's bit 7 is clear, and 0x5ED now reads 0xFF, as on MAME.
