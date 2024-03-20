
#ifndef BASIC_HPP
#define BASIC_HPP
#include "spdlog/spdlog.h"
#include <yaml-cpp/yaml.h>
#include <csignal>


enum class Mode 
{
    Dbg,
    Rel
};
enum class IF_SUCCESS
{
    SUCCESS = true,
    FAIL = false
};



class Params
{
private:
public:
    Params();
    ~Params();
    virtual bool load_params_from_yaml(const std::string& file_path) = 0;
    virtual void print_show_params() = 0;

    template<typename T>
    void safe_load_key(YAML::Node& config, const std::string& key, T& value)
    {
        if(config[key])
        {
            value = config[key].as<T>();
        }
        else
        {
            SPDLOG_ERROR("Key {0} not found in config file {1}",key,config.as<std::string>());
            throw std::runtime_error("Key not found in config file");
        }
    }

};


class Timer
{
private:
    std::chrono::high_resolution_clock::time_point start_time;
public:
    Timer();
    ~Timer(){};
    void reset();
    double elapsed() const;
};

void INIT_GLOBAL_LOGGER(const std::string& save_folder, 
                        const spdlog::level::level_enum& save_log_level,
                        const spdlog::level::level_enum& console_log_level,
                        const bool if_enable_logging);



bool CHECK_IF_FILE_EXIST(const std::string& file_path);


void signal_handler(int sig);

#endif // BASIC_HPP 

