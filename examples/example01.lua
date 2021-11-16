local nocurses = require("nocurses")

local function printf(...)
    io.write(string.format(...))
    io.flush()
end

nocurses.clrscr();

local w, h = nocurses.gettermsize()

local message = "Hello World!"

local x = math.floor((w-#message)/2)
local y = math.floor(h/2)

nocurses.gotoxy(x, y)
nocurses.setfontbold(true)
nocurses.setunderline(true)
printf(message)

nocurses.gotoxy(1, h-1)
nocurses.resetcolors()
nocurses.setinvert(true)
nocurses.clrline()
printf("Press q to Quit")
nocurses.resetcolors()

while true do
    local c = nocurses.getch() -- may be nil
    c = c and string.char(c)
    if c == "Q" or c == "q" then
        break
    end
end

local w, h = nocurses.gettermsize()
nocurses.gotoxy(1, h-2)
nocurses.clrline()
printf("Finished.\n")
