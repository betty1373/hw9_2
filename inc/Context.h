#ifndef Context_H
#define Context_H
#include <sstream>
#include "Logger.h"
/// @file
/// @brief Class context for async processing commands
/// @author btv<example@example.com>
class Context
{
public:
/// @brief Construct object class
  Context(std::size_t numCmds); 
/// @brief Destruct object class
  ~Context();
/// @brief Get new data
  void NewData(const char* data, std::size_t size);

private:
/// @brief Process async
  void Work();
/// @brief Stop thread
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