int test() {
    for (int i = 0; i < 10000; ++i);
    return 0;
}

extern "C" int _start() {
    test();
    return 0;
}