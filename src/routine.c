#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "routine.h"

int run_lua_code(char * code)
{
	lua_State *L = luaL_newstate();

	if(!L) {
		printf("THREAD FATAL ERROR: could not create lua state\n");
		return -1;
	}
	luaL_openlibs(L);

	if(luaL_loadstring(L, code)) {
		printf("FATAL THREAD PANIC: (loadstring) %s\n", lua_tolstring(L, -1, NULL));
		lua_close(L);
		return -1;
	}

	if(lua_pcall(L, 0, 0, 0)) {
		printf("FATAL THREAD PANIC: (pcall) %s\n", lua_tolstring(L, -1, NULL));
		lua_close(L);
		return -1;
	}

	lua_close(L);
	return 0;
}