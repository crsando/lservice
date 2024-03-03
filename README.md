# lservice

lservice implements a model: a separate thread with a message queue (inbox). the thread wakes up whenever a new message arrives and begins to process the message.


# design

```

local service = require "lservice"

```