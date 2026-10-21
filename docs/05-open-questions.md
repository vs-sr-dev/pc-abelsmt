# 05 - Open questions

1. **"Runs too fast": how fast is a Towns?** MAME's list marks the game
   partial for that alone. What is known (`03-executables.md`): the main
   loop waits on no timer, counter or vertical blank; walking and scrolling
   are as fast as the 386 draws (MAME, pressed right at 70.0 s: Abel crosses
   the throne room in 0.9 s; then a dialogue writes about 15-20 characters a
   second). **The scroll moves by whole tiles**: frame by frame on MAME
   (`--at 70.0-70.6/0.0166667`, the map window matched between frames), a
   step of 16 pixels every 1.5 frames on average (8 steps in 12 frames: about
   640 pixels, 40 tiles, a second); the frames between steps are unchanged.
   So on any machine the scroll goes by 16-pixel steps, and only their rate
   is the CPU's. A video of the game (YouTube, not said whether a real Towns
   or an emulator; the user's viewing, session 1) looks "normal, the
   scrolling fluid, like the first Duke Nukem": steps at a human rate, not
   40 a second. That video (about 16 years old, and with the intro the
   only one of the game on YouTube) starts in the first dungeon: the
   reference is the user's eye on the port, or the oracle, once there --
   whether it moves like the video. The intro's length (about 11 minutes left alone) says
   nothing of the CPU: it is the CD's (`01-disc-layout.md`), the same on
   both. MAME's i386 counts
   the datasheet's clocks with no wait states, and the kit's recompiler
   charges the same clocks: **a port timed as the kit times it now runs as
   fast as MAME, too fast.** To find:
   * where the real machine is slower: the VRAM's wait states (the drawing
     is VRAM writes, through the write mask), RAM refresh, the TBIOS's own
     loops; the FM Towns' documented wait states, if any are on record;
   * a reference of the real machine's speed: a recording of the game on a
     real Towns (the walk across the first room, the scroll's 16-pixel steps
     a second, the text's speed, against a timer the game cannot change --
     the music's tempo, which the YM's timer keeps, and the CD's tracks are
     the same on MAME and on the machine);
   * whether the text speed (`\S01`-`\S03`) and the battles have the same
     cause, or a delay loop of their own.
   The port's answer is likely a clock rate for the guest (the kit's
   `X86_TICK` budget against real time), set to match the reference; the
   user's eyes are the judge (`06-attack-plan.md`).
   *Session 2*: OPNG's pace is not the CPU's -- its waits are the timer
   BIOS's 10 ms ticks, and the port's logo and title take the oracle's time
   to the frame (`11-runtime.md`); the opening's scenes are the CD's. The
   question stands for ABEL's main loop. **The user played the port's window**
   (session 2): the walk is "very fast" against the YouTube video -- as MAME,
   so MAME's speed is the port's and the oracle stays faithful for everything
   else -- while **the dialogue's text looks the right speed**. So the text
   is paced by something real-time (*inferred*: the timer BIOS, as OPNG's
   waits; ABEL calls int 97h too), and only the CPU-paced part, the walk and
   the scroll, wants the guest's clock slowed.
2. **Which TBIOS module answers what**: the modules (V31L31 graphic and
   sound, V31L35 the rest) against the one-file V31L31 92/11/11 of the
   first two games -- the same functions at other offsets, or other code.
   The recompiler takes them from the composed image either way; their
   machine effects (the warikomi module's interrupts, the font module's ROM)
   are the runtime's.
3. **The MIDI driver with no MIDI card** -- *answered, session 2*: MAME reads
   0xFF at every port the driver probes (0xE51, 0xE59, 0xE61, 0xE69, 0xE70,
   0xE71, 0x4A0-0x4AD; read from Lua, `tools/ioread.lua`), and the driver
   takes it for no card; answered 0, it found one and waited for ever for its
   buffer to drain. The kit answers 0xFF where MAME has no reader
   (`11-runtime.md`). Still to see: whether the 470 reads a second of 0xE71
   while walking (session 1) are the same status poll, with no card.
4. **Discovery's remains**: the four overlapping copies (`movsd` inside
   `66 A5`) compile and pass the self-test (session 2); whether the game
   ever runs both readings, *not seen*. The table at 0x8CCB4 read to 52
   entries where 49 are real (a function at 0x1 seeded from data). The
   indirect transfers left (38, 38, 78 in OPNG, ABEL, TBIOS) are met by the
   runtime one by one (`09-recompiler.md`, the seeds). **OPNG's code between
   0x2E00 and 0x1E238** (over half of it) is reached by nothing found so far,
   and the whole opening ran without it: what it is (the HDD install? the
   other menu items?).
5. **The three programs in one port** -- *OPNG and ABEL, session 2*: the
   port runs START's script itself, on one machine (`11-runtime.md`); OPNG's
   errorlevel 0 for CD PLAY, GAME START. To do: ENDG (recompiled, its start
   read on the oracle: its physical base and limit), and OPNG's errorlevel for
   LOAD (*inferred*: 1 or more, START's `if errorlevel 1`).
6. **HDD PLAY and HDD INSTALL**: the title copies the data to drive D: (20
   MB). The port reads the disc image; the menu's other items to keep,
   hide or answer.
7. **int 97h** -- *answered, session 2*: the Towns' timer BIOS (timers of 10
   ms ticks with a limit and a far callback, AH=00h-02h, `11-runtime.md`),
   which FORRBIOS.NSD hands to the ROM's real-mode BIOS. What the mode's bit 1
   means (OPNG's timers are mode 2) and what AH=02h writes besides the count,
   *not known*. **int 8Eh AX=CF00h** (KimG) is answered as on 4D Driving
   (EAX 0); whether OPNG and ABEL call it, *not traced* (the kit does not
   trace it).
8. **The CD-DA**: the intro's are known (tracks 2-10, `01-disc-layout.md`);
   the ending's (11-20, *inferred*) to read; whether the game ever plays one
   (its int 93h has AH=02h only); the title and the logo are silent on MAME
   -- as on the machine? And **FM or CD for the intro**: left alone, the
   pre-analysis' run (the oracle's NVRAM then new) played the prologue with
   FM music and no CD command; the two later runs that reached it (OPENING
   chosen, and 900 s left alone) play the CD's tracks. What
   decides (the CMOS RAM, the clock, a check of the CD's audio) is not known;
   the NVRAM is shared by the oracle's runs.
9. **The music**: Euphony's `.EUP` scores, the `.FMB`/`.PMB` banks, the
   driver that plays them on the YM3438 and the RF5C68 (and the MIDI card):
   what the runtime's chips must do for it (the first FM-heavy game of the
   kit: 4D Driving played none).
10. **The pad**: the game reads the pads at 0x4D0/0x4D2 itself; which
    buttons do what (A, B, Run, Select), and the keyboard (int 90h through
    FORRBIOS) if it uses one.
11. **The save**: `ABEL LOAD`, and where the game writes its saves (DOS
    calls through MetaWare's library; "書込みが保護されています。" in
    `ABEL.EXE`: a write-protected disk; "ドライブ０にユーザディスクを入れ": a
    user disk in drive 0) -- a floppy, the CMOS RAM, or D:.
12. **The speakers' numbers** (89) to names and portraits, for the
    translation; and the pictures not yet decoded (`.16`, `.256`, `.IMG`,
    `.I16`, `.I25`, `.EVG`, `.DIF`, `.DIT`, `.CHR`), for the census of text
    drawn in them.
13. **The translation's two roads** (decided with the user: one phrase
    table, two outputs): for the port, where the game reads a record (the
    .MES/.ABM loaders) and where it draws a character (TBIOS's font module,
    or its own routine) -- the place to substitute text and a Western font;
    for the disc patch, what limits a longer line meets: the index's
    offsets are 32-bit (room to grow), but the window's lines and width,
    the font ROM's half-width set, and `ABEL.EXE`'s own strings (pointers
    into its data) are to read.
14. **ABEL's loading screen** (session 2): on the oracle the console's text
    (`ABEL Ver 1.00  System Loading....`, a dot per file) is drawn on the
    screen in the system's text mode (an 800x440 frame); the port writes it to
    the host's terminal and leaves the screen as OPNG left it. Who draws it
    on the Towns (DOS's CON through the BIOS, *inferred*) and whether the port
    should (a few seconds, then the game's own screen).
15. **The throne room's fade** (session 2): the oracle's snapshot at 60 s,
    inside the fade in, differs in 5.2% of its pixels from the nearest
    runtime frame; the room after it is the same pixel for pixel. A step of
    the fade the runtime does not show, or MAME's partial update when a
    palette is written during the frame (`towns_update_palette` draws the
    screen up to the beam first): *not checked*.
16. **TBIOS's conflict at 0x836E** (`test` overlapping an `or` at 0x8370), in
    the graphic module: real overlapping code, or a seed into data.
17. **The seeds' scans into the kit**: the rules that found this session's
    seeds (`09-recompiler.md`) are throw-away scripts; a `recomp.discover`
    option to apply them (with a report to review, as the scans gave) would
    spare the next game the one-at-a-time runs.
