require "ratchet"

function ctx1(host, port)
    local rec = ratchet.socket.prepare_tcp(host, port, "AF_INET")
    local socket = ratchet.socket.new(rec.family, rec.socktype, rec.protocol)
    socket:setsockopt("SO_REUSEADDR", true)
    socket:bind(rec.addr)
    socket:listen()

    connect_client("127.0.0.1", port)

    local client, from = socket:accept()

    local ptr = ratchet.dns.query(tostring(from), "ptr")
    assert(ptr and ptr[1] == "localhost.")
end

function connect_client(host, port)
    local rec = ratchet.socket.prepare_tcp(host, port)
    local socket = ratchet.socket.new(rec.family, rec.socktype, rec.protocol)
    socket:connect(rec.addr)
end

kernel = ratchet.new(function ()
    ratchet.thread.attach(ctx1, "*", 10025)
end)
kernel:loop()

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et:
