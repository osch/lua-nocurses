package = "nocurses"
version = "0.0.1-1"
local versionNumber = version:gsub("^(.*)-.-$", "%1")
source = {
  url = "https://github.com/osch/lua-nocurses/archive/v"..versionNumber..".zip",
  dir = "lua-nocurses-"..versionNumber,
}
description = {
  summary = "A terminal screen manipulation library",
  homepage = "https://github.com/osch/lua-nocurses",
  license = "GPL2",
  detailed = [[
    This package provides simple terminal manipulation capability 
    by the use of VT100 ESC sequences.
    It is aimed to simple applications where ncurses is simple "too much".    
    Based on nocurses.h by Rahul M. Juliato (see https://github.com/LionyxML/nocurses)
  ]],
}
dependencies = {
  "lua >= 5.1, <= 5.4",
}
build = {
  type = "builtin",
  modules = {
    nocurses = {
      sources = { 
          "src/main.c",
          "src/nocurses_compat.c",
      },
      defines = { "NOCURSES_VERSION="..versionNumber },
    },
  }
}