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

while true do
    if redisplay then
        nocurses.clrscr();

        local y = math.floor(screenHeight / 2)
        printCentered(y, "Value = %d", value)
        printCentered(y+1, "(use arrow keys to increase/decrease value, press q to Quit)")

        nocurses.gotoxy(1, screenHeight - 1)
        redisplay = false
    end

    local inp = nocurses.getch() -- might be nil if terminal size changes

    local w, h = nocurses.gettermsize()
    if w ~= screenWidth or h ~= screenHeight then
        screenWidth, screenHeight = w, h
        redisplay = true
    end

    local c = inp and string.char(inp)
    
    if c == "Q" or c == "q" then
        break
    elseif inp == 27 then
        c = ""
        while inp do
            inp = nocurses.getch(0)
            if inp then
                c = c..string.char(inp)
            end
        end
        -- https://en.wikipedia.org/wiki/ANSI_escape_code
        if c == "[A" then      -- Arrow Up
            value = value + 10
        elseif c == "[B" then  -- Arrow Down
            value = value - 10
        elseif c == "[C" then  -- Arrow Right
            value = value + 1
        elseif c == "[D" then  -- Arrow Left
            value = value - 1
        else
            print(c)
        end
        redisplay = true
    else
        print(inp)
    end
end

nocurses.gotoxy(1, screenHeight - 2)
nocurses.clrline()
printf("Finished.\n")
