local inspect = require "inspect"
local seri = require "lseri"
local service = require "lservice"
local ffi = service.ffi

local name, config, pool = service.input(seri.unpack_remove, ...)

local common = ffi.cast("int *", pool:registry "common")

print("init", name, inspect(config))
print(common[0], common[5])

local i = 1

function hello(msg)
    i = i + 1
    print(ffi.C.pthread_self(), "from service hello", i, msg)
    print("inside", service.thread_id(), "counter", i)

    local data = seri.unpack_remove(msg)
    print(inspect(data))
end

return hello