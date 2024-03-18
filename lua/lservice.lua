-- use ffi to load the C module
local ffi = require "ffi"

ffi.cdef[[
typedef unsigned long int pthread_t;
pthread_t pthread_self();

typedef struct _dummy1 registry_t;
typedef struct _dummy2 lua_State;

typedef struct {
    registry_t * services;
    registry_t * variables;
} service_pool_t;

service_pool_t * service_pool_new();

void * service_pool_registry(service_pool_t * pool, const char * key, void * ptr);

typedef struct {
    service_pool_t * pool;
    char name[32];

    // init params
    char * code;
    void * config;
    // service_id id;

    // multi-thread utilities
    pthread_t thread;
    struct queue * q;
    struct cond * c;


    lua_State * L;
    int lua_func_ref;
} service_t;

// service_t * service_new(service_pool_t * pool, const char * name);
service_t * service_new(service_pool_t * pool, const char * name, const char * code, void * config);

// int service_init_lua(service_t * s, const char * code, void * config);
int service_init_lua(service_t * s);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);

int service_send(service_t * s, void * msg);
int service_free(service_t * s);
]]


ffi.cdef[[
    unsigned int sleep (unsigned int seconds);
    int queue_length(struct queue *q);
]]

--
-- end of ffi cdecl
--

local lservice = ffi.load("lservice")

-- wrap C module into a lua module

local M = {}
M.ffi = ffi

function M.new_pool(core_ptr)
    local p = {}
    if core_ptr ~= nil then 
        p.core = ffi.cast("service_pool_t *", core_ptr)
    else 
        p.core = lservice.service_pool_new();
    end

    local _mt = {
        new_service = function(self, t)
                local name = t.name or t.path
                assert(name, "please specify name or path")
                if #name > 30 then name = string.sub(name, 1, 30) end
                local s = M.new {
                        pool = p.core, 
                        name = t.name,
                        code = t.code,
                        path = t.path,
                        config = t.config
                    }
                -- s:init { code = t.code, path = t.path, config = t.config }
                s:start()
                return s
            end,
        
        registry = function(self, key, ptr) 
                return lservice.service_pool_registry(self.core, key, ptr)
            end,
    }
    setmetatable(p, { __index = _mt })
    return p
end

function M.new(t)
    if t.pool ~= nil then 
        assert(t.name, "service name not specified")
    end


    local code = t.code
    if not code then 
        assert(t.path)
        code = assert(io.open(t.path):read("*all"), "service code path not found")
    end


    local _mt = {
        start = function (self)
                return tonumber(lservice.service_start(self._s))
            end,
        send = function (self, msg)
                lservice.service_send(self._s, msg)
                return self
            end,
    }
    local s = {}
    s._s = lservice.service_new(t.pool, t.name, code, t.config)
    setmetatable(s, { __index = _mt })
    return s
end

-- function M.load(path, config)
--     local s = M.new()
--     local ret = 0
--     ret = s:init { path = path, config = config }
--     if ret ~= 0 then 
--         print("init service failed : %s", path)
--         return nil
--     end
--     ret = s:start()
--     if ret ~= 0 then 
--         print("start service failed : %s", path)
--         return nil
--     end
--     return s
-- end

function M.thread_id()
    return (ffi.C.pthread_self())
end

function M.sleep(secs)
    ffi.C.sleep(secs)
end

--
-- utility functions
--

-- deserilizer is the function for resolving the configuration
function M.input(deserilizer, ...)
    local srv_ptr, cfg_ptr = ...
    local config = (cfg_ptr ~= nil) and deserilizer(cfg_ptr) or {}
    local name, pool = nil, nil
    if srv_ptr ~= nil then 
        local srv = ffi.cast("service_t *", srv_ptr) 
        name = ffi.string(srv.name)
        local pool_ptr = srv.pool
        pool = (pool_ptr ~= nil ) and M.new_pool(pool_ptr) or nil
    end
    return name, config, pool
end

return M