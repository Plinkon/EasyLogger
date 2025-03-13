#include "../EasyLogger.hpp"
#include <iostream>
#include <thread>
#include <vector>

// This example showcases all EasyLogger features

void threadFunction(int id, const EasyLogger::LogLevel& customLevel);

int main() {
    //
    // 1. ADVANCED INITIALIZATION AND CONFIGURATION
    //
    
    // Get logger instance directly for advanced configuration
    auto& logger = EasyLogger::Logger::getInstance()
        .setLogFile("application.log")
        .setLogFormat("[%d %Th:%Tm:%Ts] [%l] %m")
        .enableTimeStamps()
        .enableDateStamp()
        .enableFileLogging()
        .enableConsoleLogging();
    
    LOG_INFO("Logger initialized with custom configuration");
    
    //
    // 2. CUSTOM LOG LEVELS
    //
    
    // Register custom log levels
    auto TRACE = EasyLogger::Log::registerLevel(5, "TRACE", EasyLogger::Colors::BLUE);
    auto SUCCESS = EasyLogger::Log::registerLevel(25, "SUCCESS", 
                   EasyLogger::Colors::GREEN + EasyLogger::Colors::BOLD);
    auto AUDIT = EasyLogger::Log::registerLevel(35, "AUDIT", EasyLogger::Colors::MAGENTA);
    auto METRIC = EasyLogger::Log::registerLevel(15, "METRIC", EasyLogger::Colors::CYAN + EasyLogger::Colors::BOLD);
    
    // Log with custom levels
    LOG_CUSTOM(TRACE, "Function entry point");
    LOG_CUSTOM(SUCCESS, "Operation completed successfully");
    LOG_CUSTOM(AUDIT, "User 'admin' performed deletion of record #1234");
    LOG_CUSTOM(METRIC, "Memory usage: %d MB", 1024);
    
    //
    // 3. FORMAT CUSTOMIZATION
    //
    
    // Change format during runtime
    logger.setLogFormat("[PID:%d][%d %Th:%Tm:%Ts] [%l] %m");
    LOG_INFO("Log format changed to include process ID");
    
    // Reset to default format
    logger.setLogFormat("[%d %Th:%Tm:%Ts] [%l] %m");
    
    //
    // 4. LEVEL FILTERING
    //
    
    LOG_INFO("About to change minimum log level to WARNING");
    
    // Set minimum log level to filter less severe messages
    EasyLogger::Log::setMinLevel(EasyLogger::LogLevel(EasyLogger::BuiltInLevel::WARNING));
    
    // These won't be displayed
    LOG_DEBUG("This debug message won't appear");
    LOG_INFO("This info message won't appear");
    LOG_CUSTOM(TRACE, "This trace message won't appear");
    
    // These will be displayed
    LOG_WARNING("This warning will be displayed");
    LOG_ERROR("This error will be displayed");
    LOG_CRITICAL("This critical message will be displayed");
    
    // Reset to show all levels
    EasyLogger::Log::setMinLevel(EasyLogger::LogLevel(EasyLogger::BuiltInLevel::DEBUG));
    LOG_INFO("Reset minimum log level to DEBUG");
    
    //
    // 5. ADVANCED STRING FORMATTING
    //
    
    // Printf-style formatting with various types
    int count = 42;
    double pi = 3.14159265359;
    std::string name = "EasyLogger";
    void* ptr = &pi;
    
    LOG_INFO("Integer: %d", count);
    LOG_INFO("Float with precision: %.3f", pi);
    LOG_INFO("String: %s", name.c_str());
    LOG_INFO("Pointer address: %p", ptr);
    LOG_INFO("Mixed types: %s has counted %d items with average value %.2f", 
             name.c_str(), count, pi);
    
    //
    // 6. THREAD SAFETY DEMONSTRATION
    //
    
    LOG_INFO("Starting thread demonstration");
    
    // Create multiple threads that log simultaneously
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(threadFunction, i, AUDIT);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    LOG_INFO("Thread demonstration completed");
    
    //
    // 7. DISABLE LOGGING DESTINATIONS
    //
    
    // Disable file logging but keep console
    logger.enableFileLogging(false);
    LOG_INFO("File logging disabled, this will only appear in console");
    
    // Disable console but enable file
    logger.enableConsoleLogging(false).enableFileLogging(true);
    logger.info("Console logging disabled, this will only appear in file");
    
    // Re-enable console for final message
    logger.enableConsoleLogging(true);
    LOG_INFO("Advanced usage example completed");
    
    return 0;
}

// Thread function for demonstrating thread safety
void threadFunction(int id, const EasyLogger::LogLevel& customLevel) {
    for (int i = 0; i < 3; i++) {
        // Log using both standard and custom levels from threads
        LOG_INFO("Thread %d: iteration %d", id, i);
        LOG_CUSTOM(customLevel, "Thread %d: custom log at iteration %d", id, i);
        
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
