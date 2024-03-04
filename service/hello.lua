local uv = require "luv"
local i = 1

print("init hello")

function hello(msg)
    i = i + 1
    print("from service hello", i, msg)
    uv.sleep(3000)
end

return hello