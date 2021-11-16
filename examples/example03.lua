local nocurses  = require("nocurses")
local llthreads = require("llthreads2.ex")
local mtmsg     = require("mtmsg")


local function printf(...)
    io.write(string.format(...))
    io.flush()
end

local threadIn  = mtmsg.newbuffer()
local threadOut = mtmsg.newbuffer()

threadOut:notifier(nocurses)

local thread = llthreads.new(function(inId, outId)
                                    local mtmsg      = require("mtmsg")
                                    local threadIn   = mtmsg.buffer(inId)
                                    local threadOut  = mtmsg.buffer(outId)
                                    local started    = mtmsg.time()
                                    local lastReport = mtmsg.time()
                                    local paused     = false
                                    local DURATION   = 10 -- work has finished after 10 seconds
                                    local function progress(text)
                                        return string.format("%-8s %3d%%", text, math.floor((mtmsg.time() - started)/DURATION*100))
                                    end
                                    threadOut:addmsg(progress("working"))
                                    threadIn:nonblock(true)
                                    while true do
                                        local msg = threadIn:nextmsg()
                                        if msg == "start" then
                                            if not started or paused then
                                                if paused then
                                                    started = mtmsg.time() - paused
                                                else
                                                    started = mtmsg.time()
                                                end
                                                paused = false
                                                threadOut:addmsg(progress("working"))
                                                lastReport = mtmsg.time()
                                                threadIn:nonblock(true)
                                            end
                                        elseif msg == "pause" then
                                            if started and not paused then
                                                paused = mtmsg.time() - started
                                                threadOut:addmsg(progress("paused"))
                                                threadIn:nonblock(false)
                                            end
                                        elseif msg == "terminate" then
                                            threadOut:addmsg("terminated")
                                            return
                                        end
                                        if started and not paused then
                                            -- do some "work" here ...
                                            if mtmsg.time() - lastReport >= 1 then
                                                threadOut:addmsg(progress("working"))
                                                lastReport = mtmsg.time()
                                            end
                                            if mtmsg.time() - started > DURATION then
                                                started = false
                                                threadOut:addmsg("finished")
                                                threadIn:nonblock(false)
                                            end
                                        end
                                    end
                                end,
                                threadIn:id(), threadOut:id())
thread:start()

nocurses.setfontbold(true)
printf("Press <Q> to Quit, <Space> for thread start/stop...\n")
nocurses.setfontbold(false)

local started = true
while true do
    local c = nocurses.getch()
    c = c and string.char(c)
    local status = threadOut:nextmsg(0)
    if status then
        if status:match("^paused") then
            started = false
            nocurses.setfontbold(true)
            nocurses.setfontcolor("RED")
        elseif status:match("^working") then
            started = true
        elseif status:match("^finished") then
            started = false
            nocurses.setfontbold(true)
            nocurses.setfontcolor("GREEN")
        end
        printf("Thread status: %s\n", status)
        nocurses.resetcolors()
    end
    if c == ' ' then
        if started then
            threadIn:addmsg("pause")
        else
            threadIn:addmsg("start")
        end
    elseif c == 'q' or c == 'Q' then
        threadIn:addmsg("terminate")
        break
    end
end
thread:join()
nocurses.setfontbold(true)
printf("Quit.\n")

