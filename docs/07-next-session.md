# 07 - Next session

Where session 3 left it: ENDG recompiled and run after ABEL (the ending, read
on the oracle by ending ABEL at once: 125 snapshots of 126 the same); a rate
for the guest's CPU, **35% in the window by the user's eye** (an estimate
against the video, not a specification); ABEL's own measure of the machine
found (a map's drawing timed at its start, the walk's wait); a play recorded
in the window and run again the same; the second program's frozen first
seconds fixed in the kit; the sound "perfect" by the user's ear
(`00-sessions.md`).

Goal: **the first dungeon, and the oracle on the way there**.

1. **The way to the first dungeon**, played by the user in the window at
   35% (`build/rt/abel --record build/play3.txt`), or found headless from
   the recordings so far (`build/play2.txt`: the castle, the corridor, the
   menu). Its frames a PNG a second for `docs/`.
2. **The same way on the oracle**, where a room stands still: the
   recording's presses turned into the oracle's `--input` (MAME's times its
   own: it loads at the CD's speed, so each press is placed by what is on the
   screen), the port headless at 100% (MAME's speed), the frames held against
   each other at the dungeon's entrance.
3. **ABEL's measure, 3 ticks on the oracle and 2 here** at the same speed
   (`11-runtime.md`): the drawing's clocks on both (a trace on the oracle
   from 0xD64 to 0xD94), the timer's ticks around it. Tried without effect:
   the tick's phase from power-on, port 0x6C's 1 us waits.
4. Worth doing on the way: OPNG's LOAD (errorlevel 1, `ABEL LOAD`) and where
   the game saves (`05-open-questions.md` 5, 11); the loading screen's
   console text (14), the fades (15), the seeds' scans into the kit (17).

Tools: `tools/recomp.sh` (`KIT=DIR`, the directory holding the kit being worked on;
`--out build/stest --optest --selftest 3 --jobs 16`, then
`build/stest/b/selftest build/stest/selftest/*.txt`), `build/rt/abel`
(`11-runtime.md`; headless at 100%, A at frames 840 and 1320 is CD PLAY,
GAME START; `--play`, `--profile`, `--abel-quit` for the ending),
`tools/framematch.py`, `tools/oracle.py` (`--input 30-30.2:A,38-38.2:A` to
the game; `--lua tools/abelquit.lua` for the ending; `--dump-write`,
`--trace-calls`, `--lua tools/ioread.lua`), `tools/abelfiles.py`. The port
builds against the kit's runtime given by `-DFMT_RUNTIME` (the working kit
until its commit is pulled into the submodule).
