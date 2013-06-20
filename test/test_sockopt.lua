require "ratchet"

function ctx1(host, port)
    local rec = ratchet.socket.prepare_tcp(host, port)
    local socket = ratchet.socket.new(rec.family, rec.socktype, rec.protocol)
    socket:setsockopt("SO_REUSEADDR", true)
    socket:bind(rec.addr)
    socket:listen()

    assert(socket:getsockopt("SO_ACCEPTCONN") == true, "SO_ACCEPTCONN != true")

    assert(socket:getsockopt("SO_REUSEADDR") == true, "SO_REUSEADDR != true")
    socket:setsockopt("SO_REUSEADDR", false)
    assert(socket:getsockopt("SO_REUSEADDR") == false, "SO_REUSEADDR != false")
    socket:setsockopt("SO_REUSEADDR", true)
end

function ctx2(host, port)
    local rec = ratchet.socket.prepare_tcp(host, port)
    local socket = ratchet.socket.new(rec.family, rec.socktype, rec.protocol)

    assert(socket:getsockopt("SO_ACCEPTCONN") == false, "SO_ACCEPTCONN != false")

    -- Linux kernel doubles whatever value you set to SO_SND/RCVBUF.
    socket:setsockopt("SO_SNDBUF", 1024)
    assert(socket:getsockopt("SO_SNDBUF") == 2048, "SO_SNDBUF (" .. socket:getsockopt("SO_SNDBUF") .. ") != 2048")
end

kernel = ratchet.new(function ()
    ratchet.thread.attach(ctx1, "127.0.0.1", 10025)
    ratchet.thread.attach(ctx2, "127.0.0.1", 10025)
end)
kernel:loop()

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et:
