local ffi = require "ffi"

ffi.cdef[[
typedef unsigned long int pthread_t;
pthread_t pthread_self();
]]

ffi.cdef[[
typedef unsigned long long service_id;

typedef void * (*service_routine_t)(void * s, void * msg);

typedef struct _lua_State lua_State;

typedef struct {
    service_id id;
    struct queue * q;
    pthread_t thread;
    struct cond * c;
    uint32_t service_type; 
    lua_State * L;
    int lua_func_ref;
    service_routine_t routine;
} service_t;

service_t * service_new();

int service_init_lua(service_t * s, const char * code);
int service_routine_lua(service_t * s, void * msg);
int service_start(service_t * s);
int service_send(service_t * s, void * msg);

int service_free(service_t * s);

typedef struct {
    uint32_t type;
    void * body;
    size_t size;
} message_t;
]]


ffi.cdef[[
    unsigned int sleep (unsigned int seconds);
    int queue_length(struct queue *q);
]]


local lservice = ffi.load("lservice")

local s = lservice.service_new()
local code = io.open("service/hello.lua"):read("*all")

lservice.service_init_lua(s, code)
lservice.service_start(s)

local msg = ffi.new("message_t")

print(ffi.C.pthread_self())


local seri = require "lseri"
local data = {
    topic = "hello",
    body = "world",
}


while true do
    local n = lservice.queue_length(s.q)
    print("queue_length: ", n)
    local msg = seri.pack(data)
    lservice.service_send(s, msg)
    ffi.C.sleep(1)
end