# 09 - The recompiler

The programs' code and TBIOS's, recompiled from 386 to C++ by fmtownskit
(`python -m fmtownskit.recomp`), as the first two games'. Their own
`docs/09-recompiler.md` have the decoder's, the interpreter's and the
emitter's checks; this file is what this disc asked.

```sh
sh tools/recomp.sh          # KIT=D:/Homebrew6 for a kit being worked on
```

`tools/recomp.sh` composes TBIOS from the disc's modules (`fmtownskit.tbiosld
--out`) and runs the recompiler on three modules: **OPNG and ABEL under code
selector 0x0C** (one at a time at run time, `11-runtime.md`), **TBIOS under
0x110** (`--raw`, its far-pointer table at 0x20-0x1E0, the font ROM's
pointers after it left out, as 4D Driving's). No program has symbols: the C++'s
names are addresses, and the seeds below are what discovery could not find
from the entries.

## Discovery

| | functions | instructions | jump tables (bounded) | indirect left | refused | conflicts |
|---|---|---|---|---|---|---|
| OPNG | 730 | 24,424 | 22 (17) | 38 | 0 | 0 |
| ABEL | 1,612 | 84,422 | 202 (195) | 38 | 0 | 4 |
| TBIOS (composed) | 686 | 21,592 | 51 (13) | 78 | 0 | 1 |

ABEL's four conflicts are its overlapping string moves (`03-executables.md`);
TBIOS's one is at 0x836E (`test`, with an `or` at 0x8370 inside it), *not
looked at*. **OPNG's code between 0x2E00 and 0x1E238** -- more than half of it
-- is reached by no call, table or pointer discovery or this session's scans
found; the whole opening ran without needing it (*not known*: the HDD
install's, the other menu items', or never run).

## The seeds

Every seed was met the same way: the runtime stops at "no recompiled function
at CS:EIP", the address is looked for in the image, and the rule that finds
it is applied to the whole image, not just that address. The rules, and what
they gave:

* **The records of TBIOS's modules** (`tbiosld --seeds`, new this session):
  each export's entry and each old pointer a record keeps when it is TBIOS's
  own code. A module chains to the entry it replaced through its record, not
  the table: the mouse module calls the int 04dh module's System entry
  (0x149CC) so. 18 entries, the default stubs at 0x2E0-0x2F0 among them.
* **MetaWare's nested helpers and stored handlers** (OPNG's `push 0x2A51C;
  call`, `mov [0x46C90], 0x24C74`): immediates that point at a function's
  prologue (`push ebp; mov ebp, esp`). OPNG 8, ABEL 8.
* **The timer BIOS's callbacks** (`11-runtime.md`): pushed before `lea eax,
  [ebp - 0xA]` when the library fills its packet. OPNG 8 (0x1D0, 0x21AB8...),
  ABEL 11; and each program's eight slot trampolines (`push n; jmp`, 10 bytes
  apart: OPNG 0x2F937, ABEL 0x47E83).
* **Code right after a `ret`**, with MetaWare's padding between (`mov eax,
  eax`, `mov ax, ax`, zeros, nops), pointed at by an immediate or by a dword
  in the data: OPNG's table of eight handlers at 0x46218 (0x21E44...), 0x2180;
  ABEL's 0x3F740, 0x46198 (a `ret` stored as the MIDI driver's "none"),
  0x46440, 0x46658, the far stubs 0x45B50, 0x45B9C, 0x45BE8, two pointers into
  padding (0x3241, 0x7282: the functions after it, padding run). OPNG's IRQ 4
  and 5 handlers given to Phar Lap's 2506h (0x2DAF4, 0x2DB88), 0x2D84C, 0x2DD0C,
  and its far trampolines `call [var]; retf` (0x2CEA0, 0x2CEEC, 0x2CF38).
* **TBIOS's graphic module's frame slots**: it keeps routines in its frame
  (`mov gs:[ebp - 0x18], 0x5BF4` ... `call gs:[ebp - 0x18]`) and loads them
  from tables (`mov eax, cs:[eax*4 + 0x7B14]`): every table so read, to the
  first value that is not code -- 0x34BC, 0x5234, 0x5404, 0x55D8, 0x6158,
  0x7D50, 0xADD8, 0xB00C, 0xE7C8, 0x7B14 (22 entries, also a jump table at
  0x7B09) and 0x678C (128 entries, indexed by `eax + ecx*4`, the targets of
  `jmp gs:[ebp - 0x28]`: entry points into unrolled loops); the tables 0xAF58
  and 0xB22C are fill masks, left out. The routines passed in tables (`push
  4; push 0x7708; jmp 0x7604`). Immediates moved into a register that is then
  called (`mov esi, 0x14A0E` ... `call esi`) or that land right after a `ret`
  or `jmp`; the sound and int 04dh modules' handlers and their "none" (`retf`
  at 0x1448C, `ret` at 0x14902). 203 seeds besides the records.

The scans are throw-away scripts of this session (*not* in the kit yet): what
they found is the list in `tools/recomp.sh`, with where each group comes from.

## What the kit learned (`10-fmtownskit.md`)

* **A table bounded by `cmp; ja; je`**: TBIOS's sprite module at 0x15B20,
  `cmp al, 2; ja out; je elsewhere`, then `jmp cs:[ebx*4 + 0x15B68]` -- 2 goes
  elsewhere, so the table has 2 entries, not 3 (read as 3, its third was code
  bytes, 0xEF77013C, "outside the image"). `recomp.discover` takes the `je`
  on the same flags.
* **A call made by hand**, `push RET; jmp F` (TBIOS's graphic module: `push
  0x41B4; jmp 0x5FA4`, F's `ret` coming back to 0x41B4): read as a jump, F's
  `ret` returned where no C++ frame expected it and unwound the program to its
  entry ("the program returned from its entry", 36 s into OPNG). Discovery now
  makes F a function and RET code that goes on; the emitter calls F's C++,
  checks the return and goes on at RET. RET must be code already (an
  instruction discovery has) or start right after a `ret`, `retf` or `jmp` --
  not the pair's own `jmp`: `push 0x7708; jmp 0x7604` passes a table placed
  after it -- and not below 0x100 (a compiler's `push 2; jmp` to where its call
  happens pushes an argument). In all eight programs the kit has read, these
  are TBIOS's five pairs and nothing else.

## The C++, its build, the self-test

* 15 files for OPNG (3.9 MB, 100,000 lines), 48 for ABEL (11.8 MB, 319,000),
  24 for TBIOS (5.7 MB, 150,000); clang -O2 builds the port from nothing in
  **31 seconds** on 16 threads, every file at the first try.
* **The self-test** (`sh tools/recomp.sh --out build/stest --optest
  --selftest 3 --jobs 16`, 3 minutes; its `selftest` built from
  `build/stest`): **64,131 vectors replayed on the C++, 0 failures** --
  the optest's 59,314 (every distinct instruction of the three modules and a
  random sweep), OPNG's 906, ABEL's 2,568, TBIOS's 1,343 (24, 11 and 35 stop
  at the machine in the C++, as expected of calls into TBIOS and the ports).
* The first two games regenerated with this kit: their C++ byte for byte the
  same (`11-runtime.md`).
