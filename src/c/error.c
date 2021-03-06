/* Copyright (c) 2010 Ian C. Good
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "config.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "ratchet.h"
#include "misc.h"

#define add_errno_code(e) { lua_pushstring (L, #e); lua_rawseti (L, -2, e); }

/* {{{ build_errno_code_table() */
static void build_errno_code_table (lua_State *L)
{
	lua_newtable (L);

	/* From socket(2). */
	add_errno_code (EACCES);
	add_errno_code (EAFNOSUPPORT);
	add_errno_code (EINVAL);
	add_errno_code (EMFILE);
	add_errno_code (ENFILE);
	add_errno_code (ENOBUFS);
	add_errno_code (ENOMEM);
	add_errno_code (EPROTONOSUPPORT);

	/* From accept(2). */
	add_errno_code (EAGAIN);
	add_errno_code (EWOULDBLOCK);
	add_errno_code (EBADF);
	add_errno_code (ECONNABORTED);
	add_errno_code (EFAULT);
	add_errno_code (EINTR);
	add_errno_code (EINVAL);
	add_errno_code (ENOTSOCK);
	add_errno_code (EOPNOTSUPP);
	add_errno_code (EPROTO);
	add_errno_code (EPERM);

	/* From bind(2). */
	add_errno_code (EADDRINUSE);
	add_errno_code (EADDRNOTAVAIL);
	add_errno_code (ELOOP);
	add_errno_code (ENAMETOOLONG);
	add_errno_code (ENOENT);
	add_errno_code (ENOTDIR);
	add_errno_code (EROFS);

	/* From connect(2). */
	add_errno_code (EALREADY);
	add_errno_code (ECONNREFUSED);
	add_errno_code (EINPROGRESS);
	add_errno_code (EISCONN);
	add_errno_code (ENETUNREACH);
	add_errno_code (ETIMEDOUT);

	/* From send(2). */
	add_errno_code (ECONNRESET);
	add_errno_code (EDESTADDRREQ);
	add_errno_code (EMSGSIZE);
	add_errno_code (ENOTCONN);
	add_errno_code (EPIPE);

	/* From ZeroMQ. */
	add_errno_code (ENODEV);
	add_errno_code (ENOTSUP);

	lua_setfield (L, LUA_REGISTRYINDEX, "ratchet_error_errno_code_table");
}
/* }}} */

/* {{{ push_errno_description() */
static void push_errno_description (lua_State *L, int e)
{
	if (e)
	{
		char errorbuf[512];
		if (strerror_r (e, errorbuf, sizeof (errorbuf)) == -1)
			lua_pushfstring (L, "Unknown error occurred. [errno=%d]", e);
		else
			lua_pushstring (L, errorbuf);
	}
	else
		lua_pushliteral (L, "Unknown error occurred.");
}
/* }}} */

/* ---- ratchet Functions --------------------------------------------------- */

/* {{{ ratchet_error_new() */
static int ratchet_error_new (lua_State *L)
{
	lua_settop (L, 7);

	lua_newtable (L);

	luaL_getmetatable (L, "ratchet_error_meta");
	lua_setmetatable (L, -2);

	/* Set the "thread" field to the current thread. */
	if (!lua_pushthread (L))
		lua_setfield (L, -2, "thread");
	else
		lua_pop (L, 1);

	/* Set the type field to indicate a ratchet error. */
	lua_pushliteral (L, "ratchet_error");
	lua_setfield (L, -2, "type");

	/* Set other fields from arguments. */
	luaL_checkstring (L, 1);
	lua_pushvalue (L, 1);
	lua_setfield (L, -2, "description");

	luaL_optstring (L, 2, NULL);
	lua_pushvalue (L, 2);
	lua_setfield (L, -2, "code");

	luaL_optstring (L, 3, NULL);
	lua_pushvalue (L, 3);
	lua_setfield (L, -2, "function");

	luaL_optstring (L, 4, NULL);
	lua_pushvalue (L, 4);
	lua_setfield (L, -2, "file");

	(void) luaL_optint (L, 5, -1);
	lua_pushvalue (L, 5);
	lua_setfield (L, -2, "line");

	luaL_optstring (L, 6, NULL);
	lua_pushvalue (L, 6);
	lua_setfield (L, -2, "syscall");

	(void) luaL_optint (L, 7, -1);
	lua_pushvalue (L, 7);
	lua_setfield (L, -2, "errno");

	return 1;
}
/* }}} */

/* ---- C API Functions ----------------------------------------------------- */

/* {{{ ratchet_error_push_constructor() */
void ratchet_error_push_constructor (lua_State *L)
{
	lua_pushcfunction (L, ratchet_error_new);
}
/* }}} */

/* {{{ ratchet_error_push_code() */
void ratchet_error_push_code (lua_State *L, int e)
{
	lua_getfield (L, LUA_REGISTRYINDEX, "ratchet_error_errno_code_table");
	lua_rawgeti (L, -1, e);
	lua_remove (L, -2);
}
/* }}} */

/* {{{ ratchet_error_errno_ln() */
int ratchet_error_errno_ln (lua_State *L, const char *function, const char *syscall, const char *file, int line)
{
	int e = errno;

	lua_settop (L, 0);

	ratchet_error_push_constructor (L);
	push_errno_description (L, e);
	ratchet_error_push_code (L, e);
	lua_pushstring (L, function);
	lua_pushstring (L, file);
	lua_pushinteger (L, line);
	lua_pushstring (L, syscall);
	lua_pushinteger (L, e);
	lua_call (L, 7, 1);

	return lua_error (L);
}
/* }}} */

/* {{{ ratchet_error_top_ln() */
int ratchet_error_top_ln (lua_State *L, const char *function, const char *code, const char *file, int line)
{
	ratchet_error_push_constructor (L);
	lua_pushvalue (L, -2);
	lua_pushstring (L, code);
	lua_pushstring (L, function);
	lua_pushstring (L, file);
	lua_pushinteger (L, line);
	lua_pushnil (L);
	lua_pushnil (L);
	lua_call (L, 7, 1);

	return lua_error (L);
}
/* }}} */

/* {{{ ratchet_error_str_ln() */
int ratchet_error_str_ln (lua_State *L, const char *function, const char *code, const char *file, int line, const char *description, ...)
{
	lua_settop (L, 0);

	va_list args;
	va_start (args, description);
	lua_pushvfstring (L, description, args);
	va_end (args);

	return ratchet_error_top_ln (L, function, code, file, line);
}
/* }}} */

/* ---- ratchet Methods ----------------------------------------------------- */

/* {{{ ratchet_error_is() */
static int ratchet_error_is (lua_State *L)
{
	lua_settop (L, 2);
	if (lua_isstring (L, 1))
	{
		lua_pushboolean (L, lua_compare (L, 1, 2, LUA_OPEQ));
		return 1;
	}

	lua_getfield (L, 1, "type");
	if (!strequal (L, -1, "ratchet_error"))
	{
		lua_pop (L, 1);
		lua_pushboolean (L, 0);
		return 1;
	}
	lua_pop (L, 1);

	lua_getfield (L, 1, "code");
	lua_pushboolean (L, lua_compare (L, -1, 2, LUA_OPEQ));

	return 1;
}
/* }}} */

/* {{{ ratchet_error_get_string() */
static int ratchet_error_get_string (lua_State *L)
{
	lua_settop (L, 1);

	lua_getfield (L, 1, "description");
	const char *description = lua_tostring (L, -1);
	lua_getfield (L, 1, "file");
	const char *file = lua_tostring (L, -1);
	lua_getfield (L, 1, "line");
	int line = lua_tointeger (L, -1);
	lua_getfield (L, 1, "function");
	const char *func = lua_tostring (L, -1);
	
	if (func)
		lua_pushfstring (L, "%s: %s", func, description);
	else if (file)
		lua_pushfstring (L, "%s:%d: %s", file, line, description);
	else
		lua_pushstring (L, description);
	lua_replace (L, 2);
	lua_settop (L, 2);

	return 1;
}
/* }}} */

/* {{{ ratchet_error_tostring() */
static int ratchet_error_tostring (lua_State *L)
{
	lua_settop (L, 1);

	lua_getfield (L, 1, "thread");
	lua_State *L1 = lua_tothread (L, -1);

	lua_getfield (L, 1, "get_string");
	lua_pushvalue (L, 1);
	lua_call (L, 1, 1);
	const char *msg = lua_tostring (L, -1);

	if (L1)
		luaL_traceback (L, L1, msg, 1);
	return 1;
}
/* }}} */

/* ---- Public Functions ---------------------------------------------------- */

/* {{{ luaopen_ratchet_error() */
int luaopen_ratchet_error (lua_State *L)
{
	const luaL_Reg funcs[] = {
		{"new", ratchet_error_new},
		{"is", ratchet_error_is},
		{NULL}
	};

	const luaL_Reg meths[] = {
		/* Documented methods. */
		{"is", ratchet_error_is},
		{"get_string", ratchet_error_get_string},
		/* Undocumented, helper methods. */
		{NULL}
	};

	const luaL_Reg metameths[] = {
		{"__tostring", ratchet_error_tostring},
		{NULL}
	};

	luaL_newlib (L, funcs);
	lua_pushvalue (L, -1);
	lua_setfield (L, LUA_REGISTRYINDEX, "ratchet_error_class");

	luaL_newmetatable (L, "ratchet_error_meta");
	lua_newtable (L);
	luaL_setfuncs (L, meths, 0);
	lua_setfield (L, -2, "__index");
	luaL_setfuncs (L, metameths, 0);
	lua_pop (L, 1);

	build_errno_code_table (L);

	return 1;
}
/* }}} */

// vim:fdm=marker:ai:ts=4:sw=4:noet:
