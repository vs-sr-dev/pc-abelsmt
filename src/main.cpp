// ABEL - Shin Mokushiroku Taisen, natively: the programs' code and TBIOS's, recompiled, on
// fmtownskit's FM Towns runtime. In a window (SDL3: the picture, the sound, the keyboard and
// gamepads as the Towns pad), or headless, saving frames and the sound.
//
//     abel [--cue "disc/....cue"] [--save DIR] [--font FILE] [--scale N] [--fullscreen] [--frames N]
//          [--cpu PCT] [--clock T] [--record FILE] [--play FILE]
//     abel --headless [--frames N] [--png F,F,...] [--png-every N] [--out DIR] [--wav FILE]
//          [--input F-G:BUTTONS,...] [--play FILE] [--trace-int] [--trace-io] [--trace-from F] [--vram F] [--ram F]
//     abel --replay-sound LOG --seconds S --wav FILE [--mute ...]
//
// The port runs the disc's programs as its START does (docs/01-disc-layout.md): OPNG (the logo,
// the opening, the title's menu), then ABEL (with LOAD if OPNG's errorlevel is 1 or more), then,
// if ABEL ends with errorlevel 0, ENDG 429266408 (the ending).
//
// --save: the host directory holding the floppy drives (A: is DIR/a; default save).
// --font: the Towns' font ROM (FMT_FNT.ROM, 256 KB; by default bios/fmtowns/fmt_fnt.rom).
// --frames: stop after N frames (headless: 600 if not given; in the window: never).
// --png, --png-every: the frames to save (they imply --headless); MAME's whole frame, as the
// oracle's snapshots, or with --crop the picture only, as the window shows it.
// --wav: the sound (the FM, the PCM and the CD's audio mixed as MAME's fmtowns mixes them), 16-bit
// stereo at 48 kHz, from the program's first instruction to the last frame.
// --mute fm,pcm,cd: leave sources out of the mix.
// --vram F: VRAM and sprite RAM at frame F, to vram.bin and sprram.bin in --out.
// --ram F: the program's segment at frame F (its first bytes, to the stack's top), to prog.bin in --out.
// --input: buttons held from frame F to frame G (both included), headless: up down left right a b
// run select, joined by '+' (e.g. 433-450:a).
// --cpu PCT: the 386's rate against MAME's (100, the datasheet's clocks at 16 MHz): by default 35 in
// the window (the user's estimate by eye against the game's video, not a measure of the machine), 100 headless (the oracle's, for comparisons). ABEL
// measures the machine once at its start (a map drawn, timed: the walk's wait), so the rate is
// set from the start; F5 and F6 change it by 5 and F7 sets 100, but the walk keeps the wait
// measured at the start (docs/11-runtime.md).
// --clock T: DOS's date and time start at T (seconds since 1970) and run with the machine's time,
// not the host's clock's.
// --record FILE: in the window, the pad and the CPU's rate written down by frame, with the clock
// the run started at (--clock's); --play FILE runs them again, headless or in the window (the
// keys then unheard), the same frame for frame:
//     clock T          DOS's clock (as --clock)
//     cpu F PCT        the CPU's rate from frame F's VSYNC on
//     pad F BUTTONS    the pad held from frame F on (as --input's; '-' none)
// --profile FILE: a line a frame -- the CPU's clocks in it, and its accesses to VRAM, sprite RAM
// and the ports (read, written): where the CPU's time goes (docs/05-open-questions.md 1).
// --abel-quit: ABEL ends at once with errorlevel 0 and ENDG runs, as tools/abelquit.lua makes it
// on the oracle (the ending without the game).
// --replay-sound: no game, the oracle's sound log (tools/oracle.py --sound-log) played into the
// runtime's chips from power-on for S seconds.
//
// The machine is MAME's fmtowns with 4 MB, booted from this disc (docs/06-attack-plan.md).
// Frames are counted in VSYNCs from the program's first instruction; on the oracle OPNG's
// runs 16.002 seconds after power-on (docs/11-runtime.md), so frame F here is MAME's
// 16.002 + F/60 s.
#include "towns.h"
#ifdef ABEL_SDL
#include "towns_sdl.h"
#endif
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

// The machine as this disc's boot leaves it to each program (CONFIG.SYS loads TBIOS's modules
// through TBIOSLD.SYS; AUTOEXEC.BAT runs START, whose script runs each program, whose stub runs
// RUN386 on it): read on MAME's fmtowns -ramsize 4M at each program's first write
// (tools/oracle.py --dump-write 973c4=2 for OPNG, 3bb834=2 for ABEL, docs/11-runtime.md). Each
// asks RUN386 for all memory: its segment at linear 0x408000, physical 0x50000 for OPNG's first
// 384 KB and 0x4F000 for ABEL's (OPNG's limit 0x376FFF, ABEL's 0x377FFF); selector 0x40 all of
// RAM but the 32 KB FORRBIOS.NSD keeps at the top.
static const towns::Profile kOpng = {"towns4m+nsd (ABEL's OPNG)", 4, 0x408000, 0x50000, 0x376FFF, 0x3F7FFF};
static const towns::Profile kAbel = {"towns4m+nsd (ABEL's ABEL)", 4, 0x408000, 0x4F000, 0x377FFF, 0x3F7FFF};
// ENDG's is OPNG's (read at its first write, 2 to [0x27C04]: --dump-write 77c04=2 with
// tools/abelquit.lua, which ends ABEL at once so START runs ENDG)
static const towns::Profile kEndg = {"towns4m+nsd (ABEL's ENDG)", 4, 0x408000, 0x50000, 0x376FFF, 0x3F7FFF};

// START's programs: the file, the recompiled module, the directory START is in (cd), the
// machine RUN386 gives it, its name as the stub passes it to RUN386 (the environment's last
// string).
struct Program { const char* file; const char* module; const char* dir; const towns::Profile* profile; const char* name; };
static const Program kOpngProg = {"ABEL/OP/OPNG.EXE", "OPNG", "ABEL\\OP", &kOpng, "Q:\\ABEL\\OP\\OPNG.exp"};
static const Program kAbelProg = {"ABEL/ABEL.EXE", "ABEL", "ABEL", &kAbel, "Q:\\ABEL\\ABEL.exp"};
static const Program kEndgProg = {"ABEL/ED/ENDG.EXE", "ENDG", "ABEL\\ED", &kEndg, "Q:\\ABEL\\ED\\ENDG.exp"};

// What differs from the state the kit's townsos() sets (the first game's boot), as on 4D
// Driving's disc: CRTC register 28, video register 1, the 16-colour palettes' half-bright
// colours (0xB0, not 0x80); and the slave PIC's mask, 0xCE: IRQ 13 (the sound chips') unmasked.
static void townsos_abel(towns::Machine& m) {
    m.video.crtc[28] = 0x801F;
    m.video.vreg[1] = 0x09;
    for (int l = 0; l < 2; ++l)
        for (int i = 1; i < 8; ++i)
            for (int k = 0; k < 3; ++k)
                if (m.video.pal16[l][i][k] == 0x80) m.video.pal16[l][i][k] = 0xB0;
    m.pic[1].imr = 0xCE;
}

// The PSP and the environment DOS made for RUN386 (selectors 0x24 and 0x2C, at the same places
// for OPNG, ABEL and ENDG in the dumps): the command tail (none for OPNG and ABEL: its length byte
// 0; ENDG's " 429266408");
// the environment COMSPEC, PROMPT and PATH (AUTOEXEC.BAT's), then the program's name as the stub
// passes it (OPNG's 0x57 bytes, selector 0x2C's limit 0x56; ABEL's 0x54, limit 0x53).
static void psp_abel(towns::Machine& m, const Program& pr, const std::string& args) {
    const uint32_t psp = 0xBF890, env = 0xBF820;
    m.set_desc(0x04, psp, 0xFF);
    m.set_desc(0x24, psp, 0xFF);
    static const char head_env[] = "COMSPEC=C:\\COMMAND.COM\0PROMPT=$p$g\0PATH=Q:\\;Q:\\HCOPY;Q:\\T_TOOL\0\0\x01\0";
    std::string e(head_env, sizeof head_env - 1);
    e += pr.name;
    e += std::string(2, '\0');
    m.set_desc(0x2C, env, (uint32_t)e.size() - 1);
    m.write_block(env, (const uint8_t*)e.data(), (uint32_t)e.size());
    static const uint8_t head[2] = {0xCD, 0x20};
    m.write_block(psp, head, 2);
    std::string tail = args.empty() ? "" : " " + args;
    std::vector<uint8_t> t;
    t.push_back((uint8_t)tail.size());
    t.insert(t.end(), tail.begin(), tail.end());
    t.push_back(0x0D);
    m.write_block(psp + 0x80, t.data(), (uint32_t)t.size());
}

struct Press { long from, to; unsigned held; };

static unsigned buttons(const std::string& s) {
    static const std::pair<const char*, unsigned> names[] = {
        {"up", towns::kPadUp}, {"down", towns::kPadDown}, {"left", towns::kPadLeft}, {"right", towns::kPadRight},
        {"a", towns::kPadA}, {"b", towns::kPadB}, {"run", towns::kPadRun}, {"select", towns::kPadSelect}};
    unsigned v = 0;
    for (size_t p = 0; p <= s.size();) {
        size_t q = s.find('+', p);
        std::string n = s.substr(p, q == std::string::npos ? std::string::npos : q - p);
        bool found = false;
        for (auto& [name, bit] : names) if (n == name) { v |= bit; found = true; }
        if (!found) { std::fprintf(stderr, "--input: no button %s\n", n.c_str()); std::exit(2); }
        if (q == std::string::npos) break;
        p = q + 1;
    }
    return v;
}

int main(int argc, char** argv) {
    std::string cue = "disc/Abel - Shin Mokushiroku Taisen (Japan).cue", out = "build/frames", save = "save",
                font = "bios/fmtowns/fmt_fnt.rom";
    std::string wav, replay;
    long frames = -1, every = 0, trace_from = 0, vram_at = -1, ram_at = -1;
    double seconds = 60;
    uint8_t mute = 0;
    std::set<long> at;
    std::vector<Press> presses;
    bool trace_int = false, trace_io = false, headless = false, crop = false, abel_quit = false;
    int cpu = -1;
    long long clock_start = 0;
    std::string record, play, profile;
#ifdef ABEL_SDL
    towns::SdlOptions win;
    win.title = "Abel";
#endif
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { if (i + 1 >= argc) { std::fprintf(stderr, "%s: a value\n", a.c_str()); std::exit(2); } return argv[++i]; };
        if (a == "--cue") cue = next();
        else if (a == "--save") save = next();
        else if (a == "--font") font = next();
        else if (a == "--frames") frames = std::atol(next().c_str());
        else if (a == "--out") out = next();
        else if (a == "--headless") headless = true;
        else if (a == "--png-every") { every = std::atol(next().c_str()); headless = true; }
        else if (a == "--png") {
            std::string s = next();
            for (size_t p = 0; p < s.size();) {
                size_t q = s.find(',', p);
                at.insert(std::atol(s.substr(p, q - p).c_str()));
                p = q == std::string::npos ? s.size() : q + 1;
            }
            headless = true;
        }
        else if (a == "--input") {
            std::string s = next();
            for (size_t p = 0; p < s.size();) {
                size_t q = s.find(',', p);
                std::string item = s.substr(p, q == std::string::npos ? std::string::npos : q - p);
                Press pr{};
                size_t colon = item.find(':'), dash = item.find('-');
                if (colon == std::string::npos) { std::fprintf(stderr, "--input: F-G:BUTTONS\n"); return 2; }
                pr.from = std::atol(item.c_str());
                pr.to = dash != std::string::npos && dash < colon ? std::atol(item.c_str() + dash + 1) : pr.from;
                pr.held = buttons(item.substr(colon + 1));
                presses.push_back(pr);
                p = q == std::string::npos ? s.size() : q + 1;
            }
        }
        else if (a == "--wav") wav = next();
        else if (a == "--replay-sound") replay = next();
        else if (a == "--seconds") seconds = std::atof(next().c_str());
        else if (a == "--mute") {
            std::string s = next();
            mute = (uint8_t)((s.find("fm") != std::string::npos ? 1 : 0) | (s.find("pcm") != std::string::npos ? 2 : 0)
                             | (s.find("cd") != std::string::npos ? 4 : 0));
        }
#ifdef ABEL_SDL
        else if (a == "--scale") win.scale = std::atoi(next().c_str());
        else if (a == "--fullscreen") win.fullscreen = true;
#endif
        else if (a == "--trace-int") trace_int = true;
        else if (a == "--trace-io") trace_io = true;
        else if (a == "--trace-from") trace_from = std::atol(next().c_str());
        else if (a == "--vram") { vram_at = std::atol(next().c_str()); headless = true; }
        else if (a == "--ram") { ram_at = std::atol(next().c_str()); headless = true; }
        else if (a == "--crop") crop = true;
        else if (a == "--cpu") cpu = std::atoi(next().c_str());
        else if (a == "--clock") clock_start = std::atoll(next().c_str());
        else if (a == "--record") record = next();
        else if (a == "--play") play = next();
        else if (a == "--abel-quit") abel_quit = true;
        else if (a == "--profile") profile = next();
        else { std::fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
    }
#ifndef ABEL_SDL
    headless = true;                    // built without SDL3: no window
#endif
    if (frames < 0) frames = headless ? 600 : 0;
    if (cpu < 0) cpu = headless ? 100 : 35;
    // a recording to play: its clock, its rates and its pad by frame
    struct Event { long frame; bool is_cpu; unsigned v; };
    std::vector<Event> events;
    if (!play.empty()) {
        FILE* f = std::fopen(play.c_str(), "r");
        if (!f) { std::fprintf(stderr, "cannot read %s\n", play.c_str()); return 1; }
        char line[256];
        while (std::fgets(line, sizeof line, f)) {
            char word[16], b[128];
            long fr;
            long long t;
            if (line[0] == '#') continue;
            if (std::sscanf(line, "clock %lld", &t) == 1) clock_start = t;
            else if (std::sscanf(line, "cpu %ld %127s", &fr, b) == 2) events.push_back({fr, true, (unsigned)std::atoi(b)});
            else if (std::sscanf(line, "%15s %ld %127s", word, &fr, b) == 3 && !std::strcmp(word, "pad"))
                events.push_back({fr, false, std::strcmp(b, "-") ? buttons(b) : 0});
        }
        std::fclose(f);
        std::stable_sort(events.begin(), events.end(), [](const Event& x, const Event& y) { return x.frame < y.frame; });
        for (const Event& e : events)       // the rate at frame 0 is the run's from its first instruction
            if (e.is_cpu && e.frame == 0) cpu = (int)e.v;
    }
    FILE* rec = nullptr;
    if (!record.empty()) {
        if (headless || !play.empty()) { std::fprintf(stderr, "--record is for the window, without --play\n"); return 2; }
        if (!clock_start) clock_start = (long long)std::time(nullptr);
        rec = std::fopen(record.c_str(), "w");
        if (!rec) { std::fprintf(stderr, "cannot write %s\n", record.c_str()); return 1; }
        std::fprintf(rec, "# abel --record: frame F is the oracle's 16.002 + F/60 s\nclock %lld\ncpu 0 %d\n", clock_start, cpu);
    }

    towns::Machine m;
    m.profile = &kOpng;
    townsos_abel(m);
    std::string err;
    if (!m.disc.open(cue, err)) { std::fprintf(stderr, "%s\n", err.c_str()); return 1; }
    m.audio_mute = mute;
    m.clock_start = clock_start;
    m.set_cpu_rate(cpu);
    m.save_dir = save;
    m.cd_dir = "ABEL\\OP";              // START's script: cd \abel\op, then OPNG
    if (!replay.empty()) {
        towns::WavWriter w;
        if (wav.empty() || !w.open(wav)) { std::fprintf(stderr, "--replay-sound needs --wav FILE\n"); return 2; }
        if (!towns::replay_sound_log(m, replay, seconds, w, err)) { std::fprintf(stderr, "%s\n", err.c_str()); return 1; }
        return 0;
    }
    std::vector<uint8_t> tbios;
    if (!towns::compose_tbios(m.disc, "TBIOS/TBIOS.INI", tbios, err)) { std::fprintf(stderr, "TBIOS: %s\n", err.c_str()); return 1; }
    // the recompiled code is this disc's code
    auto same = [](const char* name, const std::vector<uint8_t>& img) {
        const X86Module* mod = x86_module(name);
        uint32_t crc = x86_crc32(img.data(), img.size());
        if (mod && mod->crc == crc && mod->size == img.size()) return true;
        std::fprintf(stderr, "%s on this disc is not the one recompiled (crc %08x)\n", name, crc);
        return false;
    };
    std::vector<std::pair<const Program*, towns::P3>> progs;
    for (const Program* pr : {&kOpngProg, &kAbelProg, &kEndgProg}) {
        std::vector<uint8_t> exe;
        towns::P3 p;
        if (!m.disc.file(pr->file, exe)) { std::fprintf(stderr, "%s not on the disc\n", pr->file); return 1; }
        if (!towns::parse_p3(exe, p, err)) { std::fprintf(stderr, "%s: %s\n", pr->file, err.c_str()); return 1; }
        if (!same(pr->module, p.image)) return 1;
        progs.push_back({pr, std::move(p)});
    }
    if (!same("TBIOS", tbios)) return 1;
    if (FILE* f = std::fopen(font.c_str(), "rb")) {
        m.font.resize(0x40000);
        m.font.resize(std::fread(m.font.data(), 1, m.font.size(), f));
        std::fclose(f);
    } else {
        std::fprintf(stderr, "no font ROM (%s)\n", font.c_str());
    }
    m.trace_int = trace_int && !trace_from;
    m.trace_io = trace_io && !trace_from;
    if (every || !at.empty()) std::filesystem::create_directories(out);
    towns::WavWriter wavw;
    if (!wav.empty() && !wavw.open(wav)) { std::fprintf(stderr, "cannot write %s\n", wav.c_str()); return 1; }
#ifdef ABEL_SDL
    towns::SdlHost host;
    if (!headless && !host.open(win, err)) { std::fprintf(stderr, "no window: %s\n", err.c_str()); return 1; }
#endif
    std::vector<int16_t> audio;
    uint32_t stack_top = 0;
    size_t next_event = 0;
    FILE* prof = nullptr;
    uint64_t prof_cpu = 0;
    towns::Machine::AccessCount prof_n;
    if (!profile.empty()) {
        prof = std::fopen(profile.c_str(), "w");
        if (!prof) { std::fprintf(stderr, "cannot write %s\n", profile.c_str()); return 1; }
        std::fprintf(prof, "# frame cpu_clocks vram_r vram_w spr_r spr_w io_r io_w pad_r wait_w\n");
        m.set_count_access(true);
    }
    unsigned played = 0, recorded = 0;
    int recorded_rate = cpu;
    // the button names, as --input and the recording write them
    auto names = [](unsigned held) {
        static const char* n[] = {"up", "down", "left", "right", "a", "b", "run", "select"};
        std::string s;
        for (int i = 0; i < 8; ++i)
            if (held & (1u << i)) { if (!s.empty()) s += '+'; s += n[i]; }
        return s.empty() ? std::string("-") : s;
    };
    m.on_vsync = [&](towns::Machine& mm) {
        long f = (long)mm.frame;
        if (trace_from && f == trace_from) { mm.trace_int = trace_int; mm.trace_io = trace_io; }
        if (f == ram_at) {
            std::vector<uint8_t> seg(stack_top);
            mm.read_block(mm.profile->prog_base, seg.data(), (uint32_t)seg.size());
            std::filesystem::create_directories(out);
            if (FILE* fo = std::fopen((out + "/prog.bin").c_str(), "wb")) { std::fwrite(seg.data(), 1, seg.size(), fo); std::fclose(fo); }
        }
        if (f == vram_at) {
            std::filesystem::create_directories(out);
            for (auto [name, mem] : {std::pair<const char*, const std::vector<uint8_t>*>{"/vram.bin", &mm.vram}, {"/sprram.bin", &mm.sprram}})
                if (FILE* fo = std::fopen((out + name).c_str(), "wb")) { std::fwrite(mem->data(), 1, mem->size(), fo); std::fclose(fo); }
        }
        if (at.count(f) || (every && f % every == 0)) {
            std::vector<uint32_t> rgb;
            int w, h;
            mm.render(rgb, w, h);
            if (crop) {                 // the picture only, as the window shows it
                int x, y, cw, ch;
                mm.display_area(x, y, cw, ch);
                std::vector<uint32_t> part((size_t)cw * ch);
                for (int r = 0; r < ch; ++r)
                    std::copy_n(rgb.begin() + (long)(y + r) * w + x, cw, part.begin() + (long)r * cw);
                rgb.swap(part);
                w = cw; h = ch;
            }
            char name[64];
            std::snprintf(name, sizeof name, "/f%06ld.png", f);
            towns::write_png(out + name, rgb, w, h);
        }
        if (prof) {
            uint64_t cpu_now = mm.cpu_time(mm.now);
            const towns::Machine::AccessCount& n = mm.access;
            std::fprintf(prof, "%ld %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", f, (unsigned long long)(cpu_now - prof_cpu),
                         (unsigned long long)(n.vram_r - prof_n.vram_r), (unsigned long long)(n.vram_w - prof_n.vram_w),
                         (unsigned long long)(n.spr_r - prof_n.spr_r), (unsigned long long)(n.spr_w - prof_n.spr_w),
                         (unsigned long long)(n.io_r - prof_n.io_r), (unsigned long long)(n.io_w - prof_n.io_w),
                         (unsigned long long)(n.pad_r - prof_n.pad_r), (unsigned long long)(n.wait_w - prof_n.wait_w));
            prof_cpu = cpu_now;
            prof_n = n;
        }
        // the recording's events due: the CPU's rate at this VSYNC, the pad held from the next frame
        for (; next_event < events.size() && events[next_event].frame <= f + (events[next_event].is_cpu ? 0 : 1); ++next_event)
            if (events[next_event].is_cpu) mm.set_cpu_rate((int)events[next_event].v);
            else played = events[next_event].v;
        if (headless) {
            unsigned held = played;
            for (const Press& pr : presses) if (f + 1 >= pr.from && f + 1 <= pr.to) held |= pr.held;
            mm.set_pad(0, held);
            if (!wav.empty()) {
                mm.audio_take(audio);
                wavw.write(audio);
            }
        }
#ifdef ABEL_SDL
        else {
            host.vsync(mm);
            if (!play.empty()) mm.set_pad(0, played);
            if (rec) {
                if (mm.cpu_rate != recorded_rate) { recorded_rate = mm.cpu_rate; std::fprintf(rec, "cpu %ld %d\n", f, recorded_rate); }
                if (mm.pad_held[0] != recorded) { recorded = mm.pad_held[0]; std::fprintf(rec, "pad %ld %s\n", f + 1, names(recorded).c_str()); }
                std::fflush(rec);
            }
        }
#endif
        if (frames && f >= frames) throw towns::Stop{0};
    };

    X86Cpu c;
    towns::boot_towns(m, tbios);
    // one program as RUN386 runs it: its errorlevel, or -1 when the run stopped inside it
    auto run = [&](size_t k, const std::string& args) -> int {
        const auto& [pr, p] = progs[k];
        m.profile = pr->profile;
        m.cd_dir = pr->dir;
        for (const auto& [q, _] : progs) x86_park(x86_module(q->module), q != pr);
        towns::run386_load(m, c, p);
        psp_abel(m, *pr, args);
        stack_top = p.esp;
        X86Func entry = x86_lookup_cs(0x0C, c.eip);
        if (!entry) { std::fprintf(stderr, "no recompiled function at %s's entry %x\n", pr->module, c.eip); return -1; }
        try {
            entry(c);
            std::fprintf(stderr, "%s returned from its entry\n", pr->module);
            return -1;
        } catch (const towns::Stop&) {
        }
        if (!m.exited) return -1;
        int code = m.exit_code;
        std::fprintf(stderr, "%s: exit %d after %llu frames, %.2f s of the guest\n", pr->module, code,
                     (unsigned long long)m.frame, (double)m.now / towns::kClock);
        towns::run386_end(m);
        return code;
    };
    // START's script
    int code = run(0, "");                          // OPNG
    if (code >= 0 && abel_quit) {                   // ABEL as tools/abelquit.lua leaves it on the oracle
        std::fprintf(stderr, "ABEL: exit 0 at once (--abel-quit)\n");
        code = 0;
    } else if (code >= 0) code = run(1, code >= 1 ? "LOAD" : "");      // ABEL, or ABEL LOAD
    if (code == 0) code = run(2, "429266408");     // ENDG, when ABEL ends with errorlevel 0
    std::fprintf(stderr, "%s after %llu frames, %.2f s of the guest (the last safe point %04x:%08x)\n",
                 code >= 0 ? "exit" : "stopped", (unsigned long long)m.frame, (double)m.now / towns::kClock,
                 c.sreg[1], c.eip);
    return code >= 0 ? 0 : 0;
}
