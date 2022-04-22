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

#if defined(__unix__)    

static int  awake_fds[2];

#endif /* __unix__ */

/* ============================================================================================ */

#if defined(__unix__)

static void sendAwake()
{
    if (awake_fds[0] >= 0) {
      char c = 0;
      write(awake_fds[1], &c, 1);
    }
}

static void handleSwinch(int sig)
{
    sendAwake();
}

static void initAwake()
{
    if (pipe(awake_fds) == 0) {
        fcntl(awake_fds[1],
              F_SETFL,
              fcntl(awake_fds[1], F_GETFL) | O_NONBLOCK);
        signal(SIGWINCH, handleSwinch);  /* set C-signal handler */
    } else {
        awake_fds[0] = -1;
    }
}

static bool drainAwakePipe()
{
    bool hasAwake = false;
    const int afd  = awake_fds[0];
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
            read(afd, buf, sizeof(buf));
        }
    }
    return hasAwake;
}


static bool waitForInput(const double timeout)
{
    if (drainAwakePipe()) {
        return false;
    }

    const int ifd  = STDIN_FILENO;
    const int afd  = awake_fds[0];
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
    bool hasInput  = (ret > 0) && (FD_ISSET(ifd, &fds));
    bool hasAwake  = (ret > 0) && (afd >= 0) && (FD_ISSET(afd, &fds));
    if (hasAwake || hasSignal) {
        drainAwakePipe();
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return hasInput;
}

#endif /* __unix__ */

/* ============================================================================================ */

static int Nocurses_wait(lua_State* L)
{
#if defined(__unix__)
    bool wasEnter = false;
    while (true) {
        if (waitForInput(-1)) {
            if (fgetc(stdin) == '\n') {
                break;
            }
        } else {
            break;
        }
    }
    lua_pushboolean(L, wasEnter);
#else
    while (fgetc(stdin) != '\n');
    lua_pushboolean(L, true);
#endif
    return 1;
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

static int Nocurses_getch(lua_State* L)
{
    double timeout = -1;
    if (!lua_isnoneornil(L, 1)) {
        timeout = luaL_checknumber(L, 1);
        if (timeout < 0){
            timeout = 0;
        }
    }
#if defined(__unix__)
    bool hasInput = waitForInput(timeout);
    if (hasInput) {
        lua_pushinteger(L, getch());
    } else {
        lua_pushnil(L);
    }
#else
    lua_pushinteger(L, getch());
#endif
    return 1;
}

/* ============================================================================================ */

static int Nocurses_getche(lua_State* L)
{
    double timeout = -1;
    if (!lua_isnoneornil(L, 1)) {
        timeout = luaL_checknumber(L, 1);
        if (timeout < 0){
            timeout = 0;
        }
    }
#if defined(__unix__)
    bool hasInput = waitForInput(timeout);
    if (hasInput) {
        lua_pushinteger(L, getche());
    } else {
        lua_pushnil(L);
    }
#else
    lua_pushinteger(L, getche());
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
    { "getche",         Nocurses_getche       },
    { "clrline",        Nocurses_clrline      },
    { "resetcolors",    Nocurses_resetcolors  },
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

    bool unrestricted = false;
    if (atomic_set_if_equal(&initStage, 0, 1)) {
        unrestricted = true;
    #if defined(__unix__)
        initAwake();
    #endif
    }

    /* ---------------------------------------- */

    int n = lua_gettop(L);
    
    int module      = ++n; lua_newtable(L);

    lua_pushvalue(L, module);
    if (unrestricted) {
        luaL_setfuncs(L, ModuleFunctions, 0);
    } else {
        luaL_setfuncs(L, RestrictedModuleFunctions, 0);
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
    lua_setmetatable(L, module);                       /* -> */
    
    lua_settop(L, module);
    return 1;
}

/* ============================================================================================ */

