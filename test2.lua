local service = require "lservice"
local seri = require "lseri"
local ffi = require "ffi"
service.serializer = seri

local pool = service.new_pool()

-- preinit, add common configurations
local common = ffi.new("int [6]", {1,2,3,4,5,6})
print(common)
pool:registry("common", common)

local config1 = { sentence = "hello world" }
local config2 = { sentence = "hello world" }

local s1 = pool:new_service { 
    name = "s1",
    path = "service/hello.lua", 
    config = config1,
}

local s2 = pool:new_service {
    name = "s2",
    path ="service/hello.lua",
    config = config2,
}


local cfg = ffi.cast("int *", pool:registry "common")
print(cfg, cfg[0], cfg[1], cfg[2], cfg[3], cfg[4], cfg[5])

local data = { 1, 2, 3, "hello", last = "world"}

while true do 
    local stmp = pool:query_service "s1"
    if stmp then 
        stmp:send(seri.pack(data))
    end
    service.usleep(10000)


    -- s1:send(seri.pack(data))
    -- service.sleep(1)
    -- s2:send(seri.pack(data))
    -- service.sleep(2)
end