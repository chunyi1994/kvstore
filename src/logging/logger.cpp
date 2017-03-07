#include "logger.h"
#include "singleton.h"

#include <iostream>
#include <ctime>
#include <string>
#include <utility>
#include <thread>

namespace logging {

Logger::LogLevel DEFAULT_LEVEL = Logger::eTRACE;

typename Logger::LogLevel Logger::level_limit_ = DEFAULT_LEVEL;

typename Logger::OutputCallback Logger::output_callback_ = [](std::string msg){
    std::cout<<msg<<std::endl;
};

const char* LEVEL_STRING[] = { "TRACE", "DEBUG", "INFO",
                               "WARN", "ERROR", "FATAL"};

std::string get_time_str() {
    std::time_t t;
    std::time(&t);
    std::string str = std::ctime(&t);
    //去掉回车
    str.erase(str.size() - 1, 1);
    return str;
}

Logger::Logger(const char *filename, int line,
               const char *func_name, Logger::LogLevel level) {
    sstream_<<"[";
    sstream_<<LEVEL_STRING[level];
    sstream_<<"] ";
    sstream_<<get_time_str() << " ";
  //  sstream_<<"tid:" << std::this_thread::get_id()<<" ";
    sstream_<< filename << " ";
    sstream_<<"(" << line<<") ";
    sstream_<<func_name<<"";
    sstream_<<"：";
}

Logger::~Logger() {
    if (output_callback_) {
        std::string str = sstream_.str();
        output_callback_(std::move(str));
    }
}

Logger::LogLevel Logger::log_level() {
    return Logger::level_limit_;
}

void Logger::set_level(Logger::LogLevel level) {
    Logger::level_limit_ = level;
}

void Logger::set_output(OutputCallback cb) {
    Logger::output_callback_ = std::move(cb);
}

std::stringstream &Logger::stream() {
    return sstream_;
}


} // namespace


