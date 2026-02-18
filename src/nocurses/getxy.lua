local nocurses = require("nocurses")

local write  = io.write

local getkey           = nocurses.getkey
local request_cur_pos  = nocurses.seq.request_cur_pos

local function getxy()
    write(request_cur_pos)
    while true do
        local key, bytes, x, y = getkey()
        if key == "CursorXY" then
            return x, y
        end
    end
end

return getxy
