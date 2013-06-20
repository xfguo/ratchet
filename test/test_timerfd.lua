require "ratchet"

function ctx1 ()
    local i = 0
    local tfd = ratchet.timerfd.new()
    tfd:settime(1.0, 1.0)

    while i < 3 do
        local fired = tfd:read()
        print("fire!")
        i = i + 1
    end
end

local r = ratchet.new(function ()
    ratchet.thread.attach(ctx1)
end)
r:loop()

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et:
