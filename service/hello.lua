local i = 1
local inspect = require "inspect"
local seri = require "lseri"
local service = require "lservice"

local config = seri.unpack_remove(...)

print("config:", inspect(config))


function hello(msg)
    i = i + 1
    -- print(ffi.C.pthread_self(), "from service hello", i, msg)
    print("inside", service.thread_id(), "counter", i)

    local data = seri.unpack_remove(msg)
    print(inspect(data))
end

return hello