local i = 1

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
end

return hello