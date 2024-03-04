local seri = require "lua-seri"
local inspect = require "inspect"
local ffi = require "ffi"

print(seri)

for k,v in pairs(seri) do 
    print(k,v)
end


data1 = {
    1,
    2,
    3,
    n = "1234",
    ptr = ffi.new("void *", nil),
}

print("data1", inspect(data1))

bin = seri.pack(data1)

print(bin)

print("after unpack", inspect(seri.unpack_remove(bin)))