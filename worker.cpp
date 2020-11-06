
// Libraries
#include <iostream>

#include "app_worker.hpp"

// Program entry-point function.
int main(int argc, char* argv[])
{
    // Structure for passing command-line arguments.
    // The definition of this structure is platform-specific.
    CefMainArgs main_args(argc, argv);

    std::cout << "HTML render worker running." << std::endl;

    // Execute the sub-process logic. This will block until the sub-process should exit.
    return CefExecuteProcess(main_args, new app_worker(), nullptr);
}
