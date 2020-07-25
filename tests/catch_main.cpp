#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <iostream>
#include "utils/logger.h"

using namespace std;
int main(int argc, char* argv[])
{
    // Don't spam cout with debug messages
    Log.setLevel(&cout, LogLevel::LOG_WARNING);
    
    cout << "Running tests...\n";
    cout << "Remember to CONNECT THE CAMERA or tests will fail!\n\n\n";

    int result = Catch::Session().run(argc, argv);

    return result;
}