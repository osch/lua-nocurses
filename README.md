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
     `nocurses` lua module as notifier to a [mtmsg] buffer object 
     for receiving messages from the background thread. 
     (This is done by implementing the *Notify C API*, 
      see: [src/notify_capi.h](./src/notify_capi.h))

<!-- ---------------------------------------------------------------------------------------- -->

## Documentation

TODO

See also original documentation at https://github.com/LionyxML/nocurses.

<!-- ---------------------------------------------------------------------------------------- -->

[Lua]:           https://www.lua.org
[llthreads2]:    https://luarocks.org/modules/moteus/lua-llthreads2
[mtmsg]:         https://github.com/osch/lua-mtmsg

<!-- ---------------------------------------------------------------------------------------- -->
