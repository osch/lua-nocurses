local nocurses = require("nocurses")

local function printf(...)
    io.write(string.format(...))
    io.flush()
end

local width, height = nocurses.gettermsize()
local redisplay = true
local counter = 0

while true do
    if redisplay then
        nocurses.clrscr();
        counter = counter + 1
        local message = string.format("Hello World! (%d)", counter)
        
        local x = math.floor((width - #message) / 2)
        local y = math.floor(height / 2)
        
        nocurses.gotoxy(x, y)
        nocurses.setfontbold(true)
        nocurses.setunderline(true)
        printf(message)
        
        nocurses.gotoxy(1, height - 1)
        nocurses.resetcolors()
        nocurses.setinvert(true)
        nocurses.clrline()
        printf("Press q to Quit")
        nocurses.resetcolors()
        redisplay = false
    end

    local c = nocurses.getch() -- might be nil if terminal size changes
    c = c and string.char(c)

    local w, h = nocurses.gettermsize()
    if w ~= width or h ~= height then
        width, height = w, h
        redisplay = true
    end

    if c == "Q" or c == "q" then
        break
    end
end


nocurses.gotoxy(1, height - 2)
nocurses.clrline()
printf("Finished.\n")
