local i = 1
local inspect = require "inspect"
local seri = require "lseri"

local ffi = require "ffi"

ffi.cdef[[
    typedef unsigned long int pthread_t;
    pthread_t pthread_self();
    unsigned int sleep (unsigned int seconds);
]]

print("init hello")

function hello(msg)
    i = i + 1
    print(ffi.C.pthread_self(), "from service hello", i, msg)

    local data = seri.unpack_remove(msg)
    print(inspect(data))
end

return hello