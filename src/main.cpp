extern "C" {
    __declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#include <iostream>
#include "core/app.h"

#ifdef _WIN32
#include <windows.h>

// Forward declaration of your real main
extern int main(int argc, char** argv);

// GUI entry point that just calls your normal main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void) hInstance;
    (void) hPrevInstance;
    (void) lpCmdLine;
    (void) nCmdShow;
    return main(__argc, __argv);
}
#endif

int main(int argc, char *argv[])
{
    (void) argc, (void) argv;
    core::main("Adobo");
}
