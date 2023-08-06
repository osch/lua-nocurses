local byte     = string.byte
local char     = string.char
local nocurses = require("nocurses")
local getch    = nocurses.getch
local peekch   = nocurses.peekch
local skipch   = nocurses.skipch

if not getch then
    error("nocurses.getkey must be invoked from main thread")
end

local ESC = 0x1B
local LBR = byte("[")
local TLD = byte("~")

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

local LF  = 0x0A
local TAB = 0x09
local BS1 = 0x7F
local BS2 = 0x08

local function parseCSI(rslt, c3)
    while true do
        if not c3 then
            return rslt
        end
        rslt = rslt..char(c3)
        if c3 >= 0x40 then
            return rslt
        end
        c3 = getch(0)
    end
end

local function readUtf8Char(c1)
    local rslt = char(c1)
    if c1 >= 0xC0 then -- could be start of utf8 character
        local i = 0
        while true do
            local cn = peekch(i + 1)
            if cn then
                if cn >= 0x80 and cn <= 0xBF then
                    rslt = rslt..char(cn) -- utf8 follow-up byte
                    i = i + 1
                else
                    skipch(i)
                    return rslt, rslt
                end
            else
                skipch(i)
                return rslt, rslt
            end
        end
    else
        return rslt, rslt
    end
end

local function getkey(timeout)
    local c1 = getch(timeout)
    if c1 then
        if c1 == ESC then
            local c2 = peekch()
            if c2 then
                if c2 == LBR then
                    skipch()
                    local c3 = getch(0)
                    if c3 then
                            if c3 == BYTE_A then return "Up", char(c1, c2, c3)
                        elseif c3 == BYTE_B then return "Down", char(c1, c2, c3)
                        elseif c3 == BYTE_C then return "Right", char(c1, c2, c3)
                        elseif c3 == BYTE_D then return "Left", char(c1, c2, c3)
                        elseif c3 == BYTE_F then return "End", char(c1, c2, c3)
                        elseif c3 == BYTE_H then return "Home", char(c1, c2, c3)

                        elseif c3 == BYTE_1
                            or c3 == BYTE_2
                            or c3 == BYTE_3
                            or c3 == BYTE_5
                            or c3 == BYTE_6
                        then
                            local c4 = getch(0)
                            if c4 then
                                if c4 == TLD then
                                        if c3 == BYTE_2 then return "Insert", char(c1, c2, c3, c4)
                                    elseif c3 == BYTE_3 then return "Delete", char(c1, c2, c3, c4)
                                    elseif c3 == BYTE_5 then return "PageUp", char(c1, c2, c3, c4)
                                    elseif c3 == BYTE_6 then return "PageDown", char(c1, c2, c3, c4)
                                    end
                                elseif c4 >= BYTE_0 and c4 <= BYTE_9 then
                                    local c5 = getch(0)
                                    if c5 == TLD then
                                            if c3 == BYTE_1 and c4 == BYTE_1 then return "F1", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_2 then return "F2", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_3 then return "F3", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_4 then return "F4", char(c1, c2, c3, c4, c5)


                                        elseif c3 == BYTE_1 and c4 == BYTE_5 then return "F5", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_7 then return "F6", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_8 then return "F7", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_1 and c4 == BYTE_9 then return "F8", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_2 and c4 == BYTE_0 then return "F9", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_2 and c4 == BYTE_1 then return "F10", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_2 and c4 == BYTE_3 then return "F11", char(c1, c2, c3, c4, c5)
                                        elseif c3 == BYTE_2 and c4 == BYTE_4 then return "F12", char(c1, c2, c3, c4, c5)
                                        end
                                    end
                                    return false, parseCSI(char(c1, c2, c3, c4), c5)
                                end
                                local parsed = parseCSI(char(c1, c2, c3), c4)
                                
                                    if parsed == "\27[1;2A" then return "Shift+Up", parsed
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
                                

                                end
                                return false, parsed
                            end
                        end
                    end
                    return false, char(c1, c2)
                elseif c2 == BYTE_O then
                    skipch()
                    local c3 = getch(0)
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
                local c, parsed = readUtf8Char(c2)
                if c == " " then c = "Space" end
                return "Alt+"..c, char(c1)..parsed
            else
                return "Escape", char(c1)
            end
        elseif c1 == LF  then return "Enter", char(c1)
        elseif c1 == TAB then return "Tab", char(c1)
        elseif c1 == BS1
            or c1 == BS2 then return "Backspace", char(c1)
        else
            return readUtf8Char(c1)
        end
    else
        return nil
    end
end

return getkey
