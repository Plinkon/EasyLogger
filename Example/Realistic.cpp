#include "../EasyLogger.hpp"
#include <iostream>
#include <cmath>

// Example demonstrating mixed usage patterns from simple to intermediate

int main() {
    //
    // PART 1: SIMPLE USAGE
    //
    
    // Quick setup for console only
    EasyLogger::Log::init();
    
    // Basic logging
    LOG_INFO("Application started");
    LOG_WARNING("This is a warning message");
    
    // Simple variable logging
    int count = 5;
    LOG_DEBUG("Processing %d items", count);
    
    //
    // PART 2: INTERMEDIATE USAGE
    //
    
    // Enable file logging too
    EasyLogger::Logger::getInstance()
        .setLogFile("app.log")
        .enableFileLogging();
    
    LOG_INFO("Now logging to both console and file");
    
    // Create a custom log level for success messages
    auto SUCCESS = EasyLogger::Log::registerLevel(25, "SUCCESS", 
                  EasyLogger::Colors::GREEN + EasyLogger::Colors::BOLD);
    
    // Calculate something
    double result = 0;
    for (int i = 1; i <= count; i++) {
        result += std::sqrt(i);
        LOG_DEBUG("Added sqrt(%d) = %.4f, running total: %.4f", i, std::sqrt(i), result);
    }
    
    // Log with custom level
    LOG_CUSTOM(SUCCESS, "Calculation complete! Final result: %.4f", result);
    
    // Temporarily filter out debug messages
    LOG_INFO("Changing log level to INFO");
    EasyLogger::Log::setMinLevel(EasyLogger::LogLevel(EasyLogger::BuiltInLevel::INFO));
    
    LOG_DEBUG("This debug message won't appear");
    LOG_INFO("This info message will appear");
    
    // Reset log level
    EasyLogger::Log::setMinLevel(EasyLogger::LogLevel(EasyLogger::BuiltInLevel::DEBUG));
    LOG_DEBUG("Debug messages visible again");
    
    LOG_INFO("Application completed");
    
    return 0;
}
