# 01 - The disc

## Which disc this is

| | |
|---|---|
| Title | ＡＢＥＬ 真・黙示録大戦 / ABEL - Shin Mokushiroku Taisen ("Cyber Fantasy ABEL" on its pictures), a science-fiction RPG seen from above |
| Publisher | Family Soft (ファミリーソフト) |
| Platform | FM Towns, CD; MAME's list: "Requires 4 MB RAM" |
| Serial | **MTC-1137** |
| Release | the list says 1995-11 |
| Volume | `ABEL`, prepared with "MERIDIAN_DATA_CD_PUBLISHER", created and modified 1995-12-05 03:29:04 |
| The programs | three, each a Phar Lap P3 bound behind a DOS stub, MetaWare High C (`03-executables.md`) |

Checked: every track's size, CRC32 and SHA-1, and the cue sheet's, are the
ones MAME's software list (`fmtowns_cd.xml`, entry `abel`, origin
redump.org) gives -- 21 of 21 (`build/preanalysis/verify_disc.py`); and the
CHD that `tools/oracle.py` made from the cue sheet has the list's SHA-1
(`d4a49a59b582c870bb3ab843ffc3685c29efef86`; the tool checks it as it makes it).

MAME's list marks the entry `supported="partial"`, and the comment on the
line above it says why: **"Runs too fast"** (`05-open-questions.md` 1). The
same comment stands over four other entries of the list, and "Runs too fast?"
over Family Soft's own *Azure* (1993).

On the oracle (`fmtowns -ramsize 4M`, the default of `tools/oracle.py`) the
disc boots: TownsOS ("TownsOS V2.1 L31", and "ユーザ設定ファイルの読み込みに失敗しました",
the user settings file it does not find on a CD), Family Soft's logo at 20 s,
the title at 25 s with its menu (CD PLAY, HDD PLAY, HDD INSTALL, OPENING).
Left alone, the title goes after 40 s to the logo again and to the intro, and
the intro runs **about 11 minutes** (the user's count; on the oracle the title
comes back at 740 s, and the whole runs again): the prologue's red text, the
leaders' dialogue, the staff's titles, the story, the characters, a last
picture -- scored with the CD's tracks 2 to 10 (below). The logo and the
title are silent. (The pre-analysis' first run played the prologue with FM
music instead, and no CD: `05-open-questions.md` 8.) CD PLAY then GAME START
(`--input 30-30.2:A,38-38.2:A`): "ABEL Ver 1.00 System Loading" at 50 s, and
at 60 s the game: a throne room, ABEL's card (LV 01, HP 175, SP 24), two empty
places in the party; the pad walks him, and a first dialogue opens.

## The tracks

`python -m fmtownskit.disc "disc/Abel - Shin Mokushiroku Taisen (Japan).cue"`:

| Track | Mode | First sector (INDEX 01) | Sectors | Length |
|---|---|---|---|---|
| 01 | MODE1/2352 | 0 | 19,678 | 4:22.37 |
| 02 | AUDIO | 19,828 | 4,650 | 1:02 |
| 03 | AUDIO | 24,628 | 13,350 | 2:58 |
| 04 | AUDIO | 38,128 | 1,800 | 0:24 |
| 05 | AUDIO | 40,078 | 3,075 | 0:41 |
| 06 | AUDIO | 43,303 | 3,750 | 0:50 |
| 07 | AUDIO | 47,203 | 4,500 | 1:00 |
| 08 | AUDIO | 51,853 | 4,200 | 0:56 |
| 09 | AUDIO | 56,203 | 2,775 | 0:37 |
| 10 | AUDIO | 59,128 | 825 | 0:11 |
| 11 | AUDIO | 60,103 | 2,475 | 0:33 |
| 12 | AUDIO | 62,728 | 825 | 0:11 |
| 13 | AUDIO | 63,703 | 1,125 | 0:15 |
| 14 | AUDIO | 64,978 | 2,775 | 0:37 |
| 15 | AUDIO | 67,903 | 3,225 | 0:43 |
| 16 | AUDIO | 71,278 | 14,775 | 3:17 |
| 17 | AUDIO | 86,203 | 5,175 | 1:09 |
| 18 | AUDIO | 91,528 | 12,075 | 2:41 |
| 19 | AUDIO | 103,753 | 11,850 | 2:38 |
| 20 | AUDIO | 115,753 | 16,800 | 3:44 |

* Every audio track has a pregap of 150 sectors (2 s), in its own file
  (INDEX 00 at 0, INDEX 01 at 2 s), **silent** (peak 0 to 16); every audio
  track is a whole number of seconds.
* The ISO volume is 19,528 blocks of the data track's 19,678.
* Who plays them: the opening and the ending programs (`OPNG.EXE`,
  `ENDG.EXE`) call the CD BIOS's audio functions; the game (`ABEL.EXE`) does
  not, and its music is the FM and PCM chips' (`03-executables.md`,
  `02-data-formats.md`). On the oracle, OPENING chosen at the title plays
  **track 02** from 53.9 s (the CD's PLAY command, 0x24, from 04:26:28 to
  05:30:27, BCD: LBA 19,828 to 24,627), and MAME's output is that track
  (the loudness envelopes at 100 Hz, correlation 0.984 over 18 s).
* **The intro is tracks 2 to 10, in order**, each played when the last one
  ends, give or take seconds (the oracle left alone 900 s, `--sound-log`;
  each a stop, 0xA4, then a play, 0x24):

  | Play at | Track | Length | The intro's part |
  |---|---|---|---|
  | 68.4 s | 02 | 1:02 | the prologue's red text |
  | -- | -- | -- | 132-235 s: the leaders' dialogue, no CD |
  | 235.0 s | 03 | 2:58 | the staff's titles |
  | 417.1 s | 04 | 0:24 | "21世紀初頭..." in red |
  | 452.9 s | 05 | 0:41 | the Earth, the fortress |
  | 496.1 s | 06 | 0:50 | the battle |
  | 550.7 s | 07 | 1:00 | the characters' faces |
  | 612.8 s | 08 | 0:56 | a girl in space |
  | 674.7 s | 09 | 0:37 | Abel's face |
  | 713.9 s | 10 | 0:11 | the last picture; a stop at 734.0 s, the title at 740 s |

  So the intro's length is the CD's, on MAME as on a Towns. Tracks 11-20
  (*inferred*) are the ending's.

## The data track

661 files in 67 directories (`build/disc` after `--extract`). Sector 0 is
the IPL (`IPL4`, then a jump to +0x5B), as on the other two discs.

The boot chain, as the files say it (the version strings read in each):

| Step | File | What it is |
|---|---|---|
| 1 | sector 0 | the IPL |
| 2 | `IO.SYS` (110,592) | Towns-DOS, "BIOS Version 3.58" **1994-02-02** (the other two discs: 77,824 bytes, 1992-12-15) |
| 3 | `CONFIG.SYS` | `SHELL=C:\COMMAND.COM C:\ /P`, `DEVICE=\SYS\CTL.SYS`, **`DEVICE=\TBIOS\TBIOSLD.SYS`**, `DEVICE=\TOWNS.SYS`, `DEVICE=\SYS\GDS.SYS`, `BUFFERS=8`, `FILES=30` |
| 4 | `TBIOS\TBIOSLD.SYS` + `TBIOS.INI` + eight modules | "TownsBIOS Loader Version3.0", 1993.12.06 ("All programmed by Y.Miyazawa"): **TBIOS in modules**, V31L35 93/10/15, laid out by the loader at selector 0x110 (`03-executables.md`) |
| 5 | `SYS\GDS.SYS` | "V31L22-01 890118", "グラフィックス機能（Towns)", Fujitsu 1989: *inferred* the graphics driver of the DOS side; not on the other discs |
| 6 | `AUTOEXEC.BAT` | `C:MSCDEX /D:TOWNS_CD /M:8 /L:Q`, `\HCOPY\COCO`, `\HCOPY\COCO_MEM`, `\run386 \hcopy\adddrv \hcopy\forrbios.nsd`, then **`START`**; on an error, `\T_TOOL\DELVDISK` and `\T_TOOL\REIPL` |
| 7 | `HCOPY\COCO.EXE`, `COCO_MEM.EXE` | "CoCo (Code of Co-native os) Version 1.8 E04", "Memory Requester Version 1.8 E00": the version strings of 4D Driving's, the same sizes, other bytes (3,876 and 146 differ) |
| 8 | `HCOPY\ADDDRV.EXP` + `FORRBIOS.NSD` | ADDDRV **4D Driving's, byte for byte**; `FORRBIOS.NSD` another build (28,772 bytes against 28,644). Read running (the IDT 66 s in): it answers int 8Eh, 90h, 91h, 93h, 96h, 97h in protected mode, at 0054:0224, 0244, 024C, 0254, 0264, 026C |
| 9 | `RUN386.EXE` (78,064) | "386\|DOS-Extender Version 1.2aJ **L51**" (the other discs: L50, 78,496 bytes) |
| 10 | `START.EXE` (2,691) | a compiled batch file ("BATCP"), below |

`START.EXE` is an MZ program that decrypts its own script (image offset
0x98C, file offset 0x9AC, 0xD7 bytes; each byte b becomes `(b ^ cl) + 0x5E`,
CL the low byte of a count from 0xD7 down: its loop at image 0x3DC), writes
it to `BATCPTMP.BAT` and runs it with `COMMAND.COM /C`. The script, decrypted
(`build/preanalysis/startbat.py`):

    cd \abel\op
    Q:\ABEL\OP\OPNG
    cd \abel
    if errorlevel 1 goto LOAD_START
    Q:\ABEL\ABEL
    goto TERM
    :LOAD_START
    Q:\ABEL\ABEL LOAD
    :TERM
    if errorlevel 1 goto EXIT
    q:
    cd \abel\ed
    Q:\ABEL\ED\ENDG 429266408
    :EXIT

So **three programs**, one after the other: `OPNG` (the logo, the opening, the
title and its menu) ends with an errorlevel that says new game or LOAD;
`ABEL` is the game; when it ends with errorlevel 0 (*inferred*: the game
finished), `ENDG` shows the ending, given the number 429266408 (*inferred*: a
key that the ending is not run by hand). The title's HDD PLAY and HDD
INSTALL copy the data to drive D: ("Ａボタンを押すとデータファイルをＤドライブにインストールします", "２０ＭＢ以上の空きが必要です" in `OPNG.EXE`).

Of the other system files: `SYS\CTL.SYS`, `TOWNS.SYS`, `HCOPY\COCO_END.EXE`,
`SYSINIT\SYSTEM.FNT/.ICO/.SKB` and the three `T_TOOL` tools are 4D Driving's
byte for byte (`build/preanalysis/syscmp.py`); `SYSINIT\SYSINIT.EXE` ("FM
Towns システムデータローダ V1.0 L22", 1991-1993) is there, but `AUTOEXEC.BAT`
does not run it.

Everything else is the game's, under `ABEL\` (`02-data-formats.md`).
