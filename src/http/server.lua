
require "package"
local common = require "ratchet.http.common"

module("ratchet.http.server", package.seeall)
local class = getfenv()
__index = class

-- {{{ new()
function new(socket, from, handlers, send_size)
    local self = {}
    setmetatable(self, class)

    self.socket = socket
    self.from = from
    self.handlers = handlers

    -- socket(7) option SO_SNDBUF returns double the desired value.
    self.send_size = send_size or (socket.SO_SNDBUF / 2)

    return self
end
-- }}}

-- {{{ build_response_and_headers()
function build_response_and_headers(self, response)
    local ret = "HTTP/1.0 " .. response.code .. " " .. response.message .. "\r\n"
    if response.headers and #response.headers then
        ret = ret .. common.build_header_string(response.headers)
    end
    ret = ret .. "\r\n"
    return ret
end
-- }}}

-- {{{ slow_send()
function slow_send(self, socket, request, data)
    -- The purpose of this function is to avoid concatenating with data.

    while #request > self.send_size do
        local to_send = request:sub(1, self.send_size)
        socket:send(to_send)
        request = request:sub(self.send_size+1)
    end
    if not data then
        socket:send(request)
    else
        local to_send = request .. data:sub(1, self.send_size - #request)
        socket:send(to_send)
        data = data:sub(self.send_size - #request + 1)
        while data ~= "" do
            to_send = data:sub(1, self.send_size)
            socket:send(to_send)
            data = data:sub(self.send_size+1)
        end
    end
end
-- }}}

-- {{{ send_response()
function send_response(self, response)
    local response_str = self:build_response_and_headers(response)
    self:slow_send(self.socket, response_str, response.data)
    self.socket:shutdown("both")
    self.socket:close()
end
-- }}}

-- {{{ parse_request_so_far()
function parse_request_so_far(self, so_far, unparsed_i, request)
    local i

    if not request.command or not request.uri then
        local cmd_pattern = "^(.-)%s+(.-)%s+[hH][tT][tT][pP]%/([%d%.]+)\r\n()"
        request.command, request.uri, request.http_ver, i = so_far:match(cmd_pattern, unparsed_i)
        if i then
            unparsed_i = i
        else
            if so_far:match("^.-\r\n", unparsed_i) then
                self.socket:close()
                error("Malformed HTTP session.")
            end
            return false, unparsed_i
        end
    end

    if not request.headers then
        local hdr_pattern = "^(.-\r\n)\r\n()"
        local hdrs, i = so_far:match(hdr_pattern, unparsed_i)
        if i then
            request.headers = common.parse_header_string(hdrs)
            unparsed_i = i
        else
            hdr_pattern = "^\r\n()"
            i = so_far:match(hdr_pattern, unparsed_i)
            if i then
                request.headers = {}
                unparsed_i = i
            else
                return false, unparsed_i
            end
        end
    end

    if not request.data then
        if not request.headers['content-length'] then
            return true
        end
        local content_len = tonumber(request.headers['content-length'][1])
        if not content_len then
            return true
        end

        local recved_len = #so_far - unparsed_i + 1
        if recved_len >= content_len then
            request.data = so_far:sub(unparsed_i, unparsed_i+content_len-1)
            return true
        else
            return false, unparsed_i
        end
    end
end
-- }}}

-- {{{ get_request()
function get_request(self)
    local request = {}
    local so_far = ""
    local unparsed_i = 1
    local done = false

    while not done do
        local data = self.socket:recv()
        so_far = so_far .. data
        done, unparsed_i = self:parse_request_so_far(so_far, unparsed_i, request)
        if data == "" then
            break
        end
    end
    self.socket:shutdown("read")

    --return request.command, request.uri, request.headers, request.data
    return request
end
-- }}}

-- {{{ handle()
function handle(self)
    local req = self:get_request()

    local cmd_handler
    if req.command then
        cmd_handler = self.handlers[req.command:upper()]
    end
    local response = {code = 501, message = "Not Implemented"}
    if req.http_ver ~= "1.0" then
        response = {code = 505, message = "Version Not Supported"}
    end
    if cmd_handler then
        response = cmd_handler(self.handlers, req.uri, req.headers, req.data, self.from)
    end

    self:send_response(response)
end
-- }}}

-- vim:foldmethod=marker:sw=4:ts=4:sts=4:et: