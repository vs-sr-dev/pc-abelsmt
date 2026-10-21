# 02 - The data

The game's files, under `ABEL\`. Session 1 read the text and the maps to
their last byte (they are what a translation needs), and the rest by its
headers; the port runs the programs' own code over all of them, so a format
matters here when a tool needs it, or when the oracle and the runtime
disagree.

## The directories

| Directory | Files | What (*inferred* from names and headers where not said) |
|---|---|---|
| `ABEL\` | `ABEL.EXE` | the game (`03-executables.md`) |
| `OP\` | `OPNG.EXE`, 49 `.TIF`, `.EUP` x2, `.FMB`, `.PMB` x2, `OPNG.MES/.MID/.MCT` | the opening and the title: its pictures, its music, its dialogue |
| `ED\` | `ENDG.EXE`, 16 `.TIF`, `KANE.EUP`, `.FMB`, `.PMB` | the ending |
| `MAP\0`-`9` | 107 `.ABM` | the maps: tiles, events and the dialogue (below) |
| `MSG\` | `MSG_BAT`, `MSG_B_EV`, `MSG_ITM` (`.MES` + `.MID`), `MESCTRL.BIN` | battle messages, battle events' dialogue, item messages |
| `BGM\`, `BGM\0`-`3`, `MB` | 41 `.EUP`, `ABEL.FMB`, `GSCAPTAL.FMB`, 4 `.PMB` | the music: Euphony scores, FM voice banks ("Piano"...), PCM banks |
| `SND\`, `SND\AB`, `IO`, `RB` | 54 `.SND` | sound effects and, by the directories, the voices of Abel, Io and Rubyna |
| `CHR\` | `PC_ABEL`, `PC_IO`, `PC_RUBY` `.CHR` (27,648 each) | the three playable characters |
| `IMG\0`-`2`, `C`, `D`, `W` | `.IMG`, `.16`, `.256`, `.I16`, `.I25`, `.PAL`, `.TIF` | screens, windows, the battle's pictures (`MONSDAT.IMG`, `DMG_BOSS.IMG`, `ATKMARK.IMG`...) |
| `EG\0`-`6` | 60 `.EVG` (41,472 each) | event pictures |
| `OG\16\0`-`6`, `OG\256` | 65 `.16`, 20 `.256` | portraits (`OG_10`-`13.TIF` in `OP\` are 80x80 faces) |
| `MS\0`-`6` | 67 `.DIF`, `MS65.TIF` | monsters |
| `SPEF\` | 16 `.DIT` | special effects |
| `OMAKE\` | 47 `.TIF` (640x480, 8 bits) | the extras gallery: the magazine page and the manual's pages, unused pictures, the design sketches |

## Text

**One format, in two places.** A `.MES` file is a run of records:

    u32 speaker   Shift-JIS text   00

and its `.MID` is the index, twelve bytes an entry: `u32 id, u32 n, u32
offset` -- n records from that offset. A map (`.ABM`) carries the same
pair as two of its chunks, `.BIN` (the records) and `.IDX` (the index). An
entry with no records (n = 0) owns the bytes up to the next entry, and they
are dead: one NUL in `MAP\8\M03.ABM` (entry 5), an old line with no speaker
in `Z04.ABM` (entry 901, a copy of the line before). With those two read so,
**every byte of every text is one record's, once**
(`python tools/abelfiles.py text build/disc/ABEL`):

| Where | Files | Index entries | Records | Characters |
|---|---|---|---|---|
| maps (`.BIN`/`.IDX`) | 95 of 107 (12 carry no text: `.CBM` and `.DAT` only) | 1,360 | 3,296 | 64,890 |
| `.MES`/`.MID` | 4 (`MSG\` 3, `OP\OPNG`) | 162 | 223 | 3,388 |
| | | | **3,519** | **68,278** |

Characters are counted with the control codes taken out. The index ids
group a scene's lines: `OPNG.MID` has two entries, 2 and 22 lines (the
prologue's dialogue); `MSG_ITM` and `MSG_BAT` one line an entry (38 and 112).
In the maps the ids run in hundreds by kind (*inferred*: 1-99 the map's own
events, 500s, 800s, 900s others, as `Z04.ABM`'s 1, 2, 800-803, 850, 851, 900,
901).

**The speaker** is a number: 89 of them. 1 is the narration and the
messages with no face (19,355 characters: "廃墟と化した都市。巨大なクレーターの中に、獣魔基地への入口が開いている。");
in the maps 2 is *inferred* Abel ("白骨？！"), in `MSG_B_EV` 44 *inferred* Reis
(he laughs, and 5 answers "レイス！"). The opening has its own numbers: in
`OPNG.MES` 2, 3, 4 are the leader and two scientists. The table of numbers
to names is for the next sessions (the portraits in `OG\` by number,
*inferred*).

**The control codes**, inside the text (how often, all text):

| Code | Count | What (*inferred* from where they stand, unless said) |
|---|---|---|
| `\N` | 714 | a new line |
| `\@` | 237 | the end of a page: wait for the button |
| `\B01` (`\B02`, `\B03` in item stats) | 491 | a pause; in `ABEL.EXE`'s item texts it stands between fields |
| `\C04` `\C06` `\C07` `\C12` `\C15` | 21, 82, 1,408, 8, 1,153 | a colour: `\C07` before a name or a question, `\C15` after it (back to the text's colour) |
| `\S01` `\S02` `\S03` | 225, 175, 1,834 | a speed of the text: `\S01` before "・・・・", `\S03` most lines' start |
| `\I` | 29 | an item's name (`\C04\I\C15を見つけた！`: "found X!") |
| `\G`, `\T`, `\P`, `\X` | 55, 53, 21, 4 | names and numbers filled in (in `MSG_BAT`: *inferred* the gold, the target, the party member, an amount) |

The other text of the game is in **`ABEL.EXE`'s data**: about 1,040
strings, 4,940 double-byte characters -- the menus and the status screens'
labels ("名　称：", "能　力：", "消費SP："), and the names and descriptions of
items, weapons, skills and monsters ("凶獣ギルガが持っていた銀色のクリスタル。"),
with the same control codes. `OPNG.EXE` and `ENDG.EXE` hold a few dozen
strings each: the hard disk install's prompts, and a CD library's error
messages (*inferred* never shown). The title's menu is a picture.

**Text in pictures**, for a translation to redraw (the TIFFs seen whole,
`python tools/abelfiles.py tif`): `OP\OP01.TIF` (the prologue's red text,
640x480), `OP07.TIF` (a second page of red text), the subtitle
真・黙示録大戦 (`OP05`, `OP05B`, `KIDOU_BG`), the opening's staff titles
`OPTP_S01`-`S15` (a role and names a picture, in kana and kanji),
`ED\ED07A.TIF` (the ending's text); in `OMAKE\`, the magazine page and the
manual's (`OM01_T`, `OM02_T`, `OM03_T1`-`T3`, dense text), the captions
"未使用グラフィック ..." under `OM04_V`-`OM09_V`, handwritten notes on the
map sketches (`OM29`-`OM34`). The ending's credits (`EDCT_01`-`07`), the
title's menu and the second menu (`GAME START`, `LOAD START`) are already in
Latin letters. Pictures in the other formats (`.16`, `.256`, `.IMG`, `.EVG`)
are not yet decoded: a census for later.

**The font**: none of the game's. Its text is drawn from the Towns' font ROM
(*inferred* from TBIOS's `FNT_STD` module, 1,344 bytes, which reads the ROM
through selector 0x138; the ROM also has half-width ASCII -- whether the
game's text routine can draw it is for the translation's sessions).

## Maps (`.ABM`)

`ABEL TOWNS V1.00`, sixteen bytes more (*inferred*: sizes or offsets of the
map's parts), then chunks to the file's end, each `'.XYZ'`, `u32 size`, the
data. All 107 files read so to their last byte. 95 have nine chunks:

| Chunk | Bytes (`A01.ABM`) | What (*inferred*) |
|---|---|---|
| `.RGB` | 48 | the palette: 16 colours, 3 bytes each (4-bit values) |
| `.PAT` | 1,026 | the tile patterns' table |
| `.CBM` | 40,960 | the tiles' pixels (40 KB in every map) |
| `.DAT` | 9,220 | the map itself |
| `.DIR` | 4,608 | directions or passability |
| `.EVL`, `.EVT` | 2,304, 1,308 | the events: where, and what |
| `.IDX`, `.BIN` | 204, 1,722 | the text (above) |

The other 12 have `.CBM` and `.DAT` only (*inferred*: a map's other state,
as `A03D` beside `A03`).

## Pictures (`.TIF`)

TIFF, uncompressed, with a palette: 8 bits (the 47 in `OMAKE\`) or 4 bits
with **FillOrder 2** (the low nibble the first pixel), which PIL opens but
does not decode; `tools/abelfiles.py tif` does. 115 in all.

## Sound

| Kind | What (*inferred* from the headers) |
|---|---|
| `.EUP` | Euphony scores (the Towns' "HE386" music format): a title at 0x20 (`abel_02`...) |
| `.FMB` | FM voice banks, 6,152 bytes, named voices ("Piano") |
| `.PMB` | PCM banks (`@01`...) |
| `.SND` | the Towns' PCM sound format: an 8-byte Shift-JIS name ("刀で切る", "cut with a sword"), its id, lengths, rate |

The CD-DA is the opening's and the ending's (`01-disc-layout.md`).
