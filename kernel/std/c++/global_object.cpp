void *__dso_handle;
 
extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso) { return 0; }
extern "C" void __cxa_finalize(void *f) { }