local service = require "lservice"
local seri = require "lseri"


local s1 = service.load("service/hello.lua")
local s2 = service.load("service/hello.lua")


local data = { 1, 2, 3, "hello", last = "world"}

while true do 
    s1:send(seri.pack(data))
    service.sleep(1)
    s2:send(seri.pack(data))
    service.sleep(2)
end