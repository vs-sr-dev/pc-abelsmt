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
