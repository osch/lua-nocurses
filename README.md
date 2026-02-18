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

   * [`example04.lua`](./examples/example04.lua)
   
     Demonstrates how to read keystrokes with multibyte sequences
     (arrow keys).
   
   * [`example05.lua`](./examples/example05.lua)
   
     Demonstrates how to read keystrokes with multibyte sequences
     (arrow keys) using the convenience method [nocurses.getkey()](#nocurses_getkey).
   
<!-- ---------------------------------------------------------------------------------------- -->

## Documentation

See also original documentation at https://github.com/LionyxML/nocurses.

   * [Module Functions](#module-functions)
        * [nocurses.awake()](#nocurses_awake)
        * [nocurses.clrline()](#nocurses_clrline)
        * [nocurses.clrscr()](#nocurses_clrscr)
        * [nocurses.clrtoeol()](#nocurses_clrtoeol)
        * [nocurses.clrtoeos()](#nocurses_clrtoeos)
        * [nocurses.setraw()](#nocurses_setraw)
        * [nocurses.israw()](#nocurses_israw)
        * [nocurses.getch()](#nocurses_getch)
        * [nocurses.peekch()](#nocurses_peekch)
        * [nocurses.skipch()](#nocurses_skipch)
        * [nocurses.getkey()](#nocurses_getkey)
        * [nocurses.getxy()](#nocurses_getxy)
        * [nocurses.gettermsize()](#nocurses_gettermsize)
        * [nocurses.gotoxy()](#nocurses_gotoxy)
        * [nocurses.gotox)](#nocurses_gotox)
        * [nocurses.up()](#nocurses_up)
        * [nocurses.down()](#nocurses_down)
        * [nocurses.left()](#nocurses_left)
        * [nocurses.right()](#nocurses_right)
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
        * [nocurses.hidecursor()](#nocurses_hidecursor)
        * [nocurses.showcursor()](#nocurses_showcursor)
   * [Color Names](#color-names)
   * [Shape Names](#shape-names)
   * [Control Sequences](#control-sequences)

<!-- ---------------------------------------------------------------------------------------- -->
##   Module Functions
<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_awake">**`nocurses.awake()
  `**</span>

  May be called from any thread to interrupt [nocurses.getch()](#nocurses_getch) on the main
  thread. The main thread is the first thread that loads the *nocurses* module.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_clrline">**`nocurses.clrline()
  `**</span>

  Clears the row contents.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_clrscr">**`nocurses.clrscr()
  `**</span>

  Clears the screen.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_clrtoeol">**`nocurses.clrtoeol()
  `**</span>

  Clears from cursor position to the end of the line.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_clrtoeos">**`nocurses.clrtoeos()
  `**</span>

  Clears from cursor position to the end of the screen.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setraw">**`nocurses.setraw([enable])
  `**</span>

  Sets terminal to raw input mode.

  * *enable* - optional boolean, *true* to enable raw mode, *false* to disable raw mode,
               default value is *true*.
  
  Raw input mode is needed to get keystrokes from the operation system.
  If you invoke [nocurses.getch()](#nocurses_getch) without explicitly setting the raw mode 
  via **setraw(true)**, the raw mode is enabled implicitly by the function **getch()**
  and disabled on exit of this function. This is suitable for playing around on the lua 
  commandline interactively but for real world programs it is recommended to enter
  the raw mode at program and keep this mode until program termination.
  
  On normal program termination (i.e. destruction of the lua_State) the raw mode is disabled
  automatically by *nocurses*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_israw">**`nocurses.israw()
  `**</span>
  
  Returns *true* if *setraw(true)* was invoked, otherwise *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_getch">**`nocurses.getch([timeout])
  `**</span>

  Gets a character byte without waiting for enter.
  
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
  
  Otherwise this function returns the obtained character byte as integer value.

<!-- ---------------------------------------------------------------------------------------- -->
* <span id="nocurses_peekch">**`nocurses.peekch([cnt])
  `**</span>
  
  Obtains next character byte from the input queue without consuming the character, i.e.
  a subsequent call to [nocurses.getch()](#nocurses_getch) will deliver this byte again.
  
  * *cnt* - optional integer, number of the byte in the input queue to obtain, first byte
            has number 1. If not given, the default value is 1, i.e. the first byte in the
            input queue is obtained.

  This function does not wait and will return *nil* if the specified input byte is not 
  available. 
  
  The input character byte is returned as integer value.

<!-- ---------------------------------------------------------------------------------------- -->
* <span id="nocurses_skipch">**`nocurses.skipch([cnt])
  `**</span>

  Discards bytes in the input queue.

  * *cnt* - optional integer, number of bytes in the input queue to discard. If not given, 
            the default value is 1, i.e. the first byte in the input queue is skipped, 
            if available..

  This function does not wait and will skip only input bytes in the queue that are immediately 
  available.
  
  This function returns the number of bytes skipped in the input queue.
  
<!-- ---------------------------------------------------------------------------------------- -->
* <span id="nocurses_getkey">**`nocurses.getkey([timeout])
  `**</span>

  Returns the name of the pressed key as string and the input bytes for this key.
  
  This function is implemented in Lua (see [`getkey.lua`](./src/nocurses/getkey.lua)) 
  using the low level functions [getch()](#nocurses_getch), 
  [peekch()](#nocurses_peekch) and [skipch()](#nocurses_skipch) and 
  heuristically determines the pressed key for the current input byte sequence.

  * *timeout* - optional float, timeout in seconds.
  
  The timeout handling is the same as in the function [nocurses.getch()](#nocurses_getch).

  If a special control key is recognized (e.g. arrow keys) , this function returns the 
  key name as string and the consumed raw input bytes as string.  

  If a special key name could not be determined (e.g. normal letter key) from the input 
  byte sequence, this function returns the boolean value *false*  and the consumed 
  raw input bytes as string.
  
  See also: [`example05.lua`](./examples/example05.lua)

<!-- ---------------------------------------------------------------------------------------- -->
* <span id="nocurses_getxy">**`nocurses.getxy()
  `**</span>

  Returns the current cursor position x, y. Where x is the column number and y the row number.
  
  This function is implemented in Lua (see [`getxy.lua`](./src/nocurses/getxy.lua)) using the 
  function [getkey()](#nocurses_getkey).


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_gettermsize">**`nocurses.gettermsize()
  `**</span>

  Gets the columns and rows of the terminal.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_gotoxy">**`nocurses.gotoxy(x, y)
  `**</span>

  Sets the cursor to the position x, y. Where x is the column number and y the row number.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_gotox">**`nocurses.gotox(x)
  `**</span>

  Sets the cursor to the column x.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_up">**`nocurses.up([d])
  `**</span>

  Moves the cursor upward.

  * *d* - optional integer, number of rows the cursor should be moved, 
          default value is 1. May be negative for moving the cursor downward.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_down">**`nocurses.down([d])
  `**</span>

  Moves the cursor downward.

  * *d* - optional integer, number of rows the cursor should be moved, 
          default value is 1. May be negative for moving the cursor upward.


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_left">**`nocurses.left([d])
  `**</span>

  Moves the cursor leftward.

  * *d* - optional integer, number of columns the cursor should be moved, 
          default value is 1. May be negative for moving the cursor rightward.


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_right">**`nocurses.right([d])
  `**</span>

  Moves the cursor rightward.

  * *d* - optional integer, number of columns the cursor should be moved, 
          default value is 1. May be negative for moving the cursor leftward.


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_resetcolors">**`nocurses.resetcolors()
  `**</span>

  Reset terminal to default colors.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setbgrcolor">**`nocurses.setbgrcolor(colorName)
  `**</span>

  Sets the background color to one of the colors described on the color table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setblink">**`nocurses.setblink(status)
  `**</span>

  Sets the blink attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setcurshape">**`nocurses.setcurshape(shapeName)
  `**</span>

  Sets the shape of the cursor in the terminal in the shape table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setfontbold">**`nocurses.setfontbold(status)
  `**</span>

  Sets the bold attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setfontcolor">**`nocurses.setfontcolor(colorName)
  `**</span>

  Sets the text color to one of the colors described on the color table below.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setinvert">**`nocurses.setinvert(status)
  `**</span>

  Sets the invert attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_settitle">**`nocurses.settitle(title)
  `**</span>

  Sets the title of the terminal.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_setunderline">**`nocurses.setunderline(status)
  `**</span>

  Sets the underline attribute on or off. *status* can be *true* or *false*.

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_wait">**`nocurses.wait()
  `**</span>

  Waits for the user to hit [ENTER].


<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_hidecursor">**`nocurses.hidecursor()
  `**</span>

  Hides the cursor. If the program exits, a hidden cursor is automatically made visible again.

  See also: [`example05.lua`](./examples/example05.lua)

<!-- ---------------------------------------------------------------------------------------- -->

* <span id="nocurses_showcursor">**`nocurses.showcursor()
  `**</span>

  Makes the cursor visible. To be called after [nocurses.hidecursor()](#nocurses_hidecursor).


<!-- ---------------------------------------------------------------------------------------- -->
##   Color Names
<!-- ---------------------------------------------------------------------------------------- -->

Valid color names are:
  * DEFAULT
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
  * DEFAULT
  * BLOCK_BLINK
  * BLOCK
  * UNDERLINE_BLINK
  * UNDERLINE
  * BAR_BLINK
  * BAR

<!-- ---------------------------------------------------------------------------------------- -->
##   Control Sequences
<!-- ---------------------------------------------------------------------------------------- -->

The *nocurses.seq* table contains predefined VT100 escape sequences for terminal control. These sequences can be used directly via `io.write()` or other output functions for advanced terminal manipulation.

**Screen Control Sequences:**
   * `clear_screen` - Clear entire screen (`ESC[2J`)
   * `clear_to_eol` - Clear from cursor to end of line (`ESC[0K`)
   * `clear_to_eos` - Clear from cursor to end of screen (`ESC[0J`)
   * `clear_line` - Clear current line (`ESC[2K`)
   * `use_alt_buff` - Switch to alternate screen buffer (`ESC[?1049h`)
   * `use_main_buff` - Switch to main screen buffer (`ESC[?1049l`)

**Cursor Positioning Sequences:**
   * `goto_row_col` - Move cursor to row and column, format: `ESC[%d;%dH` (row, col)
   * `goto_col` - Move cursor to column, format: `ESC[%dG` (column)
   * `go_home` - Move cursor to home position (`ESC[H`)
   * `go_up` - Move cursor up, format: `ESC[%dA` (distance)
   * `go_down` - Move cursor down, format: `ESC[%dB` (distance)
   * `go_left` - Move cursor left, format: `ESC[%dD` (distance)
   * `go_right` - Move cursor right, format: `ESC[%dC` (distance)

**Cursor Shape and Visibility Sequences:**
   * `set_cur_shape` - Set cursor shape, format: `ESC[%d q` (shape code)
   * `set_cur_shape_default` - Set cursor to default shape (`ESC[0 q`)
   * `set_cur_shape_block_blink` - Set cursor to blinking block (`ESC[1 q`)
   * `set_cur_shape_block` - Set cursor to solid block (`ESC[2 q`)
   * `set_cur_shape_underline_blink` - Set cursor to blinking underline (`ESC[3 q`)
   * `set_cur_shape_underline` - Set cursor to solid underline (`ESC[4 q`)
   * `set_cur_shape_bar_blink` - Set cursor to blinking bar (`ESC[5 q`)
   * `set_cur_shape_bar` - Set cursor to solid bar (`ESC[6 q`)
   * `show_cur` - Show cursor (`ESC[?25h`)
   * `hide_cur` - Hide cursor (`ESC[?25l`)

**Text Attributes (Building Blocks):**
   * `attrs_begin` - Start attribute sequence (`ESC[`)
   * `attrs_next` - Separator between attributes (`;`)
   * `attrs_end` - End attribute sequence (`m`)
   * `attr_bold` - Bold attribute (`1`)
   * `attr_dim` - Dim attribute (`2`)
   * `attr_italic` - Italic attribute (`3`)
   * `attr_underline` - Underline attribute (`4`)
   * `attr_blink` - Blink attribute (`5`)
   * `attr_inverse` - Inverse/reverse video attribute (`7`)
   * `attr_reset` - Reset all attributes (`0`)

**Foreground Color Codes (for building attributes):**
   * `attr_foregrd_black` - Black (`30`)
   * `attr_foregrd_red` - Red (`31`)
   * `attr_foregrd_green` - Green (`32`)
   * `attr_foregrd_yellow` - Yellow (`33`)
   * `attr_foregrd_blue` - Blue (`34`)
   * `attr_foregrd_magenta` - Magenta (`35`)
   * `attr_foregrd_cyan` - Cyan (`36`)
   * `attr_foregrd_white` - White (`37`)
   * `attr_foregrd_default` - Default foreground color (`39`)

**Background Color Codes (for building attributes):**
   * `attr_backgrd_black` - Black (`40`)
   * `attr_backgrd_red` - Red (`41`)
   * `attr_backgrd_green` - Green (`42`)
   * `attr_backgrd_yellow` - Yellow (`43`)
   * `attr_backgrd_blue` - Blue (`44`)
   * `attr_backgrd_magenta` - Magenta (`45`)
   * `attr_backgrd_cyan` - Cyan (`46`)
   * `attr_backgrd_white` - White (`47`)
   * `attr_backgrd_default` - Default background color (`49`)

**Convenience Sequences (pre-formatted):**
   * `reset_attrs` - Reset all attributes (`ESC[0m`)
   * `set_foregrd_color` - Set foreground color, format: `ESC[3%dm` (color code 0-7)
   * `set_backgrd_color` - Set background color, format: `ESC[4%dm` (color code 0-7)
   * `set_attr_bold` - Set bold (`ESC[1m`)
   * `set_attr_underline` - Set underline (`ESC[4m`)
   * `set_attr_blink` - Set blink (`ESC[5m`)
   * `set_attr_inverse` - Set inverse video (`ESC[7m`)
   * `set_title` - Set terminal window title, format: `ESC]0;%s\x7` (title string)

**Cursor Position Query:**
   * `request_cur_pos` - Request current cursor position (`ESC[6n`)
   * `response_cur_pos` - Parse cursor position response, pattern: `ESC[(%d+);(%d+)R` (row; col)

**Example Usage:**
```lua
local nocurses = require('nocurses')
io.write(nocurses.seq.clear_screen)      -- Clear screen
io.write(nocurses.seq.set_attr_bold)     -- Make text bold
io.write("Bold text")
io.write(nocurses.seq.reset_attrs)       -- Reset to normal
```

<!-- ---------------------------------------------------------------------------------------- -->

[Lua]:           https://www.lua.org
[llthreads2]:    https://luarocks.org/modules/moteus/lua-llthreads2
[mtmsg]:         https://github.com/osch/lua-mtmsg
[Notify C API]: https://github.com/lua-capis/lua-notify-capi


<!-- ---------------------------------------------------------------------------------------- -->
