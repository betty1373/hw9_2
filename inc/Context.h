#ifndef Context_H
#define Context_H
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <chrono>
#include "Logger.h"

class Context
{
public:
  Context(std::size_t numCmds) : m_stop(false),
   m_thread {&Context::Work,this} {
    m_cmdReader =  CmdReader::Create(numCmds);
    m_consoleLogger = ConsoleLogger::Create("log",m_cmdReader);
    m_fileLogger = FileLogger::Create("file",m_cmdReader);
    m_cmdReader->SetContext(this);
    m_consoleLogger->SetContext(this);
    m_fileLogger->SetContext(this);
  }
  ~Context() {
      StopWork();
  };

  void ProccessBuffer(const char* a_Buffer, std::size_t a_szSize);

private:
  void Work()
{
  while (!m_stop) {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_cv.wait(locker, [&](){return m_notify || m_stop;});
    std::string strLine;
    while (std::getline(m_ssInputStream, strLine) ) {
      m_cmdReader->ProccessLine(strLine);
    }
    m_ssInputStream.clear();
    m_ssInputStream.str("");
    m_notify = false;
  }
}
  void StopWork()
    {
        m_stop = true;
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    };
  
private:
  std::shared_ptr<CmdReader> m_cmdReader;
  std::shared_ptr<ConsoleLogger> m_consoleLogger;
  std::shared_ptr<FileLogger> m_fileLogger;

  std::stringstream m_stringStream;

  std::atomic<bool> m_stop;
  std::atomic<bool> m_notify;
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cv;
};

#endif