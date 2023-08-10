local nocurses = require("nocurses")

local function printf(...)
    io.write(string.format(...))
    io.flush()
end

local screenWidth, screenHeight = nocurses.gettermsize()
local redisplay = true
local value = 0

local function printCentered(y, ...)
    local message = string.format(...)
    local x = math.floor((screenWidth - #message) / 2)
    if x < 1 then x = 1 end
    nocurses.gotoxy(x, y)
    printf(message)
end

nocurses.hidecursor()

while true do
    if redisplay then
        nocurses.clrscr();

        local y = math.floor(screenHeight / 2)
        printCentered(y, "Value = %d", value)
        printCentered(y+1, "(use arrow keys to increase/decrease value, press q to Quit)")

        nocurses.gotoxy(1, screenHeight - 1)
        redisplay = false
    end

    local key = nocurses.getkey() -- might be nil if terminal size changes

    local w, h = nocurses.gettermsize()
    if w ~= screenWidth or h ~= screenHeight then
        screenWidth, screenHeight = w, h
        redisplay = true
    end
    if key then
        if key == "Q" or key == "q" then
            break                                                                                         
    
        elseif key == "Up" then
            value = value + 10
      
        elseif key == "Down" then
            value = value - 10
      
        elseif key == "Right" then
            value = value + 1
      
        elseif key == "Left" then
            value = value - 1
        else
            print(inp)
        end
        redisplay = true
    end
end

nocurses.showcursor()
nocurses.gotoxy(1, screenHeight - 2)
nocurses.clrline()
printf("Finished.\n")
