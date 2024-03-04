local ffi = require "ffi"

ffi.cdef[[
typedef unsigned long int pthread_t;
pthread_t pthread_self();

typedef unsigned long long service_id;

typedef void * (*service_routine_t)(void * s, void * msg);

typedef struct {
    service_id id;
    struct queue * q;


    pthread_t thread;
    service_routine_t init;
    service_routine_t routine;
    void * init_params;

    struct cond * c;
} service_t;

service_t * service_new();
int service_start(service_t * s, service_routine_t routine, service_routine_t init, void * init_params);
int service_send(service_t * s, void * msg);

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


function fib(n)
    if n <= 1 then return n end
    return fib(n-1) + fib(n-2)
end

local lservice = ffi.load("lservice")


local s = lservice.service_new()

function routine(s, msg)
    local n = 40
    -- s = ffi.cast("service_t *", s)
    -- print("routine job", lservice.queue_length(s.q))
    print("routine job", ffi.C.pthread_self())
    print(n, fib(n))
    print("sleep end")
end

function init(s, msg)
    s = ffi.cast("service_t *", s)
    print("init job")
end


lservice.service_start(s, routine, init, nil)

local msg = ffi.new("message_t")

print(ffi.C.pthread_self())

while true do
    local n = lservice.queue_length(s.q)
    print("queue_length: ", n)
    lservice.service_send(s, msg)
    print(n, fib(n))
end