#include "basic.hpp"
#include <sys/stat.h>
#include <chrono>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>
#include <csignal>



volatile __sig_atomic_t g_sig_int_flag = 0;

Params::Params(){}
Params::~Params(){}


Timer::Timer()
{
    this->reset();   
}
void Timer::reset()
{
    start_time = std::chrono::high_resolution_clock::now();
}


/**
 * @brief Returns the elapsed time in seconds.
*/
double Timer::elapsed() const
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();
    auto duration = end - start;
    return duration * 0.000001; 
}


void INIT_GLOBAL_LOGGER(const std::string& save_folder, 
                        const spdlog::level::level_enum& save_log_level,
                        const spdlog::level::level_enum& console_log_level,
                        const bool if_enable_logging)
{
    if(if_enable_logging)
    {
    
        spdlog::set_level(spdlog::level::trace);
        auto cur_time = std::chrono::system_clock::now();
        time_t cur_time_t = std::chrono::system_clock::to_time_t(cur_time);
        std::string cur_time_str = std::ctime(&cur_time_t);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(console_log_level);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(save_folder +"/log_"+ cur_time_str +".log", true);
        file_sink->set_level(save_log_level);
        
        auto logger = std::make_shared<spdlog::logger>("lr1", spdlog::sinks_init_list{console_sink, file_sink});
        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
        
        //spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l %t");

    }
    else
    {
        spdlog::set_level(spdlog::level::off);
    }
}
                        
bool CHECK_IF_FILE_EXIST(const std::string& file_path)
{
    struct stat buffer;
    return (stat (file_path.c_str(), &buffer) == 0);
}


std::string replace_extension(const std::string& file_name, 
                              const std::string& new_extension)
{
    size_t last_dot = file_name.find_last_of(".");
    if (last_dot != std::string::npos)
    {
        return file_name.substr(0, last_dot) +"."+ new_extension;
    }
    return file_name +"." + new_extension;
}




void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        std::cout << "Program interrupted by user." << std::endl;
        g_sig_int_flag = 1;
    }
    
    
}



