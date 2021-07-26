#ifndef Logger_H
#define Logger_H
#include <fstream>
#include <atomic>
#include <deque>
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
     static std::shared_ptr<Observer> Create(const std::string& name, std::shared_ptr<CmdReader>& reader) {
        auto ptr = std::shared_ptr<ConsoleLogger>{ new ConsoleLogger (name)};
        ptr->SetCmdReader(reader);
        return ptr;
    }
     virtual ~ConsoleLogger() {
         StopWork();
         std::cout<<"Destroy ConsoleLogger"<<std::endl;
     };
/// @brief Outs data from stringstream to console
    void Update(std::stringstream& ss) override {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_deque.push_back(ss.str());
        }
        m_cv.notify_all();
    };
private:  

    ConsoleLogger(const std::string& m_name) :
        m_stop {false},
        m_thread {&ConsoleLogger::Work,this,m_name}
    {}
/// @brief Subscribe to receiving notifications
   void SetCmdReader(std::shared_ptr<CmdReader>& _reader) {
        m_reader = _reader;
        auto ptr = m_reader.lock();
        if (ptr) {
            ptr->Subscribe(shared_from_this());
        }
    }
    void Work(std::string prefix)
    {
        
        while (!m_stop)
        {
            std::string ss;
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_cv.wait(lk,[&]() {return !m_deque.empty() || m_stop;});

                if (!m_deque.empty()) {
                    ss = m_deque.front();
                    m_deque.pop_front();
                }
            }
            if (ss.length()>0)
            {
                std::unique_lock<std::mutex> lk(m_outmutex); 
                std::cout<<ss<<std::endl;
            }
        }
    }
    void StopWork()
    {
        m_stop = true;
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
    std::weak_ptr<CmdReader> m_reader;
   
    std::mutex m_mutex;
    std::mutex m_outmutex;
    std::thread m_thread;
    std::atomic<bool> m_stop;
    std::condition_variable m_cv;
    std::deque<std::string> m_deque;
};
/// @file
/// @brief Class outs data from stringstream to file
/// @author btv<example@example.com>
class FileLogger : public Observer, public std::enable_shared_from_this<FileLogger> {
public:  
/// @brief Create object class - PatternCreater  
     static std::shared_ptr<Observer> Create(const std::string& name,std::shared_ptr<CmdReader>& reader) {
        auto ptr = std::shared_ptr<FileLogger>{ new FileLogger(name)};
        ptr->SetCmdReader(reader);
        return ptr;
    }
     virtual ~FileLogger() {
         StopWork();
         std::cout<<"Destroy FileLogger"<<std::endl;
     };
/// @brief Outs data from stringstream to file
    void Update(std::stringstream& ss) override {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_deque.push_back(ss.str());
        }
        m_cv.notify_one();
    };
private:
    FileLogger(const std::string& name) : m_stop {false}
    {
        for (size_t i=0;i<2;i++) {
            std::string m_name = name + std::to_string(i);
            m_threads.emplace_back(std::thread(&FileLogger::Work,this,m_name));
        }

    }
    void Work(std::string postfix)
    {
        while (!m_stop)
        {
            std::string ss;
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_cv.wait(lk,[&]() {return !m_deque.empty() || m_stop;});

                if (!m_deque.empty()) {
                    ss = m_deque.front();
                    m_deque.pop_front();
                }
            }
            if (ss.length()>0)
            {
                auto time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                auto fileName{"bulk_"+postfix+"_"+time+".log"};
                std::ofstream log(fileName,std::ios::out);
                log << ss << std::endl;
                log.close();
            }
        }
    }
    void StopWork()
    {
        m_stop = true;
        m_cv.notify_all();
        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    };
/// @brief Subscribe to receiving notifications
    void SetCmdReader(std::shared_ptr<CmdReader>& _reader) {
        m_reader = _reader;
        auto ptr = m_reader.lock();
        if (ptr) {
            ptr->Subscribe(shared_from_this());
        }
    }
    std::weak_ptr<CmdReader> m_reader;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::atomic<bool> m_stop;
    std::condition_variable m_cv;
    std::deque<std::string> m_deque;
};
#endif