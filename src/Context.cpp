#include "../inc/Context.h"
#include "string"
  Context::Context(std::size_t numCmds)
     : m_stop{false}
     //,
      
 //     m_thread{&Context::Work, this}
  {
    m_thread = std::thread(&Context::Work, this);
    m_cmdReader =  CmdReader::Create(static_cast<size_t>(numCmds));
     
    m_consoleLogger = ConsoleLogger::Create("log",m_cmdReader);
    m_fileLogger = FileLogger::Create("file",m_cmdReader);
    
    m_cmdReader->SetContext(this);
    m_consoleLogger->SetContext(this);
    m_fileLogger->SetContext(this);
    std::cout<<"Construct Context"<<std::endl;    
  }

  Context::~Context() {
      StopWork();
       std::cout<<"Destroy Context"<<std::endl;
  }

  void Context::NewData(const char* data, std::size_t size) {
    {
       std::unique_lock<std::mutex> locker(m_mutex);
       m_strstream.write(data,size);
    }
    m_notify =  true;
    m_cv.notify_one();
  }

  void Context::Work() 
  { 
    while (!m_stop) {     
     // std::cout<<"Context Work"<<std::endl;     
      std::unique_lock<std::mutex> locker(m_mutex);
      m_cv.wait(locker, [&](){return m_notify || m_stop;});
      std::string cmd;
      while (std::getline(m_strstream, cmd) ) {
        m_cmdReader->NewCmd(cmd);
      }
      m_strstream.clear();
      m_strstream.str("");
      m_notify = false;
      // std::cout<<"Context Work after"<<std::endl; 
    }
  }
  void Context::StopWork()
  {
        m_stop = true;
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
  }
