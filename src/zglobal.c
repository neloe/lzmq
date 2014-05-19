#include "zglobal.h"
#include "zmq.h"
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

typedef void(*luazmq_library_finalizer_t)(void*);

static luazmq_library_finalizer_t luazmq_library_finalizer     = NULL;
static void*                      luazmq_library_finalizer_ctx = NULL;

static void luazmq_set_finalizer(luazmq_library_finalizer_t fn, void *ctx){
  luazmq_library_finalizer     = fn;
  luazmq_library_finalizer_ctx = ctx;
}

static void luazmq_call_finalizer(void){
  if(luazmq_library_finalizer){
    luazmq_library_finalizer(luazmq_library_finalizer_ctx);
  }
}

#if defined(_WIN32)

#include <windows.h>

static CRITICAL_SECTION luazmq_library_mutex;

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
  switch(fdwReason){
  case DLL_PROCESS_ATTACH:
    InitializeCriticalSection(&luazmq_library_mutex);
    break;
  case DLL_PROCESS_DETACH:
    DeleteCriticalSection(&luazmq_library_mutex);
    luazmq_call_finalizer();
    break;
  }
  return TRUE;
}

static void luazmq_library_lock(){
  EnterCriticalSection(&luazmq_library_mutex);
}

static void luazmq_library_unlock(){
  LeaveCriticalSection(&luazmq_library_mutex);
}

#else

#include <pthread.h>

static pthread_mutex_t luazmq_library_mutex = PTHREAD_MUTEX_INITIALIZER;

static void luazmq_library_lock(){
  pthread_mutex_lock(&luazmq_library_mutex);
}

static void luazmq_library_unlock(){
  pthread_mutex_unlock(&luazmq_library_mutex);
}

// @todo implement calling `luq_global_on_unload` when library unload

#ifdef __GNUC__

static void __attribute__ ((destructor)) luazmq_on_unload(void){
  luazmq_call_finalizer();
}

#endif

#endif

static void* luazmq_library_context = NULL;

static void luazmq_global_context_destroy(void *ctx){
  if(!luazmq_library_context) return;
  assert(luazmq_library_context == ctx);
  zmq_ctx_destroy(ctx);
  luazmq_library_context = NULL;
}

void* luazmq_global_context(){
  if(!luazmq_library_context){
    luazmq_library_lock();
    if(!luazmq_library_context){
      luazmq_library_context = zmq_ctx_new();
      if(luazmq_library_context){
        luazmq_set_finalizer(luazmq_global_context_destroy, luazmq_library_context);
      }
    }
    luazmq_library_unlock();
  }
  return luazmq_library_context;
}
