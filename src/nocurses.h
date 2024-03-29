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
    printf(ESC"[2J"ESC"[?6h");
}


static void gotoxy(int x, int y){
    printf(ESC"[%d;%dH", y, x);
}


static void setfontcolor(int color){
    printf(ESC"[3%dm", color);
    font_color = color;
}

static void setbgrcolor(int color){
    printf(ESC"[4%dm", color);
    bg_color = color;
}

static void setunderline(int status);
static void setblink(int status);
static void setinvert(int status);

static void setfontbold(int status){
    font_bold = status;
    if (font_bold) {
        printf(ESC"[1m");
    } else {
        printf(ESC"[0m");
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
        printf(ESC"[4m");
    } else {
        printf(ESC"[0m");
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
        printf(ESC"[5m");
    } else {
        printf(ESC"[0m");
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
        printf(ESC"[7m");
    } else {
        printf(ESC"[0m");
        if (font_color >= 0) setfontcolor(font_color);
        if (bg_color   >= 0) setbgrcolor(bg_color);
        if (font_bold)       setfontbold(font_bold);
        if (font_underline)  setunderline(font_underline);
        if (font_blink)      setblink(font_blink);
    }
}

static void settitle(char const* title) {
    printf(ESC"]0;%s\x7", title);
}

static void setcurshape(int shape){
    // vt520/xterm-style; linux terminal uses ESC[?1;2;3c, not implemented
    printf(ESC"[%d q", shape);
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

static int getch(){
#ifdef _WIN32
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == NULL) return EOF;

    DWORD oldmode;
    GetConsoleMode(input, &oldmode);
    DWORD newmode = oldmode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(input, newmode);
#elif defined(__unix__)
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);

    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
#endif

    int ch = getc(stdin);

#ifdef _WIN32
    SetConsoleMode(input, oldmode);
#elif defined(__unix__)
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
#endif

    return ch;
}

static int getche(){
#ifdef _WIN32
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == NULL) return EOF;

    DWORD oldmode;
    GetConsoleMode(input, &oldmode);
    DWORD newmode = oldmode & ~ENABLE_LINE_INPUT;
    SetConsoleMode(input, newmode);
#elif defined(__unix__)
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);

    newattr = oldattr;
    newattr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
#endif

    int ch = getc(stdin);

#ifdef _WIN32
    SetConsoleMode(input, oldmode);
#elif defined(__unix__)
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
#endif

    return ch;
}

static void clrline(){
    printf(ESC"[2K"ESC"E");
}

static void resetcolors(){
//    printf(ESC"001b");
    printf(ESC"[0m");
    bg_color       = 9;
    font_color     = 9;
    font_bold      = 0;
    font_underline = 0;
    font_blink     = 0;
    font_invert    = 0;
}

static void showcursor(){
    printf(ESC"[?25h");
}

static void hidecursor(){
    printf(ESC"[?25l");
}
