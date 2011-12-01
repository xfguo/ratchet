
require "ratchet"

module("ratchet.bus.server_transaction", package.seeall)
local class = getfenv()
__index = class

-- {{{ new()
function new(request, response_to_bus, socket_buffer, from)
    local self = {}
    setmetatable(self, class)

    self.request = request
    self.socket_buffer = socket_buffer
    self.response_to_bus = response_to_bus
    self.from = from

    return self
end
-- }}}

-- {{{ send_part()
local function send_part(pad, part)
    local part_size = ratchet.socket.hton(#part)

    pad:send(part_size, true)
    pad:send(part)
end
-- }}}

-- {{{ send_response()
function send_response(self, response)
    local part_1, attachments = self.response_to_bus(response)

    local num_parts = 1 + (attachments and #attachments or 0)
    self.socket_buffer:send(ratchet.socket.hton16(num_parts), true)

    send_part(self.socket_buffer, part_1)
    if attachments then
        for i = 1, #attachments do
            send_part(self.socket_buffer, attachments[i])
        end
    end

    self.socket_buffer:close()
end
-- }}}

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et:
