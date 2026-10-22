#!/bin/sh
# The port's recompile (docs/09-recompiler.md): OPNG, ABEL and ENDG under code selector 0x0C, TBIOS
# (composed from the disc's modules by fmtownskit.tbiosld) under 0x110, into build/game.
#
#     sh tools/recomp.sh            # from the port's directory; KIT=... for a kit being worked on
#     sh tools/recomp.sh --optest --selftest 3 --jobs 16 --out build/stest
#
# The seeds are the code the runtime met that discovery could not reach (docs/09): addresses
# passed as immediates, kept in variables and tables, called through registers and frame slots.
set -e
PORT=$(pwd -W 2>/dev/null || pwd)     # D:/..., as Python on Windows wants it
KIT=${KIT:-$PORT}                  # the directory holding fmtownskit/ (the submodule by default)
OUT=build/game
case "$1" in --out) OUT=$2; shift 2;; esac
for a in "$@"; do case "$a" in --out) echo "--out first" >&2; exit 2;; esac; done

mkdir -p build/tbios
( cd "$KIT" && python -m fmtownskit.tbiosld "$PORT/build/disc/TBIOS/TBIOS.INI" --out "$PORT/build/tbios/TBIOS.IMG" > /dev/null )
RECORDS=$(cd "$KIT" && python -m fmtownskit.tbiosld "$PORT/build/disc/TBIOS/TBIOS.INI" --seeds)

# OPNG: MetaWare's nested helpers and stored handlers (24c74...2a920), callbacks given to the
# timer BIOS (pushed before `lea eax, [ebp-0xA]`: 1d0, 2180, 21ab8...2420c), the int 97h
# driver's slot trampolines (2f937 + 10*n), the far stubs it calls back through (2cea0, 2ceec,
# 2cf38), the IRQ 4/5 handlers given to 2506h (2daf4, 2db88), 2d84c, 2dd0c, and the table of
# eight handlers at 0x46218 (21e44...225d0)
OPNG=1d0,2180,21ab8,21b6c,21c20,21cd4,21e44,21f58,2206c,22180,22294,223a8,224bc,225d0,22b84,22db8,2420c,24c74,24d48,25ea8,25f90,298b4,2a51c,2a7a0,2a920,2cea0,2ceec,2cf38,2d84c,2daf4,2db88,2dd0c,2f937,2f941,2f94b,2f955,2f95f,2f969,2f973,2f97d

# ABEL: found the same ways before it ran (its timer callbacks, its int 97h trampolines at
# 47e83 + 10*n, the MIDI driver's handlers and far stubs 45b50, 45b9c, 45be8, the pointers in
# its data 3241 and 7282)
ABEL=76c,3241,7282,c230,c2c0,c350,c3fc,13044,19c70,239b0,23a70,2b50c,389a4,3ed0c,3edf4,3f740,3f8ac,42b54,43590,43a5c,43ce0,43e60,45b50,45b9c,45be8,46198,46440,46658,47e83,47e8d,47e97,47ea1,47eab,47eb5,47ebf,47ec9

# ENDG: OPNG's and ABEL's library and drivers found again by their code (the instructions alike,
# numbers masked): the timer BIOS's callbacks (c558...c774, 11f4, de74, 504), the table of eight
# handlers at 0x26CB8 (c8e4...d070), MetaWare's helpers at prologues pointed at by immediates
# (e7dc...13ff4), the far stubs (16540, 1658c, 165d8), the IRQ handlers (17194, 17228), the
# MIDI driver's "none" (16eec) and 173ac, the int 97h driver's slot trampolines (18e63 + 10*n)
ENDG=504,11e7,11f4,12ea,c558,c60c,c6c0,c774,c8e4,c9f8,cb0c,cc20,cd34,ce48,cf5c,d070,de74,e7dc,e85c,e8dc,e9b0,fb6c,fc54,fed0,105a0,105c4,1070c,12f88,13af0,13bf0,13e74,13ff4,16540,1658c,165d8,16eec,17194,17228,173ac,18e63,18e6d,18e77,18e81,18e8b,18e95,18e9f,18ea9

# TBIOS, beyond its entry table and the modules' records: the graphic module's callbacks kept in
# its frame (gs:[ebp+n]) from immediates and from the tables at 0x34BC, 0x5234, 0x5404, 0x55D8,
# 0x6158, 0x678C (128 entries), 0x7B14 (22), 0x7D50, 0xADD8, 0xB00C, 0xE7C8; the routines it
# passes in tables (0x765C, 0x7708); the sound and int 04dh modules' handlers and "none" stubs
TBIOS=4230,4278,4318,4a88,50a4,50e4,512c,5161,5164,519c,51c4,51f4,5214,5274,52b4,52fc,5334,536c,5394,53c4,53e4,5448,5488,54d0,5508,5540,5568,5598,55b8,56d4,578f,5bf4,5d54,6200,622c,6234,6244,6250,6260,6270,6280,628c,6298,62c4,62f4,6305,6334,6338,634c,6354,636c,6374,639a,63c8,63cc,63e0,63e8,6400,6408,642c,644c,6450,6454,6458,645c,6460,6468,648e,64b0,64b4,64bc,64c4,64cc,64d4,64dc,64e8,64f4,652c,6730,673c,6993,69a9,69c3,69c4,69d8,69e0,69f8,6a07,6a1d,6a2f,6a30,6ac0,6b64,6bb0,6c0c,6ca4,6d4c,6d98,6dfb,6e11,6e21,6e24,6f44,6f78,6f90,6fac,6fc4,6fe0,708c,70a8,70c8,7187,719d,71b0,71b4,72f0,7328,7340,7364,737c,73a0,74c4,74e8,750c,75e0,75ec,75f8,766c,7718,7724,772c,776c,79e8,79ef,79f8,79ff,7a0c,7a13,7b6c,7b74,7b7c,7b9b,7c70,7c78,7c80,7c9f,7ce0,7d60,7d88,7db4,7de0,7e08,82ac,82d8,83d0,8500,8548,8850,8897,a1f4,a710,aacc,abb0,ac5c,ade8,ae08,ae24,ae2c,b01c,b034,b04c,b054,c74c,d274,d6c4,dfe8,e048,e140,e18c,e280,e2e0,e7d8,e7fc,e834,e870,e8a8,ea94,ed1c,ed50,ed70,ede8,ee24,ee3c,11290,1448c,1475c,14848,148c0,14902,14904,14914,14a0e,14a13

cd "$KIT"
python -m fmtownskit.recomp --out "$PORT/$OUT" \
    --exp "opng=$PORT/build/disc/ABEL/OP/OPNG.EXE@0c+$OPNG" \
    --exp "abel=$PORT/build/disc/ABEL/ABEL.EXE@0c+$ABEL" \
    --exp "endg=$PORT/build/disc/ABEL/ED/ENDG.EXE@0c+$ENDG" \
    --raw "tbios=$PORT/build/tbios/TBIOS.IMG@110:20-1e0+$RECORDS,$TBIOS" "$@"
