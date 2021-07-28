#ifndef Context_H
#define Context_H
#include <sstream>
#include "Logger.h"

class Context
{
public:
  Context(std::size_t numCmds); 
  ~Context();

  void NewData(const char* data, std::size_t size);

private:
  void Work();
  void StopWork();
  
private:
  std::shared_ptr<CmdReader> m_cmdReader;
  std::shared_ptr<ConsoleLogger> m_consoleLogger;
  std::shared_ptr<FileLogger> m_fileLogger;

  std::stringstream m_strstream;

  std::atomic<bool> m_stop;
  std::atomic<bool> m_notify;
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cv;
};
#endif