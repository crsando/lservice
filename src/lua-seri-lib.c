#include "lua-seri.h"
#include "lua.h"
#include "lauxlib.h"

#define LUAMOD_API LUALIB_API

LUAMOD_API int
luaopen_seri(lua_State *L) {
	// luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "pack", luaseri_pack },
		{ "unpack", luaseri_unpack },
		{ "unpack_remove", luaseri_unpack_remove },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}