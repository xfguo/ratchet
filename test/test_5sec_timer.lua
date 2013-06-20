require "ratchet"

function ctx1()
    ratchet.thread.timer(5.0)
end

local r = ratchet.new(function ()
    ratchet.thread.attach(ctx1)
end)

r:loop()

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et:
