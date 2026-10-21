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
2. **Which TBIOS module answers what**: the modules (V31L31 graphic and
   sound, V31L35 the rest) against the one-file V31L31 92/11/11 of the
   first two games -- the same functions at other offsets, or other code.
   The recompiler takes them from the composed image either way; their
   machine effects (the warikomi module's interrupts, the font module's ROM)
   are the runtime's.
3. **The MIDI driver with no MIDI card**: what the program reads at 0xE71
   and 0x4A1 to decide a card is there (MAME answers an unmapped port with
   0xFF or 0, *not checked*), and whether the driver then waits on the card
   (470 reads a second of 0xE71 while walking: a status poll). The port
   must answer those ports as MAME does, or as "no card" if that differs.
4. **Discovery's remains**: the four overlapping copies (`movsd` inside
   `66 A5`) need the emitter to emit both readings (does it, today, with
   two instructions sharing bytes? to test); the table at 0x8CCB4 read to 52
   entries where 49 are real (a function at 0x1 seeded from data); the 12,
   16 and 18 indirect transfers of the three programs.
5. **The three programs in one port**: `START` runs OPNG, then ABEL (with
   or without LOAD, by OPNG's errorlevel), then ENDG (with its number) --
   three recompiled modules, each started by RUN386 on its own, the
   runtime's DOS answering EXEC (4Bh) and the errorlevel (4Dh), or the port
   running the script's logic itself. And `START.EXE`, `COMMAND.COM`'s
   batch: run, or stood in for.
6. **HDD PLAY and HDD INSTALL**: the title copies the data to drive D: (20
   MB). The port reads the disc image; the menu's other items to keep,
   hide or answer.
7. **int 97h** (AH=01h, 02h, in all three programs) and **int 8Eh AX=CF00h**:
   FORRBIOS.NSD answers them in protected mode; what 97h is (*not known*:
   a timer or a calendar service of the Towns BIOS, to read in the NSD or in
   the ROM).
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
