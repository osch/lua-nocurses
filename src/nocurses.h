/*
  nocurses.h - Provides a basic 'VT100 ESC sequences' printing without
               the need to use ncurses. 
               This is inspired by Borland conio.h

  Author     - Rahul M. Juliato
  Original   - 25 jun 2005
  Revision   -    oct 2019
*/

#include <stdio.h>
#ifdef _WIN32
# include <windows.h>
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__) || defined(__linux__)
# ifndef __unix__
#  define __unix__
# endif
# include <sys/ioctl.h>
# include <termios.h>
# include <unistd.h>
#endif

#define ESC    "\x1b"

/**************************************************************************************************/

#define SEQUENCE_DEFINES \
    \
    SEQ_DEF( clear_screen,             ESC"[2J"               ) \
    SEQ_DEF( clear_to_eol,             ESC"[0K"               ) \
    SEQ_DEF( clear_to_eos,             ESC"[0J"               ) \
    \
    SEQ_DEF( use_alt_buff,             ESC"[?1049h"           ) \
    SEQ_DEF( use_main_buff,            ESC"[?1049l"           ) \
    \
    SEQ_DEF( goto_row_col,             ESC"[%d;%dH"           ) \
    SEQ_DEF( goto_col,                 ESC"[%dG"              ) \
    \
    SEQ_DEF( go_home,                  ESC"[H"                )\
    SEQ_DEF( go_up,                    ESC"[%dA"              ) \
    SEQ_DEF( go_down,                  ESC"[%dB"              ) \
    SEQ_DEF( go_right,                 ESC"[%dC"              ) \
    SEQ_DEF( go_left,                  ESC"[%dD"              ) \
    \
    SEQ_DEF( clear_line,               ESC"[2K"               ) \
    \
    SEQ_DEF( attrs_begin,              ESC"["                 ) \
    SEQ_DEF( attrs_next,               ";"                    ) \
    SEQ_DEF( attrs_end,                "m"                    ) \
    \
    SEQ_DEF( attr_bold,                "1"                    ) \
    SEQ_DEF( attr_dim,                 "2"                    ) \
    SEQ_DEF( attr_italic,              "3"                    ) \
    SEQ_DEF( attr_underline,           "4"                    ) \
    SEQ_DEF( attr_blink,               "5"                    ) \
    SEQ_DEF( attr_inverse,             "7"                    ) \
    \
    SEQ_DEF( attr_reset,               "0"                    ) \
    \
    SEQ_DEF( attr_foregrd_black,       "30"                   ) \
    SEQ_DEF( attr_foregrd_red,         "31"                   ) \
    SEQ_DEF( attr_foregrd_green,       "32"                   ) \
    SEQ_DEF( attr_foregrd_yellow,      "33"                   ) \
    SEQ_DEF( attr_foregrd_blue,        "34"                   ) \
    SEQ_DEF( attr_foregrd_magenta,     "35"                   ) \
    SEQ_DEF( attr_foregrd_cyan,        "36"                   ) \
    SEQ_DEF( attr_foregrd_white,       "37"                   ) \
    SEQ_DEF( attr_foregrd_default,     "39"                   ) \
    \
    SEQ_DEF( attr_backgrd_black,       "40"                   ) \
    SEQ_DEF( attr_backgrd_red,         "41"                   ) \
    SEQ_DEF( attr_backgrd_green,       "42"                   ) \
    SEQ_DEF( attr_backgrd_yellow,      "43"                   ) \
    SEQ_DEF( attr_backgrd_blue,        "44"                   ) \
    SEQ_DEF( attr_backgrd_magenta,     "45"                   ) \
    SEQ_DEF( attr_backgrd_cyan,        "46"                   ) \
    SEQ_DEF( attr_backgrd_white,       "47"                   ) \
    SEQ_DEF( attr_backgrd_default,     "49"                   ) \
    \
    SEQ_DEF( reset_attrs,              ESC"[0m"               ) \
    \
    SEQ_DEF( set_foregrd_color,        ESC"[3%dm"             ) \
    SEQ_DEF( set_backgrd_color,        ESC"[4%dm"             ) \
    SEQ_DEF( set_attr_bold,            ESC"[1m"               ) \
    SEQ_DEF( set_attr_underline,       ESC"[4m"               ) \
    SEQ_DEF( set_attr_blink,           ESC"[5m"               ) \
    SEQ_DEF( set_attr_inverse,         ESC"[7m"               ) \
    \
    SEQ_DEF( set_title,                        ESC"]0;%s\x7"          ) \
    \
    SEQ_DEF( set_cur_shape,                    ESC"[%d q"              ) \
    SEQ_DEF( set_cur_shape_default,            ESC"[0 q"               ) \
    SEQ_DEF( set_cur_shape_block_blink,        ESC"[1 q"               ) \
    SEQ_DEF( set_cur_shape_block,              ESC"[2 q"               ) \
    SEQ_DEF( set_cur_shape_underline_blink,    ESC"[3 q"               ) \
    SEQ_DEF( set_cur_shape_underline,          ESC"[4 q"               ) \
    SEQ_DEF( set_cur_shape_bar_blink,          ESC"[5 q"               ) \
    SEQ_DEF( set_cur_shape_bar,                ESC"[6 q"               ) \
    \
    SEQ_DEF( show_cur,                         ESC"[?25h"             ) \
    SEQ_DEF( hide_cur,                         ESC"[?25l"             ) \
    \
    SEQ_DEF( request_cur_pos,                  ESC"[6n"               ) \
    SEQ_DEF( response_cur_pos,                 ESC"%[(%d+);(%d+)R"    ) \
    
    

/**************************************************************************************************/

#define SEQ(n) SEQ_##n

#define SEQ_DEF(n,v)  static const char SEQ_##n[] = v;
    SEQUENCE_DEFINES
#undef SEQ_DEF

/**************************************************************************************************/

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

#define BLOCK_BLINK     1
#define BLOCK           2
#define UNDERLINE_BLINK 3
#define UNDERLINE       4
#define BAR_BLINK       5
#define BAR             6

#define TRUE    1
#define FALSE   0

struct termsize {
    int cols;
    int rows;
};


static int bg_color     = 9,
         font_color     = 9,
         font_bold      = 0,
         font_underline = 0,
         font_blink     = 0,
         font_invert    = 0;


#if 0
static void wait(){
    while (fgetc(stdin) != '\n');
}
#endif

static void clrscr(){
    printf(SEQ(clear_screen));
}


static void gotoxy(int x, int y){
    printf(SEQ(goto_row_col), y, x);
}


static void setfontcolor(int color){
    printf(SEQ(set_foregrd_color), color);
    font_color = color;
}

static void setbgrcolor(int color){
    printf(SEQ(set_backgrd_color), color);
    bg_color = color;
}

static void setunderline(int status);
static void setblink(int status);
static void setinvert(int status);

static void setfontbold(int status){
    font_bold = status;
    if (font_bold) {
        printf(SEQ(set_attr_bold));
    } else {
        printf(SEQ(reset_attrs));
        if (font_color >= 0) setfontcolor(font_color);
        if (bg_color   >= 0) setbgrcolor(bg_color);
        if (font_underline)  setunderline(font_underline);
        if (font_blink)      setblink(font_blink);
        if (font_invert)     setinvert(font_invert);
    }
}

static void setunderline(int status){
    font_underline = status;
    if (font_underline) {
        printf(SEQ(set_attr_underline));
    } else {
        printf(SEQ(reset_attrs));
        if (font_color >= 0) setfontcolor(font_color);
        if (bg_color   >= 0) setbgrcolor(bg_color);
        if (font_bold)       setfontbold(font_bold);
        if (font_blink)      setblink(font_blink);
        if (font_invert)     setinvert(font_invert);
    }
}

static void setblink(int status){
    font_blink = status;
    if (font_blink) {
        printf(SEQ(set_attr_blink));
    } else {
        printf(SEQ(reset_attrs));
        if (font_color >= 0) setfontcolor(font_color);
        if (bg_color   >= 0) setbgrcolor(bg_color);
        if (font_bold)       setfontbold(font_bold);
        if (font_underline)  setunderline(font_underline);
        if (font_invert)     setinvert(font_invert);
    }
}

static void setinvert(int status){
    font_invert = status;
    if (font_invert) {
        printf(SEQ(set_attr_inverse));
    } else {
        printf(SEQ(reset_attrs));
        if (font_color >= 0) setfontcolor(font_color);
        if (bg_color   >= 0) setbgrcolor(bg_color);
        if (font_bold)       setfontbold(font_bold);
        if (font_underline)  setunderline(font_underline);
        if (font_blink)      setblink(font_blink);
    }
}

static void settitle(char const* title) {
    printf(SEQ(set_title), title);
}

static void setcurshape(int shape){
    // vt520/xterm-style; linux terminal uses ESC[?1;2;3c, not implemented
    printf(SEQ(set_cur_shape), shape);
}

static struct termsize gettermsize(){
    struct termsize size;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    size.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elif defined(__unix__)
    struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    size.cols = win.ws_col;
    size.rows = win.ws_row;
#else
    size.cols = 0;
    size.rows = 0;
#endif
    return size;
}

static bool isRaw = false;
#if defined(__unix__)
static struct termios oldattr;
#endif

static void setRaw(bool raw) 
{
    if (isRaw != raw) {
        if (raw) {
        #ifdef _WIN32
            HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
            if (input == NULL) return EOF;
        
            DWORD oldmode;
            GetConsoleMode(input, &oldmode);
            DWORD newmode = oldmode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
            SetConsoleMode(input, newmode);
        #elif defined(__unix__)
            struct termios newattr;
            tcgetattr(STDIN_FILENO, &oldattr);
        
            newattr = oldattr;
            newattr.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
        #endif
        }
        else {
        #ifdef _WIN32
            SetConsoleMode(input, oldmode);
        #elif defined(__unix__)
            tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
        #endif
        }
        isRaw = raw;
    }
}

static int getch(){
    bool switchRaw = !isRaw;
    if (switchRaw) {
        setRaw(true);
    }
    int ch = getc(stdin);
    
    if (switchRaw) {
        setRaw(false);
    }
    return ch;
}


static void clrline(){
    printf(SEQ(clear_line));
}

static void resetcolors(){
//    printf(ESC"001b");
    printf(SEQ(reset_attrs));
    bg_color       = 9;
    font_color     = 9;
    font_bold      = 0;
    font_underline = 0;
    font_blink     = 0;
    font_invert    = 0;
}

static void showcursor(){
    printf(SEQ(show_cur));
}

static void hidecursor(){
    printf(SEQ(hide_cur));
}
