# lua-nocurses

[![Install](https://img.shields.io/badge/Install-LuaRocks-brightgreen.svg)](https://luarocks.org/modules/osch/nocurses)

<!-- ---------------------------------------------------------------------------------------- -->

Simple terminal manipulation capability for the [Lua] scripting language
by the use of VT100 ESC sequences. 

This package is based on [*nocurses* by Rahul M. Juliato](https://github.com/LionyxML/nocurses).
It is aimed to simple applications where ncurses is simple "too much".    

Supported platforms: Linux, MacOS and Cygwin.


<!-- ---------------------------------------------------------------------------------------- -->

## Examples

   * [`example01.lua`](./examples/example01.lua)

     Simple *Hello World* program.

   
   * [`example02.lua`](./examples/example02.lua)

     *Hello World* program, that reacts on terminal size changes while
     waiting for keyboard input.

   
   * [`example03.lua`](./examples/example03.lua)
   
     Reacts on messages from a background thread while also
     evaluating keyboard input. 
     
     Uses [llthreads2] for thread creation and connects the 
     *nocurses* lua module as notifier to a [mtmsg] buffer object 
     for receiving messages from the background thread. 
     (This is done by implementing the [Notify C API], 
      see: [src/notify_capi.h](./src/notify_capi.h))

<!-- ---------------------------------------------------------------------------------------- -->

## Documentation

See also original documentation at https://github.com/LionyxML/nocurses.

   * [Module Functions](#module-functions)
        * [nocurses.awake()](#nocurses_awake)
        * [nocurses.clrline()](#nocurses_clrline)
        * [nocurses.clrscr()](#nocurses_clrscr)
        * [nocurses.getch()](#nocurses_getch)
        * [nocurses.gettermsize()](#nocurses_gettermsize)
        * [nocurses.gotoxy()](#nocurses_gotoxy)
        * [nocurses.resetcolors()](#nocurses_resetcolors)
        * [nocurses.setbgrcolor()](#nocurses_setbgrcolor)
        * [nocurses.setblink()](#nocurses_setblink)
        * [nocurses.setcurshape()](#nocurses_setcurshape)
        * [nocurses.setfontbold()](#nocurses_setfontbold)
        * [nocurses.setfontcolor()](#nocurses_setfontcolor)
        * [nocurses.setinvert()](#nocurses_setinvert)
        * [nocurses.settitle()](#nocurses_settitle)
        * [nocurses.setunderline()](#nocurses_setunderline)
        * [nocurses.wait()](#nocurses_wait)
   * [Color Names](#color-names)
   * [Shape Names](#shape-names)

<!-- ---------------------------------------------------------------------------------------- -->
##   Module Functions
<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_awake">**`  nocurses.awake()
  `**</a>

  May be called from any thread to interrupt [nocurses.getch()](#nocurses_getch) on the main
  thread. The main thread is the first thread that loads the *nocurses* module.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_clrline">**`  nocurses.clrline()
  `**</a>

  Clears the row contents.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_clrscr">**`  nocurses.clrscr()
  `**</a>

  Clears the screen.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_getch">**`  nocurses.getch(timeout)
  `**</a>

  Gets a character without waiting for enter.
  
  * *timeout* - optional float, timeout in seconds.
  
  If timeout is given and not *nil*, this function returns *nil* if no input is available after
  *timeout* seconds.
  
  If timeout is not given or *nil* this function waits without timeout until input becomes
  available.
  
  This function returns *nil* if:
  
     * [nocurses.awake()](#nocurses_awake) is called from any
       other thread.
  
     * the *nocurses* module is notified from native C code running
       in any other thread. This is done by implementing the [Notify C API], 
       see: [src/notify_capi.h](./src/notify_capi.h).
       
     * the terminal size changes.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_gettermsize">**`  nocurses.gettermsize()
  `**</a>

  Gets the columns and rows of the terminal.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_gotoxy">**`  nocurses.gotoxy(x, y)
  `**</a>

  Sets the cursor do the position x, y. Where x is the row number and y the line number.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_resetcolors">**`  nocurses.resetcolors()
  `**</a>

  Reset terminal to default colors.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setbgrcolor">**`  nocurses.setbgrcolor(colorName)
  `**</a>

  Sets the background color to one of the colors described on the color table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setblink">**`  nocurses.setblink(status)
  `**</a>

  Sets the blink attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setcurshape">**`  nocurses.setcurshape(shapeName)
  `**</a>

  Sets the shape of the cursor in the terminal in the shape table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setfontbold">**`  nocurses.setfontbold(status)
  `**</a>

  Sets the bold attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setfontcolor">**`  nocurses.setfontcolor(colorName)
  `**</a>

  Sets the text color to one of the colors described on the color table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setinvert">**`  nocurses.setinvert(status)
  `**</a>

  Sets the invert attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_settitle">**`  nocurses.settitle(title)
  `**</a>

  Sets the title of the terminal.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_setunderline">**`  nocurses.setunderline(status)
  `**</a>

  Sets the underline attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <a id="nocurses_wait">**`  nocurses.wait()
  `**</a>

  Waits for the user to hit [ENTER].


<!-- ---------------------------------------------------------------------------------------- -->
##   Color Names
<!-- ---------------------------------------------------------------------------------------- -->

Valid color names are:
  * BLACK
  * RED
  * GREEN
  * YELLOW
  * BLUE
  * MAGENTA
  * CYAN
  * WHITE  

<!-- ---------------------------------------------------------------------------------------- -->
##   Shape Names
<!-- ---------------------------------------------------------------------------------------- -->

Valid shape names are:
  * BLOCK_BLINK
  * BLOCK
  * UNDERLINE_BLINK
  * UNDERLINE
  * BAR_BLINK
  * BAR

<!-- ---------------------------------------------------------------------------------------- -->

[Lua]:           https://www.lua.org
[llthreads2]:    https://luarocks.org/modules/moteus/lua-llthreads2
[mtmsg]:         https://github.com/osch/lua-mtmsg
[Notify C API]: https://github.com/lua-capis/lua-notify-capi


<!-- ---------------------------------------------------------------------------------------- -->
