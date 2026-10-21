# 07 - Next session

Where session 2 left it: OPNG, ABEL and TBIOS recompiled and self-tested,
run one after another as START runs them; the logo, the title and its menu
and the throne room the oracle's pixel for pixel; the whole opening played to
its end and back to the title; the timer BIOS, the absent MIDI card and the
palette's registers answered as MAME has them (`00-sessions.md`).

Goal: **to the first dungeon, and the pace** -- where the only gameplay video
starts, so the user can hold the port (and MAME) against it by eye
(`05-open-questions.md` 1).

1. **The way to the first dungeon**, found on the port (60 s of the game in
   about a second headless, its frames the oracle's): from the throne room,
   the inputs that get there, written down as `--input` for both (the
   oracle's in seconds, the port's in frames: the port loads sooner, so its
   times are its own); then the same inputs on the oracle, the frames held
   against each other where a room stands still.
2. **The user plays it in the window** (`build/rt/abel`, X = A, Z = B, Enter
   = Run): how the walk and the scroll look against the video. The port's
   386 counts the datasheet's clocks, as MAME's, so it should look as MAME
   does -- "too fast". Then a clock rate for the guest (`X86_TICK`'s budget
   against real time, a switch, MAME's kept for comparison), set by the
   user's eye on the video; the 16-pixel scroll steps a second, counted on
   both, are the number to write down.
3. **ENDG**: its start read on the oracle (its physical base and limit; the
   way there is the game's end, or a dump at ENDG run by hand if the oracle
   can get there), recompiled with the same scans; START's `if errorlevel 1`
   after ABEL.
4. **The sound**: the opening's CD-DA (track 02, 53.9 s on the oracle; the
   port's `CD-DA 19828 to 24627` at frame 2193) against MAME's WAV
   (`tools/wavmatch.py` from 4D Driving); the game's FM and PCM music
   (Euphony, the kit's first game that plays the YM3438 for real) against
   MAME's WAV, then by ear.
5. Worth doing on the way: the loading screen's console text
   (`05-open-questions.md` 14), the throne room's fade (15), the seeds' scans
   into the kit (17).

Tools: `tools/recomp.sh` (`KIT=D:/Homebrew6` for the kit being worked on;
`--out build/stest --optest --selftest 3 --jobs 16` for the self-test),
`build/rt/abel` (`11-runtime.md`; A at frames 840 and 1320 is CD PLAY, GAME
START), `tools/framematch.py`, `tools/oracle.py` (`--input 30-30.2:A,38-38.2:A`
to the game; `--dump-write`, `--trace-calls`, `--lua tools/ioread.lua`),
`tools/abelfiles.py`. The port builds against the kit's runtime given by
`-DFMT_RUNTIME` (the working kit until its commit is pulled into the
submodule).
