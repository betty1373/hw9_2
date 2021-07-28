#ifndef Logger_H
#define Logger_H
#include <fstream>
#include <atomic>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "CmdReader.h"
/// @file
/// @brief Class outs data from stringstream to console
/// @author btv<example@example.com>
class ConsoleLogger : public Observer, public std::enable_shared_from_this<ConsoleLogger> {
public:
/// @brief Create object class - PatternCreater
     static std::shared_ptr<ConsoleLogger> Create(const std::string& name, std::shared_ptr<CmdReader>& reader); ;
     ~ConsoleLogger(); ;
/// @brief Outs data from stringstream to console
    void Update(std::stringstream& ss) override;
    void SetContext(void* a_context);;
private: 

    ConsoleLogger(const std::string& m_name);
/// @brief Subscribe to receiving notifications
   void SetCmdReader(std::shared_ptr<CmdReader>& _reader);
    void Work(std::string prefix);
    void StopWork();
    void* m_context;
    std::weak_ptr<CmdReader> m_reader;
   
    std::mutex m_mutex;
    std::mutex m_outmutex;
    std::thread m_thread;
    std::atomic<bool> m_stop;
    std::condition_variable m_cv;
    std::queue<std::string> m_deque;
};
/// @file
/// @brief Class outs data from stringstream to file
/// @author btv<example@example.com>
class FileLogger : public Observer, public std::enable_shared_from_this<FileLogger> {
public:  
/// @brief Create object class - PatternCreater  
     static std::shared_ptr<FileLogger> Create(const std::string& name,std::shared_ptr<CmdReader>& reader);
     ~FileLogger();
/// @brief Outs data from stringstream to file
    void Update(std::stringstream& ss) override;
    void SetContext(void* a_context);
private:
    FileLogger(const std::string& name);
    void Work(std::string postfix);
    void StopWork();
/// @brief Subscribe to receiving notifications
    void SetCmdReader(std::shared_ptr<CmdReader>& _reader);
    void* m_context;
    std::weak_ptr<CmdReader> m_reader;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::atomic<bool> m_stop;
    std::condition_variable m_cv;
    std::queue<std::string> m_deque;
};
#endif