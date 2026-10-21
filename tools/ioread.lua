-- What MAME's io space answers at some ports, read once 47 s in (tools/oracle.py --lua):
-- the ports the runtime answers by rule (an absent card's, the odd lanes of a umask), to ioread.txt.
local ports = {0x470, 0x5e8, 0x5e9, 0x5ea, 0x5eb, 0x5ec, 0x5ed, 0x5ee, 0x5ef, 0xe51, 0xe59, 0xe61, 0xe69,
               0xe70, 0xe71, 0x4a0, 0x4a1, 0x4a9, 0x24, 0x3001, 0xfd93, 0xfd95}
local done = false
emu.register_frame_done(function()
    if done or manager.machine.time:as_double() < 47 then return end
    done = true
    local io = manager.machine.devices[":maincpu"].spaces["io"]
    local f = io.open and nil
    f = assert(_G.io.open("ioread.txt", "w"))
    for _, p in ipairs(ports) do f:write(string.format("%04x %02x\n", p, io:read_u8(p))) end
    f:close()
end)
