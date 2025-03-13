#include "../EasyLogger.hpp"
#include <iostream>

// Simple usage example - bare minimum for easy logging

int main() {
    // Quick one-line initialization (console output only)
    EasyLogger::Log::init();
    
    // Log messages with different severity levels
    LOG_DEBUG("Debug information");
    LOG_INFO("Program started");
    LOG_WARNING("This is a warning");
    LOG_ERROR("An error occurred");
    LOG_CRITICAL("System failure");
    
    // Simple string formatting with variables
    int count = 42;
    std::string item = "apples";
    LOG_INFO("Found %d %s", count, item.c_str());
    
    // You can also log directly without macros
    EasyLogger::Log::info("Program completed");
    
    return 0;
}
