local byte     = string.byte
local char     = string.char

local nocurses         = require("nocurses")
local getch            = nocurses.getch
local peekch           = nocurses.peekch
local skipch           = nocurses.skipch
local response_cur_pos = nocurses.seq.response_cur_pos

if not getch then
    error("nocurses.getkey must be invoked from main thread")
end

local BYTE_ESC = 0x1B
local BYTE_LBR = byte("[")
local BYTE_TLD = byte("~")

local BYTE_A   = byte("A")
local BYTE_B   = byte("B")
local BYTE_C   = byte("C")
local BYTE_D   = byte("D")
local BYTE_F   = byte("F")
local BYTE_H   = byte("H")
local BYTE_O   = byte("O")
local BYTE_P   = byte("P")
local BYTE_Q   = byte("Q")
local BYTE_R   = byte("R")
local BYTE_S   = byte("S")

local BYTE_0   = byte("0")
local BYTE_1   = byte("1")
local BYTE_2   = byte("2")
local BYTE_3   = byte("3")
local BYTE_4   = byte("4")
local BYTE_5   = byte("5")
local BYTE_6   = byte("6")
local BYTE_7   = byte("7")
local BYTE_8   = byte("8")
local BYTE_9   = byte("9")

local BYTE_LF  = 0x0A
local BYTE_TAB = 0x09
local BYTE_BS1 = 0x7F
local BYTE_BS2 = 0x08
local BYTE_SPC = 0x20

local function parseCSI(rslt, c3, timeout2)
    while true do
        if not c3 then
            return rslt
        end
        rslt = rslt..char(c3)
        if c3 >= 0x40 then
            return rslt
        end
        c3 = getch(timeout2)
    end
end

local function readUtf8Char(c1, timeout2)
    local rslt = char(c1)
    if c1 < BYTE_SPC then
        return "Ctrl+"..char(BYTE_A + c1 - 1), rslt
    elseif c1 >= 0xC0 then -- could be start of utf8 character
        local i = 0
        while true do
            local cn = peekch(i + 1, timeout2)
            if cn then
                if cn >= 0x80 and cn <= 0xBF then
                    rslt = rslt..char(cn) -- utf8 follow-up byte
                    i = i + 1
                else
                    skipch(i)
                    return false, rslt
                end
            else
                skipch(i)
                return false, rslt
            end
        end
    else
        return false, rslt
    end
end

local function getkey(timeout, timeout2)
    timeout2 = timeout2 or 0.050
    local c1 = getch(timeout)
    if c1 then
        if c1 == BYTE_ESC then
            local c2 = peekch(1, timeout2)
            if c2 then
                if c2 == BYTE_LBR then
                    skipch()
                    local c3 = getch(timeout2)
                    if c3 then
                        local parsed = parseCSI(char(c1, c2), c3, timeout2)
                        
                            if parsed == "\27[A" then return "Up",    parsed
                        elseif parsed == "\27[B" then return "Down",  parsed
                        elseif parsed == "\27[C" then return "Right", parsed
                        elseif parsed == "\27[D" then return "Left",  parsed

                        elseif parsed == "\27[F" then return "End",  parsed
                        elseif parsed == "\27[H" then return "Home", parsed
                        
                        elseif parsed == "\27[2~" then return "Insert",   parsed
                        elseif parsed == "\27[3~" then return "Delete",   parsed
                        elseif parsed == "\27[5~" then return "PageUp",   parsed
                        elseif parsed == "\27[6~" then return "PageDown", parsed

                        elseif parsed == "\27[11~" then return "F1", parsed
                        elseif parsed == "\27[12~" then return "F2", parsed
                        elseif parsed == "\27[13~" then return "F3", parsed
                        elseif parsed == "\27[14~" then return "F4", parsed

                        elseif parsed == "\27[15~" then return "F5", parsed
                        elseif parsed == "\27[17~" then return "F6", parsed
                        elseif parsed == "\27[18~" then return "F7", parsed
                        elseif parsed == "\27[19~" then return "F8", parsed

                        elseif parsed == "\27[20~" then return "F9", parsed
                        elseif parsed == "\27[21~" then return "F10", parsed
                        elseif parsed == "\27[23~" then return "F11", parsed
                        elseif parsed == "\27[24~" then return "F12", parsed

                        elseif parsed == "\27[1;2A" then return "Shift+Up", parsed
                        elseif parsed == "\27[1;2B" then return "Shift+Down", parsed
                        elseif parsed == "\27[1;2C" then return "Shift+Right", parsed
                        elseif parsed == "\27[1;2D" then return "Shift+Left", parsed

                        elseif parsed == "\27[1;5A" then return "Ctrl+Up", parsed
                        elseif parsed == "\27[1;5B" then return "Ctrl+Down", parsed
                        elseif parsed == "\27[1;5C" then return "Ctrl+Right", parsed
                        elseif parsed == "\27[1;5D" then return "Ctrl+Left", parsed
                        
                        elseif parsed == "\27[1;3A" then return "Alt+Up", parsed
                        elseif parsed == "\27[1;3B" then return "Alt+Down", parsed
                        elseif parsed == "\27[1;3C" then return "Alt+Right", parsed
                        elseif parsed == "\27[1;3D" then return "Alt+Left", parsed
                        
                        elseif parsed == "\27[2;5~" then return "Ctrl+Insert", parsed
                        elseif parsed == "\27[3;5~" then return "Ctrl+Delete", parsed
                        elseif parsed == "\27[1;5H" then return "Ctrl+Home", parsed
                        elseif parsed == "\27[1;5F" then return "Ctrl+End", parsed
                        elseif parsed == "\27[5;5~" then return "Ctrl+PageUp", parsed
                        elseif parsed == "\27[6;5~" then return "Ctrl+PageDown", parsed
                        
                        elseif parsed == "\27[2;3~" then return "Alt+Insert", parsed
                        elseif parsed == "\27[3;3~" then return "Alt+Delete", parsed
                        elseif parsed == "\27[1;3H" then return "Alt+Home", parsed
                        elseif parsed == "\27[1;3F" then return "Alt+End", parsed
                        elseif parsed == "\27[5;3~" then return "Alt+PageUp", parsed
                        elseif parsed == "\27[6;3~" then return "Alt+PageDown", parsed
                        else
                            local y, x = parsed:match(response_cur_pos)
                            if y then
                                return "CursorXY", parsed, tonumber(x), tonumber(y)
                            else
                                return false, parsed
                            end
                        end
                    end
                    return false, char(c1, c2)
                elseif c2 == BYTE_O then
                    skipch()
                    local c3 = getch(timeout2)
                    if c3 then
                            if c3 == BYTE_P then return "F1", char(c1, c2, c3)
                        elseif c3 == BYTE_Q then return "F2", char(c1, c2, c3)
                        elseif c3 == BYTE_R then return "F3", char(c1, c2, c3)
                        elseif c3 == BYTE_S then return "F4", char(c1, c2, c3)
                        end
                        return false, char(c1, c2, c3)
                    end
                    return false, char(c1, c2)
                end
                skipch()
                local _, parsed = readUtf8Char(c2, timeout2)
                if parsed == " " then
                    return "Alt+Space", char(c1)..parsed
                else
                    return "Alt+"..parsed
                end
            else
                return "Escape", char(c1)
            end
        elseif c1 == BYTE_LF  then return "Enter", char(c1)
        elseif c1 == BYTE_TAB then return "Tab", char(c1)
        elseif c1 == BYTE_BS1
            or c1 == BYTE_BS2 then return "Backspace", char(c1)
        else
            return readUtf8Char(c1, timeout2)
        end
    else
        return nil
    end
end

return getkey
