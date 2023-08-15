#include "nocurses.h"

#if defined(__unix__)
# include <sys/select.h>
# include <sys/time.h>
# include <fcntl.h>
# include <signal.h>
#endif

#include "main.h"

/* ============================================================================================ */

#define NOTIFY_CAPI_IMPLEMENT_SET_CAPI 1
#include "notify_capi.h"

/* ============================================================================================ */

#ifndef NOCURSES_VERSION
    #error NOCURSES_VERSION is not defined
#endif 

#define NOCURSES_STRINGIFY(x) #x
#define NOCURSES_TOSTRING(x) NOCURSES_STRINGIFY(x)
#define NOCURSES_VERSION_STRING NOCURSES_TOSTRING(NOCURSES_VERSION)

/* ============================================================================================ */

static const char* const NOCURSES_MODULE_NAME = "nocurses";

/* ============================================================================================ */

static AtomicCounter initStage = 0;
static void*         udata = NULL;

#define NC_READBUFLEN 256

#if defined(__unix__)    

static int            nc_awake_fds[2];
static unsigned char  nc_readbuffer[NC_READBUFLEN];
static size_t         nc_readlen = 0;
static size_t         nc_readpos = 0;
static bool           nc_hidecur = 0;

#endif /* __unix__ */



/* ============================================================================================ */

static int handleClosingLuaState(lua_State* L)
{
    bool restricted = true;
    if (lua_rawgetp(L, LUA_REGISTRYINDEX, NOCURSES_MODULE_NAME) == LUA_TUSERDATA) {
        if (lua_touserdata(L, -1) == udata) {
            restricted = false;
        }
    }
    lua_pop(L, 1);
    if (!restricted) {
        if (atomic_set_if_equal(&initStage, 1, 0)) {
            if (nc_hidecur) {
                nc_hidecur = false;
                showcursor();
                fflush(stdout);
            }
        }
    }
    return 0;
}

/* ============================================================================================ */

#if defined(__unix__)

static void sendAwake()
{
    if (nc_awake_fds[0] >= 0) {
      char c = 0;
      if (write(nc_awake_fds[1], &c, 1) != 1) {
        // ignore
      }
    }
}

static void handleSwinch(int sig)
{
    sendAwake();
}

static void initAwake()
{
    if (pipe(nc_awake_fds) == 0) {
        fcntl(nc_awake_fds[1],
              F_SETFL,
              fcntl(nc_awake_fds[1], F_GETFL) | O_NONBLOCK);
        signal(SIGWINCH, handleSwinch);  /* set C-signal handler */
    } else {
        nc_awake_fds[0] = -1;
    }
}

static bool drainAwakePipe()
{
    bool hasAwake = false;
    const int afd  = nc_awake_fds[0];
    if (afd >= 0) {
        fd_set fds;
        int    nfds = afd + 1;
        FD_ZERO(&fds);
        FD_SET(afd, &fds);
        struct timeval tv  = {0, 0};
        int            ret = select(nfds, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(afd, &fds)) {
            hasAwake = true;
            char buf[128];
            if (read(afd, buf, sizeof(buf)) < 0) {
                // ignore
            }
        }
    }
    return hasAwake;
}

static bool hasInputAt(int i)
{
    return (nc_readpos + i < nc_readlen);
}

static void clearInput()
{
    nc_readpos = 0;
    nc_readlen = 0;
}

static bool hasInput()
{
    return (nc_readpos < nc_readlen);
}

static bool waitForInput(const double timeout)
{
    if (drainAwakePipe()) {
        return false;
    }

    const int ifd  = STDIN_FILENO;
    const int afd  = nc_awake_fds[0];
    int       nfds = (ifd > afd ? ifd : afd) + 1;

    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);

    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    int       ret  = 0;
    fd_set    fds;
    FD_ZERO(&fds);
    FD_SET(ifd, &fds);
    if (afd >= 0) {
        FD_SET(afd, &fds);
    }
    if (timeout < 0.0) {
       ret = select(nfds, &fds, NULL, NULL, NULL);
    } else {
        const long     sec  = (long)timeout;
        const long     usec = (long)((timeout - (double)sec) * 1e6);
        struct timeval tv   = {sec, usec};
        ret                 = select(nfds, &fds, NULL, NULL, &tv);
    }
    bool hasSignal = (ret == -1) && (errno == EINTR);
    bool hasInp    = (ret > 0) && (FD_ISSET(ifd, &fds));
    bool hasAwake  = (ret > 0) && (afd >= 0) && (FD_ISSET(afd, &fds));
    if (hasAwake || hasSignal) {
        drainAwakePipe();
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return hasInp;
}

#endif /* __unix__ */

/* ============================================================================================ */

static int Nocurses_wait(lua_State* L)
{
    clearInput();
#if defined(__unix__)
    while (true) {
        if (waitForInput(-1)) {
            if (fgetc(stdin) == '\n') {
                break;
            }
        } else {
            break;
        }
    }
#else
    while (fgetc(stdin) != '\n');
#endif
    return 0;
}

/* ============================================================================================ */

static int Nocurses_clrscr(lua_State* L)
{
    clrscr();
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_gotoxy(lua_State* L)
{
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    gotoxy(x, y);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static const char* const colors[] =
{
   "BLACK",
   "RED",
   "GREEN",
   "YELLOW",
   "BLUE",
   "MAGENTA",
   "CYAN",
   "WHITE",
    NULL
};

/* ============================================================================================ */

static int Nocurses_setfontcolor(lua_State* L)
{
    int color = luaL_checkoption(L, 1, NULL, colors);
    setfontcolor(color);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_setbgrcolor(lua_State* L)
{
    int color = luaL_checkoption(L, 1, NULL, colors);
    setbgrcolor(color);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_setfontbold(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int flag = lua_toboolean(L, 1);
    setfontbold(flag);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_setunderline(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int flag = lua_toboolean(L, 1);
    setunderline(flag);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_setblink(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int flag = lua_toboolean(L, 1);
    setblink(flag);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_setinvert(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int flag = lua_toboolean(L, 1);
    setinvert(flag);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_settitle(lua_State* L)
{
    const char* title = luaL_checkstring(L, 1);
    settitle(title);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static const char* const shapes[] =
{
    "BLOCK_BLINK",
    "BLOCK",
    "UNDERLINE_BLINK",
    "UNDERLINE",
    "BAR_BLINK",
    "BAR",
    NULL
};

static int Nocurses_setcurshape(lua_State* L)
{
    int shape = 1 + luaL_checkoption(L, 1, NULL, shapes);
    setcurshape(shape);
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_gettermsize(lua_State* L)
{
    struct termsize s = gettermsize();
    lua_pushinteger(L, s.cols);
    lua_pushinteger(L, s.rows);
    return 2;
}

/* ============================================================================================ */

#if defined(__unix__)
static int nc_getch()
{
    if (nc_readpos < nc_readlen) {
        return nc_readbuffer[nc_readpos++];
    }
    size_t len = read(fileno(stdin), nc_readbuffer, NC_READBUFLEN);
    if (len > 0) {
        nc_readpos = 1;
        nc_readlen = len;
        return nc_readbuffer[0];
    }
    else {
        clearInput();
        return -1;
    }
}

static int nc_peekch(int offs)
{
    if (nc_readpos + offs < nc_readlen) {
        return nc_readbuffer[nc_readpos + offs];
    }
    if (nc_readpos > 0 && nc_readpos < nc_readlen) {
        memmove(nc_readbuffer, nc_readbuffer + nc_readpos, nc_readlen - nc_readpos);
        nc_readlen -= nc_readpos;
        nc_readpos = 0;
    } else {
        nc_readlen = 0;
        nc_readpos = 0;
    }   
    if (nc_readlen < NC_READBUFLEN) {
        size_t len = read(fileno(stdin), nc_readbuffer + nc_readlen, NC_READBUFLEN - nc_readlen);
        if (len > 0) {
            nc_readlen += len;
        }
    }
    if (nc_readpos + offs < nc_readlen) {
        return nc_readbuffer[nc_readpos + offs];
    }
    else {
        return -1;
    }
}

static int nc_skipch(int skip)
{
    if (nc_readpos + skip <= nc_readlen) {
        nc_readpos += skip;
        return skip;
    }
    if (nc_readpos > 0 && nc_readpos < nc_readlen) {
        memmove(nc_readbuffer, nc_readbuffer + nc_readpos, nc_readlen - nc_readpos);
        nc_readlen -= nc_readpos;
        nc_readpos = 0;
    } else {
        nc_readlen = 0;
        nc_readpos = 0;
    }   
    if (nc_readlen < NC_READBUFLEN) {
        size_t len = read(fileno(stdin), nc_readbuffer + nc_readlen, NC_READBUFLEN - nc_readlen);
        if (len > 0) {
            nc_readlen += len;
        }
    }
    if (nc_readpos + skip <= nc_readlen) {
        nc_readpos += skip;
        return skip;
    }
    else {
        skip = nc_readlen - nc_readpos;
        nc_readpos = 0;
        nc_readlen = 0;
        return skip;
    }
}
#endif

/* ============================================================================================ */

static int assureUnrestricted(lua_State* L)
{
    bool restricted = true;
    if (lua_rawgetp(L, LUA_REGISTRYINDEX, NOCURSES_MODULE_NAME) == LUA_TUSERDATA) { // -> nocurses
        if (lua_touserdata(L, -1) == udata) {                                       // -> nocurses
            restricted = false;
        }
    }
    lua_pop(L, 1); // -> 
    if (restricted) {
        return luaL_error(L, "function must be called from main thread");
    }
    else {
        return 0;
    }
}

/* ============================================================================================ */

static int Nocurses_getch(lua_State* L)
{
    assureUnrestricted(L);

    double timeout = -1;
    if (!lua_isnoneornil(L, 1)) {
        timeout = luaL_checknumber(L, 1);
        if (timeout < 0){
            timeout = 0;
        }
    }
#if defined(__unix__)
    bool hasInp = hasInput() || waitForInput(timeout);
    if (hasInp) {
        int c = nc_getch();
        if (c >= 0) {
            lua_pushinteger(L, c);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
#else
    lua_pushinteger(L, getch());
#endif
    return 1;
}

/* ============================================================================================ */

static int Nocurses_peekch(lua_State* L)
{
    assureUnrestricted(L);

    int offs = 0;
    if (!lua_isnoneornil(L, 1)) {
        offs = luaL_checkinteger(L, 1) - 1;
    }
    if (offs < 0) {
        lua_pushnil(L);
        return 1;
    }
#if defined(__unix__)
    bool hasInp = hasInputAt(offs) || waitForInput(0 /* timeout */);
    if (hasInp) {
        int c = nc_peekch(offs);
        if (c >= 0) {
            lua_pushinteger(L, c);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
#else
    lua_pushnil(L);
#endif
    return 1;
}

/* ============================================================================================ */

static int Nocurses_skipch(lua_State* L)
{
    assureUnrestricted(L);

    int skip = 1;
    if (!lua_isnoneornil(L, 1)) {
        skip = luaL_checkinteger(L, 1) ;
    }
    if (skip <= 0) {
        lua_pushinteger(L, 0);
        return 1;
    }
#if defined(__unix__)
    bool hasInp = hasInputAt(skip - 1) || waitForInput(0 /* timeout */);
    if (hasInp) {
        int skipped = nc_skipch(skip);
        lua_pushinteger(L, skipped);
    } else {
        lua_pushinteger(L, 0);
    }
#else
    lua_pushnil(L);
#endif
    return 1;
}

/* ============================================================================================ */

static int Nocurses_clrline(lua_State* L)
{
    clrline();
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_resetcolors(lua_State* L)
{
    resetcolors();
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

static int Nocurses_showcursor(lua_State* L)
{
    assureUnrestricted(L);
    
    nc_hidecur = false;
    showcursor();
    fflush(stdout);

    return 0;
}

/* ============================================================================================ */

static int Nocurses_hidecursor(lua_State* L)
{
    assureUnrestricted(L);

    nc_hidecur = true;
    hidecursor();
    fflush(stdout);
    return 0;
}

/* ============================================================================================ */

#if defined(__unix__)    

static int Nocurses_awake(lua_State* L)
{
    sendAwake();
    return 0;
}

static notify_notifier* toNotifier(lua_State* L, int index)
{
    notify_notifier* rslt = NULL;
    if (lua_getmetatable(L, index))
    {                                                      /* -> meta1 */
        if (luaL_getmetatable(L, NOCURSES_MODULE_NAME)
            != LUA_TNIL)                                   /* -> meta1, meta2 */
        {
            if (lua_rawequal(L, -1, -2)) {                 /* -> meta1, meta2 */
                rslt = (notify_notifier*)NOCURSES_MODULE_NAME;
            }
        }                                                  /* -> meta1, meta2 */
        lua_pop(L, 2);                                     /* -> */
    }                                                      /* -> */
    return rslt;
}

static void retainNotifier(notify_notifier* n)
{}

static void releaseNotifier(notify_notifier* n)
{}

static int notify(notify_notifier* n, notifier_error_handler eh, void* ehdata)
{
    sendAwake();
    return 0;
}

static struct notify_capi notify_capi_impl = 
{
    NOTIFY_CAPI_VERSION_MAJOR,
    NOTIFY_CAPI_VERSION_MINOR,
    NOTIFY_CAPI_VERSION_PATCH,
    
    NULL,
    
    toNotifier,
    retainNotifier,
    releaseNotifier,
    notify
};

#endif /* __unix__ */

/* ============================================================================================ */

static const luaL_Reg RestrictedModuleFunctions[] = 
{
#if defined(__unix__)    
    { "awake",          Nocurses_awake },
#endif
    { NULL,             NULL           } /* sentinel */
};

static const luaL_Reg ModuleFunctions[] = 
{
    { "wait",           Nocurses_wait         },
    { "clrscr",         Nocurses_clrscr       },
    { "gotoxy",         Nocurses_gotoxy       },
    { "setfontcolor",   Nocurses_setfontcolor },
    { "setbgrcolor",    Nocurses_setbgrcolor  },
    { "setfontbold",    Nocurses_setfontbold  },
    { "setunderline",   Nocurses_setunderline },
    { "setblink",       Nocurses_setblink     },
    { "setinvert",      Nocurses_setinvert    },
    { "settitle",       Nocurses_settitle     },
    { "setcurshape",    Nocurses_setcurshape  },
    { "gettermsize",    Nocurses_gettermsize  },
    { "getch",          Nocurses_getch        },
    { "peekch",         Nocurses_peekch       },
    { "skipch",         Nocurses_skipch       },
    { "clrline",        Nocurses_clrline      },
    { "resetcolors",    Nocurses_resetcolors  },
    { "showcursor",     Nocurses_showcursor   },
    { "hidecursor",     Nocurses_hidecursor   },
#if defined(__unix__)    
    { "awake",          Nocurses_awake        },
#endif
    { NULL,             NULL           } /* sentinel */
};

/* ============================================================================================ */


DLL_PUBLIC int luaopen_nocurses(lua_State* L)
{
    luaL_checkversion(L); /* does nothing if compiled for Lua 5.1 */

    /* ---------------------------------------- */

    bool restricted = true;
    if (atomic_set_if_equal(&initStage, 0, 1)) {
        restricted = false;
    #if defined(__unix__)
        initAwake();
    #endif
        udata = lua_newuserdata(L, 1);                              /* -> sentinel */
        lua_newtable(L);                                            /* -> sentinel, metatable */
        lua_pushstring(L, "__gc");                                  /* -> sentinel, metatable, "__gc", callback */
        lua_pushcfunction(L, handleClosingLuaState);                /* -> sentinel, metatable */
        lua_rawset(L, -3);                                          /* -> sentinel, metatable */
        lua_setmetatable(L, -2);                                    /* -> sentinel */
        lua_rawsetp(L, LUA_REGISTRYINDEX, NOCURSES_MODULE_NAME);    /* -> */
    }
    else {
        if (lua_rawgetp(L, LUA_REGISTRYINDEX, NOCURSES_MODULE_NAME) == LUA_TUSERDATA) {
            if (lua_touserdata(L, -1) == udata) {
                restricted = false;
            }
        }
        lua_pop(L, 1);
    }

    /* ---------------------------------------- */

    int n = lua_gettop(L);
    
    int module      = ++n; lua_newtable(L);

    lua_pushvalue(L, module);            /* --> module */
    if (restricted) {
        luaL_setfuncs(L, RestrictedModuleFunctions, 0);
    } else {
        luaL_setfuncs(L, ModuleFunctions, 0);
    }
    lua_pop(L, 1);
    
    lua_pushliteral(L, NOCURSES_VERSION_STRING);
    lua_setfield(L, module, "_VERSION");
    
    lua_checkstack(L, LUA_MINSTACK);

    luaL_newmetatable(L, NOCURSES_MODULE_NAME);        /* -> meta */
#if defined(__unix__)    
    notify_set_capi(L, -1, &notify_capi_impl);         /* -> meta */
#endif
    lua_pushstring(L, NOCURSES_MODULE_NAME);           /* -> meta, "nocurses" */
    lua_setfield(L, -2, "__metatable");                /* -> meta */
    if (!restricted) {
        int rc = luaL_loadstring(L,                                 
            "local t, k = ...\n"
            "if type(k) == 'string' then\n"
            "   local name = 'nocurses.'..k\n"
            "   local found = package.loaded[name]\n"
            "   if not found then\n"
            "       for i, searcher in ipairs(package.searchers or package.loaders) do\n"
            "           local loader, data = searcher(name)\n"
            "           if type(loader) == 'function' then\n"
            "               found = loader(data or name)\n"
            "           end\n"
            "       end\n"
            "   end\n"
            "   if found then\n"
            "       t[k] = found\n"
            "       return found\n"
            "   end\n"
            "end\n"
        );                                             /* -> meta, indexFunc */
        if (rc != 0) {
            return lua_error(L);
        }
        lua_setfield(L, -2, "__index");                /* -> meta */
    }
    lua_setmetatable(L, module);                       /* -> */
    
    lua_settop(L, module);
    return 1;
}

/* ============================================================================================ */

