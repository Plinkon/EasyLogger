/*
 * EasyLogger:
 - a nice simple implementation to log stuff either in the console or in files
 - trying to value ease of use and simplicity
*/

#ifndef EASYLOGGER_HPP
#define EASYLOGGER_HPP

// Includes
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>

// Namespace
namespace EasyLogger {

    // Log levels
    enum DefaultLogLevels { DEBUG, INFO, WARN, ERROR };
    template<typename LogLevels = DefaultLogLevels> // to specify custom log levels, if you dont specify it goes back to default

    // Logger class
    class Logger {
        private:
            LogLevels logLevel;
            std::string logFile;

        public:
            Logger() = default; // default constructor so if you dont want to specify a log file
            Logger(const std::string& logfile) : logFile(logfile) {
                this->logFile = logfile;
            }

            bool setLogLevel(LogLevels level) {
                this->logLevel = level;

                if (this->logLevel != level) return false;
                else return true;
            }

            void printLogLevel(std::string message) {
                std::cout << "Current Log Level: " << static_cast<int>(logLevel) << std::endl;
            }
    };
};

#endif // EASYLOGGER_HPP