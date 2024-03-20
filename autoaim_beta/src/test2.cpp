#include <iostream>
#include <yaml-cpp/yaml.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "chrono"
#include <type_traits>

auto yaml_config_path = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_beta/src/config.yaml";
auto log_save_path = "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_beta/src/logs/";


int test0();
void test();
int test2();
int test3();
void test4();



template<typename Func, typename ReturnType>
class TimerDecorator
{
private:
    Func func;
public:
    TimerDecorator(Func&& f) : func(std::forward<Func>(f)){}
    template<typename... Args>
    ReturnType operator()(Args&&... args)
    {
        
        auto start = std::chrono::high_resolution_clock::now();
        ReturnType result = func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Duration: " << duration << " microseconds" << std::endl;
        return result;

    }
    ~TimerDecorator(){};
};

template<typename Func>
auto make_timer(Func&& f)
{
    return TimerDecorator<Func,int>(std::forward<Func>(f));
}

int test_function(){
    std::cout << "test_function start" << std::endl;
    for (int i = 0; i < 100000; i++) {
        //std::cout << i << std::endl;
    }    

    std::cout << "test_function end" << std::endl;
    return 1;
}




int main() {
    test3();
    return 0;
}



int test0() {
        try {
        // 读取 YAML 文件
        YAML::Node config = YAML::LoadFile(yaml_config_path);

        // 检查是否存在键为 "name"
        if (config["name"]) {
            // 输出键为 "name" 的值
            std::cout << "Name: " << config["name"].as<std::string>() << std::endl;
        } else {
            std::cerr << "Error: Key 'name' not found in config file." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;

}
void test() {

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
    
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");    
    
    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    
    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

}



int test2() {
    // 设置日志记录级别
    spdlog::set_level(spdlog::level::trace);

    // 创建控制台日志记录器
    auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    console_sink->set_level(spdlog::level::warn); // 控制台输出的日志级别为 warn 及以上
    auto console_logger = std::make_shared<spdlog::logger>("console", console_sink);
    spdlog::register_logger(console_logger);

    // 创建文件日志记录器
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/mylogfile.txt", true); // 第二个参数表示是否追加到现有文件
    file_sink->set_level(spdlog::level::debug); // 文件输出的日志级别为 warn 及以上
    auto file_logger = std::make_shared<spdlog::logger>("file", file_sink);
    spdlog::register_logger(file_logger);

    // 设置默认日志记录器
    spdlog::set_default_logger(file_logger);

    // 测试日志记录
    SPDLOG_TRACE("This is a trace message"); 
    SPDLOG_DEBUG("This is a debug message"); 
    SPDLOG_INFO("This is an info message");  
    SPDLOG_WARN("This is a warning message"); 
    SPDLOG_ERROR("This is an error message"); 
    SPDLOG_CRITICAL("This is a critical message");

    return 0;
}


int test3() {


    // 创建控制台日志 sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    // 创建文件日志 sink
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/mylogfile.log", true); // 第二个参数表示是否追加到现有文件
    file_sink->set_level(spdlog::level::debug);

    // 创建日志记录器并添加 sink
    auto logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, file_sink});
    spdlog::register_logger(logger);
    logger->set_level(spdlog::level::debug);
    // 设置默认日志记录器
    spdlog::set_default_logger(logger);

    // 测试日志记录
    spdlog::trace("This is a trace message"); // 输出到控制台和文件
    spdlog::debug("This is a debug message {0:03.2f}",1.23456); // 输出到控制台和文件
    spdlog::info("This is an info message");   // 输出到控制台和文件
    spdlog::warn("This is a warning message"); // 输出到控制台和文件
    spdlog::error("This is an error message"); // 输出到控制台和文件
    spdlog::critical("This is a critical message"); // 输出到控制台和文件

    return 0;
}
void test4() {
    // get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto unix_timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    std::cout << "Current time: " << std::ctime(&in_time_t) << std::endl;
    std::cout << "Current Unix Timestamp: " << unix_timestamp << std::endl;
}


