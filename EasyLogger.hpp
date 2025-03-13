#ifndef EASYLOGGER_HPP
#define EASYLOGGER_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <utility>

// Optional file/line tracking
#define EASY_LOG_LOCATION __FILE__, __LINE__

// Easy logging macros with variadic arguments
#define LOG_DEBUG(...)    EasyLogger::Log::debug(__VA_ARGS__)
#define LOG_INFO(...)     EasyLogger::Log::info(__VA_ARGS__)
#define LOG_WARNING(...)  EasyLogger::Log::warning(__VA_ARGS__)
#define LOG_ERROR(...)    EasyLogger::Log::error(__VA_ARGS__)
#define LOG_CRITICAL(...) EasyLogger::Log::critical(__VA_ARGS__)
#define LOG_CUSTOM(level, ...) EasyLogger::Log::custom(level, __VA_ARGS__)

namespace EasyLogger {

struct Colors {
    inline static const std::string RESET   = "\033[0m";
    inline static const std::string RED     = "\033[31m";
    inline static const std::string GREEN   = "\033[32m";
    inline static const std::string YELLOW  = "\033[33m";
    inline static const std::string BLUE    = "\033[34m";
    inline static const std::string MAGENTA = "\033[35m";
    inline static const std::string CYAN    = "\033[36m";
    inline static const std::string WHITE   = "\033[37m";
    inline static const std::string BOLD    = "\033[1m";
};

// Forward declarations
class LogLevel;
class Logger;

// Built-in log levels
enum class BuiltInLevel {
    DEBUG = 10,
    INFO = 20,
    WARNING = 30,
    ERROR = 40,
    CRITICAL = 50
};

// LogLevel class to handle both built-in and custom levels
class LogLevel {
private:
    int _value;
    std::string _name;
    std::string _color;

public:
    // Constructor for built-in levels
    LogLevel(BuiltInLevel level) 
        : _value(static_cast<int>(level)) {
        switch (level) {
            case BuiltInLevel::DEBUG:
                _name = "DEBUG";
                _color = Colors::CYAN;
                break;
            case BuiltInLevel::INFO:
                _name = "INFO";
                _color = Colors::GREEN;
                break;
            case BuiltInLevel::WARNING:
                _name = "WARNING";
                _color = Colors::YELLOW;
                break;
            case BuiltInLevel::ERROR:
                _name = "ERROR";
                _color = Colors::RED;
                break;
            case BuiltInLevel::CRITICAL:
                _name = "CRITICAL";
                _color = Colors::BOLD + Colors::RED;
                break;
        }
    }

    // Constructor for custom levels
    LogLevel(int value, const std::string& name, const std::string& color)
        : _value(value), _name(name), _color(color) {}
    
    // Default constructor
    LogLevel() : _value(0), _name("UNDEFINED"), _color(Colors::WHITE) {}

    int value() const { return _value; }
    const std::string& name() const { return _name; }
    const std::string& color() const { return _color; }

    // Comparison operators
    bool operator<(const LogLevel& other) const {
        return _value < other._value;
    }

    bool operator==(const LogLevel& other) const {
        return _value == other._value;
    }

    bool operator>=(const LogLevel& other) const {
        return _value >= other._value;
    }
};

struct LogConfig {
    std::string logFormat = "[%l] %m";
    bool useHourStamp = false;
    bool useMinuteStamp = false;
    bool useSecondStamp = false;
    bool useDateStamp = false;
    bool logToConsole = true;
    bool logToFile = false;
    LogLevel minLevel = BuiltInLevel::DEBUG;
};

class Logger {
private:
    std::string logFile;
    LogConfig config;
    std::mutex logMutex;
    std::unordered_map<int, LogLevel> customLevels;
    
    static std::unique_ptr<Logger> instance;

    std::string getTimeStamp(int type) {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        struct tm* timeinfo = localtime(&time);
        if (!timeinfo) return "";

        std::ostringstream oss;
        switch (type) {
            case 0: // Hour
                oss << std::setw(2) << std::setfill('0') << timeinfo->tm_hour;
                break;
            case 1: // Minute
                oss << std::setw(2) << std::setfill('0') << timeinfo->tm_min;
                break;
            case 2: // Second
                oss << std::setw(2) << std::setfill('0') << timeinfo->tm_sec;
                break;
            case 3: // Date (YYYY-MM-DD)
                oss << (1900 + timeinfo->tm_year) << "-"
                    << std::setw(2) << std::setfill('0') << (1 + timeinfo->tm_mon) << "-"
                    << std::setw(2) << std::setfill('0') << timeinfo->tm_mday;
                break;
            default:
                return "";
        }
        return oss.str();
    }

    std::string replaceInString(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    std::string formatMessage(
        const std::string& logMessage, 
        const std::string& logLevelName, 
        const std::string& logLevelColor, 
        bool isFile = false) {
        
        std::string formatted = config.logFormat;

        // Replace placeholders
        formatted = replaceInString(formatted, "%l", logLevelName);
        formatted = replaceInString(formatted, "%m", logMessage);

        // Apply time stamps based on config
        if (config.useHourStamp) {
            formatted = replaceInString(formatted, "%Th", getTimeStamp(0));
        }
        if (config.useMinuteStamp) {
            formatted = replaceInString(formatted, "%Tm", getTimeStamp(1));
        }
        if (config.useSecondStamp) {
            formatted = replaceInString(formatted, "%Ts", getTimeStamp(2));
        }
        if (config.useDateStamp) {
            formatted = replaceInString(formatted, "%d", getTimeStamp(3));
        }

        // Apply colors only for console
        if (!isFile) {
            formatted = logLevelColor + formatted + Colors::RESET;
        }

        return formatted + (isFile ? "\n" : "");
    }

public:
    Logger() = default;
    Logger(const std::string& logfile, const LogConfig& conf) 
        : logFile(logfile), config(conf) {}

    // Get or create singleton instance
    static Logger& getInstance() {
        if (!instance) {
            instance = std::make_unique<Logger>();
        }
        return *instance;
    }

    // Initialize with common settings
    static void quickInit(const std::string& logFile = "", 
                         bool console = true, 
                         bool file = false, 
                         const std::string& format = "[%d %Th:%Tm:%Ts] [%l] %m") {
        getInstance()
            .setLogFormat(format)
            .enableConsoleLogging(console)
            .enableTimeStamps()
            .enableDateStamp();
            
        if (file && !logFile.empty()) {
            getInstance().setLogFile(logFile).enableFileLogging();
        }
    }

    // Fluent API for configuration
    Logger& setLogFile(const std::string& filename) {
        logFile = filename;
        config.logToFile = true;
        return *this;
    }

    Logger& setLogFormat(const std::string& format) {
        config.logFormat = format;
        return *this;
    }

    Logger& enableConsoleLogging(bool enable = true) {
        config.logToConsole = enable;
        return *this;
    }

    Logger& enableFileLogging(bool enable = true) {
        config.logToFile = enable;
        return *this;
    }

    Logger& enableTimeStamps(bool hours = true, bool minutes = true, bool seconds = true) {
        config.useHourStamp = hours;
        config.useMinuteStamp = minutes;
        config.useSecondStamp = seconds;
        return *this;
    }

    Logger& enableDateStamp(bool enable = true) {
        config.useDateStamp = enable;
        return *this;
    }

    Logger& setMinLogLevel(const LogLevel& level) {
        config.minLevel = level;
        return *this;
    }

    // Register a custom log level
    LogLevel registerLevel(int value, const std::string& name, const std::string& color = Colors::WHITE) {
        LogLevel level(value, name, color);
        customLevels[value] = level;
        return level;
    }

    // Template to format strings with arguments
    template<typename... Args>
    static std::string format(const std::string& fmt, Args&&... args) {
        constexpr size_t BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        
        int result = snprintf(buffer, BUFFER_SIZE, fmt.c_str(), std::forward<Args>(args)...);
        
        if (result < 0) {
            return "ERROR: String formatting failed";
        }
        
        return std::string(buffer);
    }

    // Generic log method that works with any LogLevel
    template<typename... Args>
    void logf(const LogLevel& level, const std::string& fmt, Args&&... args) {
        if (level < config.minLevel) return;
        
        std::string message = format(fmt, std::forward<Args>(args)...);
        log(message, level);
    }
    
    void log(const std::string& message, const LogLevel& level) {
        if (level < config.minLevel) return;

        std::lock_guard<std::mutex> lock(logMutex);

        if (config.logToConsole) {
            std::cout << formatMessage(message, level.name(), level.color(), false) << std::endl;
        }

        if (config.logToFile && !logFile.empty()) {
            std::ofstream file(logFile, std::ios_base::app);
            if (file.is_open()) {
                file << formatMessage(message, level.name(), "", true);
                file.close();
            } else {
                std::cerr << "[EasyLogger] ERROR: Could not open file " << logFile << std::endl;
            }
        }
    }

    // Convenience methods for built-in levels
    template<typename... Args>
    void debugf(const std::string& fmt, Args&&... args) {
        logf(LogLevel(BuiltInLevel::DEBUG), fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void infof(const std::string& fmt, Args&&... args) {
        logf(LogLevel(BuiltInLevel::INFO), fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warningf(const std::string& fmt, Args&&... args) {
        logf(LogLevel(BuiltInLevel::WARNING), fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void errorf(const std::string& fmt, Args&&... args) {
        logf(LogLevel(BuiltInLevel::ERROR), fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void criticalf(const std::string& fmt, Args&&... args) {
        logf(LogLevel(BuiltInLevel::CRITICAL), fmt, std::forward<Args>(args)...);
    }

    void debug(const std::string& message) {
        log(message, LogLevel(BuiltInLevel::DEBUG));
    }

    void info(const std::string& message) {
        log(message, LogLevel(BuiltInLevel::INFO));
    }

    void warning(const std::string& message) {
        log(message, LogLevel(BuiltInLevel::WARNING));
    }

    void error(const std::string& message) {
        log(message, LogLevel(BuiltInLevel::ERROR));
    }

    void critical(const std::string& message) {
        log(message, LogLevel(BuiltInLevel::CRITICAL));
    }
};

// Initialize the static instance
std::unique_ptr<Logger> Logger::instance = nullptr;

// Static facade class for easier direct logging
class Log {
public:
    // Quick initialization
    static void init(const std::string& logFile = "", 
                    bool console = true, 
                    bool file = false,
                    const std::string& format = "[%d %Th:%Tm:%Ts] [%l] %m") {
        Logger::quickInit(logFile, console, file, format);
    }
    
    // Custom level registration
    static LogLevel registerLevel(int value, const std::string& name, const std::string& color = Colors::WHITE) {
        return Logger::getInstance().registerLevel(value, name, color);
    }
    
    // Set minimum level
    static void setMinLevel(const LogLevel& level) {
        Logger::getInstance().setMinLogLevel(level);
    }
    
    // Log with custom level
    static void custom(const LogLevel& level, const std::string& message) {
        Logger::getInstance().log(message, level);
    }
    
    // Log with custom level and formatting
    template<typename... Args>
    static void custom(const LogLevel& level, const std::string& fmt, Args&&... args) {
        Logger::getInstance().logf(level, fmt, std::forward<Args>(args)...);
    }
    
    // Standard logging methods - simple string versions
    static void debug(const std::string& message) {
        Logger::getInstance().debug(message);
    }
    
    static void info(const std::string& message) {
        Logger::getInstance().info(message);
    }
    
    static void warning(const std::string& message) {
        Logger::getInstance().warning(message);
    }
    
    static void error(const std::string& message) {
        Logger::getInstance().error(message);
    }
    
    static void critical(const std::string& message) {
        Logger::getInstance().critical(message);
    }
    
    // Standard logging methods - with formatting
    template<typename... Args>
    static void debug(const std::string& fmt, Args&&... args) {
        Logger::getInstance().debugf(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void info(const std::string& fmt, Args&&... args) {
        Logger::getInstance().infof(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void warning(const std::string& fmt, Args&&... args) {
        Logger::getInstance().warningf(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void error(const std::string& fmt, Args&&... args) {
        Logger::getInstance().errorf(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void critical(const std::string& fmt, Args&&... args) {
        Logger::getInstance().criticalf(fmt, std::forward<Args>(args)...);
    }
};

} // namespace EasyLogger

#endif // EASYLOGGER_HPP
