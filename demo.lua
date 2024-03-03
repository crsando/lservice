local service = require "lservice"

local s = service:start(function (s)
    print("Hello World", msg)
end)


s:send(msg)
s:send(msg)
s:send(msg)