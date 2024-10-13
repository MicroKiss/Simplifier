#include "SimplifierApp.hpp"

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>
class MemoryLeakDetector {
  public:
    MemoryLeakDetector () {
        _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    }

    ~MemoryLeakDetector () {
        if (_CrtDumpMemoryLeaks ()) {
            std::cout << "Memory leaks detected!" << std::endl;
        } else {
            std::cout << "No memory leaks detected." << std::endl;
        }
    }
};
#endif

int main (int argc, char *argv[]) {
#ifdef DEBUG
    MemoryLeakDetector memoryLeakDetector;
#endif

    SimplifierApp app;
    if (app.Init (argc, argv)) {
        app.Run ();
    }

    return 0;
}