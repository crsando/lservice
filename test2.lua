local service = require "lservice"
local seri = require "lseri"


local pool = service.new_pool()

local s1 = pool:load()

local config1 = seri.pack { sentence = "hello world" }

local s1 = pool:load("service/hello.lua", config1)
local s2 = pool:load("service/hello.lua", nil)


local common_config = seri.pack { ping = "hello", pong = "world" }

pool:registry("common", common_config)

local cfg = pool:registry "common"


local data = { 1, 2, 3, "hello", last = "world"}

while true do 
    s1:send(seri.pack(data))
    service.sleep(1)
    s2:send(seri.pack(data))
    service.sleep(2)
end