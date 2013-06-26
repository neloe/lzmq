package = "lzmq-ffi"
version = "scm-0"

source = {
  url = "https://github.com/moteus/lzmq/archive/master.zip",
  dir = "lzmq-master",
}

description = {
  summary = "Lua bindings to ZeroMQ 3",
  homepage = "https://github.com/moteus/lzmq",
  license = "MIT/X11",
}

dependencies = {
  "lua >= 5.1",
  -- "lua-llthreads >= 1.2"
}

build = {
  copy_directories = {"test", "examples"},

  type = "builtin",

  platforms = {
    windows = { modules = {
      ["lzmq.timer"] = {
        defines = {'USE_PERF_COUNT'}
      }
    }},
    unix    = { modules = {
      ["lzmq.timer"] = {
        defines = {'USE_CLOCK_MONOTONIC', 'USE_GETTIMEOFDAY'},
        libraries = {"rt"},
      }
    }}
  },

  modules = {
    ["lzmq.timer"] = {
      sources = {'src/ztimer.c','src/lzutils.c'},
    },
    ["lzmq"              ] = "lua/lzmq/ffi/lzmq.lua";
    ["lzmq.poller"       ] = "lua/lzmq/ffi/poller.lua";
    ["lzmq.threads"      ] = "lua/lzmq/ffi/threads.lua";
    ["lzmq.ffi"          ] = "lua/lzmq/ffi.lua";
    ["lzmq.ffi.api"      ] = "lua/lzmq/ffi/api.lua";
    ["lzmq.ffi.error"    ] = "lua/lzmq/ffi/error.lua";
    ["lzmq.ffi.loop"     ] = "lua/lzmq/ffi/loop.lua";
    ["lzmq.ffi.poller"   ] = "lua/lzmq/ffi/poller.lua";
    ["lzmq.ffi.timer"    ] = "lua/lzmq/ffi/timer.lua";
    ["lzmq.ffi.threads"  ] = "lua/lzmq/ffi/threads.lua";
    ["lzmq.llthreads.ex" ] = "lua/lzmq/llthreads/ex.lua";
    ["lzmq.impl.threads" ] = "lua/lzmq/impl/threads.lua";
    ["lzmq.impl.loop"    ] = "lua/lzmq/impl/loop.lua";
  },
}
