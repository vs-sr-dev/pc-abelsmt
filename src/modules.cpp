// The recompiled modules this port runs (build/game: the recompiler's output, which may also
// hold the self-test's OPTEST module): OPNG and ABEL under code selector 0x0C (one at a time,
// the other parked), TBIOS under 0x110.
#include "x86.h"
extern const X86Module g_mod_p_opng, g_mod_p_abel, g_mod_p_tbios;
const X86Module* const g_x86_modules[] = {&g_mod_p_opng, &g_mod_p_abel, &g_mod_p_tbios};
const int g_x86_nmodules = 3;
