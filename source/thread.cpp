#include <stdint.h>
//#include <retro_miscellaneous.h>
#include "thread.h"

#ifdef THREADED_RENDERER

#if VITA

static int _thread_func(SceSize args, void* p) {
	void** argp = static_cast<void**>(p);
	threadfunc_t func = reinterpret_cast<threadfunc_t>(argp[0]);
	(*func)(argp[1]);
	return sceKernelExitDeleteThread(0);
}

static int _thread_map_priority(int priority) {
	switch (priority) {
		case THREAD_PRIORITY_LOWEST:
			return 0x10000102;
		case THREAD_PRIORITY_LOW:
			return 0x10000101;
		case THREAD_PRIORITY_NORMAL:
		case THREAD_PRIORITY_HIGH:
		case THREAD_PRIORITY_HIGHEST:
		default:
			return 0x10000100;
	}
}

thread_t thread_run(threadfunc_t func, void* p, int priority) {
	void* argp[2];
	argp[0] = reinterpret_cast<void*>(func);
	argp[1] = p;

	SceUID thid =
	    sceKernelCreateThread("my_thread", (SceKernelThreadEntry)_thread_func, _thread_map_priority(priority), 0x10000, 0, 0, NULL);
	if (thid >= 0) sceKernelStartThread(thid, sizeof(argp), &argp);

	return thid;
}

thread_t thread_get() { return sceKernelGetThreadId(); }
void thread_sleep(int ms) { sceKernelDelayThread(ms * 1000); }  // retro_sleep causes crash
void thread_set_priority(thread_t id, int priority) { sceKernelChangeThreadPriority(id, 0xFF & _thread_map_priority(priority)); }

#elif __SWITCH__

#include <malloc.h>
#include <stdio.h>
#include <switch.h>

static int _thread_map_priority(int priority) {
	switch (priority) {
		case THREAD_PRIORITY_LOWEST:
			return 0x2c + 1;
		case THREAD_PRIORITY_LOW:
			return 0x2c + 10;
		case THREAD_PRIORITY_NORMAL:
			return 0x2c + 15;
		case THREAD_PRIORITY_HIGH:
		case THREAD_PRIORITY_HIGHEST:
		default:
			return 0x2c + 20;
	}
}

static void _thread_func(void* p) {
	void** argp = static_cast<void**>(p);
	threadfunc_t func = reinterpret_cast<threadfunc_t>(argp[0]);
	(*func)(argp[1]);
}

thread_t thread_get() { return 0; }
thread_t thread_run(threadfunc_t func, void* p, int priority) {
	void* argp[2];
	argp[0] = reinterpret_cast<void*>(func);
	argp[1] = p;

	thread_t thread = (thread_t*)malloc(sizeof(Thread));
	printf("creating thread...\n");
	threadCreate((Thread*)thread, _thread_func, argp, 0x10000, _thread_map_priority(priority), 0);
	printf("miau\n");
	threadStart((Thread*)thread);
	printf("started!\n");

	return thread;
}
void thread_sleep(int ms) { svcSleepThread(1000000UL * ms); }
void thread_set_priority(thread_t id, int priority) { svcSetThreadPriority(((Thread*)id)->handle, _thread_map_priority(priority)); }

#else  // non-vita

#include <rthreads/rthreads.h>

static void _thread_func(void* p) {
	void** argp = static_cast<void**>(p);
	threadfunc_t func = reinterpret_cast<threadfunc_t>(argp[0]);
	(*func)(argp[1]);
}

thread_t thread_run(threadfunc_t func, void* p, int priority) {
	void* argp[2];
	sthread_t* thid = NULL;
	argp[0] = reinterpret_cast<void*>(func);
	argp[1] = p;

	thid = sthread_create(_thread_func, &argp);
	sthread_detach(thid);

	return thid;
}

thread_t thread_get() { return 0; }
void thread_sleep(int ms) { retro_sleep(ms); }
void thread_set_priority(thread_t id, int priority) {}

#endif

#endif  // THREADED_RENDERER
