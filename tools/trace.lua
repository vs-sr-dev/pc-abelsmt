-- MAME's instruction trace over a span of emulated seconds, loops folded ("(loops for N
-- instructions)"), to trace.txt in the oracle's output directory; then MAME quits.
-- python tools/oracle.py --at 65,71.5 --input 30-30.2:A,38-38.2:A,69.5-71.5:Right --debug --lua tools/trace.lua
-- Edit T0 and T1 for another span (and keep --at past T1: the snapshots end the run at their
-- last); a second of the game is about 1.2 million lines.
local T0, T1 = 70.0, 71.0
local dbg = manager.machine.debugger
local state = 0
emu.register_frame_done(function()
    local t = manager.machine.time:as_double()
    if state == 0 and t >= T0 then
        dbg:command("trace trace.txt,maincpu")
        state = 1
    elseif state == 1 and t >= T1 then
        dbg:command("trace off")
        state = 2
        manager.machine:exit()
    end
end)
