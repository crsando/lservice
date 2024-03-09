-- use ffi to load the C module
local ffi = require "ffi"

ffi.cdef[[
typedef unsigned long int pthread_t;
pthread_t pthread_self();

typedef unsigned long long service_id;
typedef struct _dummy lua_State;
typedef struct {
    service_id id;
    struct queue * q;

    pthread_t thread;
    struct cond * c;

    lua_State * L;
    int lua_func_ref;
} service_t;

service_t * service_new();

int service_init_lua(service_t * s, const char * code);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);
int service_send(service_t * s, void * msg);
int service_free(service_t * s);
]]


ffi.cdef[[
    unsigned int sleep (unsigned int seconds);
    int queue_length(struct queue *q);
]]


local lservice = ffi.load("lservice")

-- wrap C module into a lua module

local M = {}

local _mt = {
    init = function (self, t)
            assert(t.code or t.path)
            local code 
            if t.path then 
                code = io.open(t.path):read("*all")
            else 
                code = t.code
            end
            return tonumber(lservice.service_init_lua(self._s, code))
        end,
    start = function (self)
            return tonumber(lservice.service_start(self._s))
        end,

    send = function (self, msg)
            lservice.service_send(self._s, msg)
            return self
        end,
}

function M.new()
    local s = {}
    s._s = lservice.service_new()
    setmetatable(s, { __index = _mt })
    return s
end

function M.load(path)
    local s = M.new()
    local ret = 0
    ret = s:init { path = path }
    if ret ~= 0 then 
        print("init service failed : %s", path)
        return nil
    end
    ret = s:start()
    if ret ~= 0 then 
        print("start service failed : %s", path)
        return nil
    end
    return s
end

function M.thread_id()
    return (ffi.C.pthread_self())
end

function M.sleep(secs)
    ffi.C.sleep(secs)
end

return M