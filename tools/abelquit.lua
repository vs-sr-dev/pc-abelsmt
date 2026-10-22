-- tools/oracle.py --lua tools/abelquit.lua: ABEL ends at once with errorlevel 0, so START runs
-- ENDG 429266408 next (the ending, without playing the game to it; docs/11-runtime.md).
--
-- At ABEL's first write (2 to [0x9B834], physical 0x3BB834, the same as --dump-write
-- 3bb834=2), the code after that instruction (0x40D9E, physical 0x4F000 + 0x40D9E: ABEL's first
-- 388 KB lie at 0x4F000) becomes `mov eax, 4C00h; int 21h`: RUN386's exit with AL 0.
local cpu = manager.machine.devices[":maincpu"]
local sp = cpu.spaces["program"]
local quit = {0xB8, 0x00, 0x4C, 0x00, 0x00, 0xCD, 0x21}
local quit_done = false
quit_tap = sp:install_write_tap(0x3BB834, 0x3BB837, "abelquit", function(offset, data, mask)
    if quit_done or offset ~= 0x3BB834 or (data & mask) ~= 2 then return end
    if (cpu.state["CR0"].value & 0x80000001) ~= 0x80000001 then return end
    quit_done = true
    for i, b in ipairs(quit) do sp:write_u8(0x4F000 + 0x40D9E + i - 1, b) end
    print(string.format("abelquit: ABEL told to exit at %.3f s", manager.machine.time:as_double()))
end)
