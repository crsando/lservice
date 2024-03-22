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

service_pool_t * service_pool_new();
void * service_pool_registry(service_pool_t * pool, const char * key, void * ptr);
service_t * service_pool_query_service(service_pool_t * pool, const char * key);

service_t * service_new(service_pool_t * pool, const char * name, const char * code, void * config);

int service_init_lua(service_t * s);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);

int service_send(service_t * s, void * msg);
int service_free(service_t * s);
]]


ffi.cdef[[
    void util_usleep(unsigned int u);
    int queue_length(struct queue *q);
]]

--
-- end of ffi cdecl
--

local lservice = ffi.load("lservice")

-- wrap C module into a lua module

local M = {}
M.ffi = ffi

-- placeholder, dummy
M.serializer = { pack = nil, unpack_remove = nil }


function M.usleep(usecs)
    lservice.util_usleep(usecs)
end

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

                config = nil
                if t.config and (type(t.config) == "table") then 
                    assert(M.serializer.pack, "serializer not specified")
                    config = M.serializer.pack(t.config)
                else 
                    config = t.config
                end

                local s = M.new {
                        pool = p.core, 
                        name = t.name,
                        code = t.code,
                        path = t.path,
                        config = t.config
                    }
                s:start()
                return s
            end,
        
        registry = function(self, key, ptr) 
                return lservice.service_pool_registry(self.core, key, ptr)
            end,

        query_service = function(self, name)
                local ptr = lservice.service_pool_query_service(self.core, name)
                if ptr ~= nil then 
                    return M.from_ptr(ptr)
                end
                return nil
            end,
        send = function (self, name, msg)
                local ptr = lservice.service_pool_query_service(self.core, name)

                if type(msg) == "table" then 
                    assert(M.serializer.pack, "serializer not specified")
                    msg = M.serializer.pack(msg)
                end

                if ptr ~= nil then
                    lservice.service_send(ptr, msg)
                end
                return self
            end,
    }
    setmetatable(p, { __index = _mt })
    return p
end

local _service_mt = {
        start = function (self)
                return tonumber(lservice.service_start(self._s))
            end,
        send = function (self, msg)
                if type(msg) == "table" then 
                    assert(M.serializer.pack, "serializer not specified")
                    msg = M.serializer.pack(msg)
                end
                lservice.service_send(self._s, msg)
                return self
            end,
    }

function M.from_ptr(ptr)
    local s = {}
    s._s = ffi.cast("service_t *", ptr)
    setmetatable(s, { __index = _service_mt })
    return s
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

    config = nil
    if t.config and (type(t.config) == "table") then 
        assert(M.serializer.pack, "serializer not specified")
        config = M.serializer.pack(t.config)
    else 
        config = t.config
    end

    local s = {}
    s._s = lservice.service_new(t.pool, t.name, code, config)
    setmetatable(s, { __index = _service_mt })
    return s
end

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
        pool = (pool_ptr ~= nil ) and M.new_pool(pool_ptr) 
    end
    config = config or {}
    pool = pool or M.new_pool()
    return name, config, pool
end

return M