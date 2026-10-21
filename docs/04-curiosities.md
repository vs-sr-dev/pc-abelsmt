# 04 - Curiosities

* **A batch file, compiled and encrypted.** The disc's start is `START.EXE`,
  2,691 bytes that decrypt their own script and hand it to COMMAND.COM: the
  three programs in turn, the game told by an errorlevel whether to LOAD,
  and the ending run with a number (429266408) that a hand typing `ENDG`
  would not know (`01-disc-layout.md`).
* **Fujitsu's "exp2exe".** Each program is an .EXE that starts the DOS
  extender on itself: a Phar Lap P3 behind a small DOS stub, so that the
  script can name `ABEL` and not `RUN386 ABEL.EXP`.
* **A TBIOS in pieces.** Where the first two discs carry TBIOS as one file,
  ABEL's (1995) has a loader, "All programmed by Y.Miyazawa", and eight
  relocatable modules in Phar Lap's old "MQ" format, each with a name
  (graphic, sound, int 04dh, mouse, mouseI/O, sprite, font), linking their
  entries into the same table at the same address as the old file did. A
  later TBIOS for a later Towns, built to be configured: `TBIOS.INI`.
* **A MIDI card the emulator does not have.** The game's music driver
  plays the Towns' own FM and PCM chips, and sends the score to a MIDI card
  as well (ports 0xE70/0xE71, a second at 0x4A0/0x4A1), with its own
  interrupts (`03-executables.md`).
* **An eleven-minute intro**, scored scene by scene with nine of the CD's
  tracks (2 to 10), the program waiting for each to end (`01-disc-layout.md`);
  on the oracle's first run the same prologue came with FM music instead.
* **What it is about**, in the words of a YouTube uploader of its gameplay
  (the user's find, session 1): "heavily inspired by the works of Go Nagai
  and Ken Ishikawa, in particular Shin Getter Robot (look at the monsters,
  they resemble A LOT the mecha beasts from Shin Getter's manga and
  anime)... the apocalyptic feeling of the best works by Nagai (in fact the
  game it's called 'true revelation war') and an heavy cyberpunk feeling...
  set in the late 21st century, where a mysterious huge and floating
  fortress of demons and other beasts emerges from the subspace." The
  game's 獣魔 (*jūma*, "beast-demons") and the fortress in the intro's
  pictures agree.
* **Paced by its own CPU.** Walking, scrolling (by whole 16-pixel tiles)
  and text go as fast as the 386 draws them: no timer waits in the main loop. MAME's 386, with no wait
  states, draws faster than a Towns, hence "Runs too fast" -- and Family
  Soft's *Azure*, two years older, carries the same note.
* **The official romanizations.** The design sketches in the extras gallery
  (`OMAKE\OM10`-`OM28`) are labelled in Latin letters: ABEL, IO, RUBYNA,
  LEADER, MARY, SERGA, FIA, Dr. EDWARD, STEPHANIE, BOB, MARK, DAVID, JEFF,
  JENETTE, BURTON, FRED, GREN, EVE, GAY -- the names a translation can take
  from the team itself. The gallery also keeps a magazine advertisement
  (`OM01_T`), the manual's pages, and unused pictures ("未使用グラフィック").
* **Two dead lines.** Two maps keep text that no index entry reaches: a
  stray NUL (`M03.ABM`) and an old copy of a line with no speaker
  (`Z04.ABM`: "基地での使用はできません。", "it cannot be used in the base").
* **"Cyber Fantasy ABEL"**: the words drawn on the game screen's frame
  (`IMG\0\GAMEN.TIF`), and "サイバーファンタジーRPG" over the logo on a page in the
  gallery (`OMAKE\OM03_T1.TIF`). The base the game starts in lies under the Australian continent
  ("この基地は、オーストラリア大陸の地中にあります！", `MAP\9\Z01.ABM` and `Z02.ABM`).
