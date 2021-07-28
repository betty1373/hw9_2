#include "../inc/Logger.h"

    std::shared_ptr<ConsoleLogger> ConsoleLogger::Create(const std::string& name, std::shared_ptr<CmdReader>& reader) {
        auto ptr = std::shared_ptr<ConsoleLogger>{ new ConsoleLogger {name}};
        ptr->SetCmdReader(reader);
        return ptr;
    }
     ConsoleLogger::~ConsoleLogger() {
         StopWork();
         std::cout<<"Destroy ConsoleLogger"<<std::endl;
     }

    void ConsoleLogger::Update(std::stringstream& ss) {
        {
            std::unique_lock<std::mutex> lk(m_mutex);          
            m_deque.push(ss.str());
        }
        //std::cout<<ss.str()<<std::endl;
        m_cv.notify_all();
    }
    void ConsoleLogger::SetContext(void* a_context)
    {
        m_context = a_context;
    }

    ConsoleLogger::ConsoleLogger(const std::string& m_name) :
        m_stop {false},
        m_thread {&ConsoleLogger::Work,this,m_name}
    {
         std::cout<<"Construct ConsoleLogger"<<std::endl;
    }
   void ConsoleLogger::SetCmdReader(std::shared_ptr<CmdReader>& _reader) {
        m_reader = _reader;
        auto ptr = m_reader.lock();
        if (ptr) {
            ptr->Subscribe(shared_from_this());
        }
    }
    void ConsoleLogger::Work(std::string prefix)
    { 
        std::string ss; 
        while (!m_stop)
        {               
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_cv.wait(lk,[&]() {return !m_deque.empty() || m_stop;});
            
                if (!m_deque.empty()) {
                    ss = m_deque.front();
                    m_deque.pop();
                }
            }
            // std::cout<<ss.size()<<std::endl;
            if (ss.size())
            {
               // std::unique_lock<std::mutex> lk(m_outmutex); 
               // std::cout<<ss<<std::endl;
               ss.clear();
            }
            
        }
    }
    void ConsoleLogger::StopWork()
    {
        m_stop = true;
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
    
    std::shared_ptr<FileLogger> FileLogger::Create(const std::string& name,std::shared_ptr<CmdReader>& reader) {
        auto ptr = std::shared_ptr<FileLogger>{ new FileLogger {name}};
        ptr->SetCmdReader(reader);
        return ptr;
    }
     FileLogger::~FileLogger() {
         StopWork();
         std::cout<<"Destroy FileLogger"<<std::endl;
     }

    void FileLogger::Update(std::stringstream& ss) {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_deque.push(ss.str());
        }
       // std::cout<<ss.str()<<std::endl;
        m_cv.notify_one();
    };
    void FileLogger::SetContext(void* a_context)
    {
        m_context = a_context;
    }
    FileLogger::FileLogger(const std::string& name) : m_stop {false}
    {
         std::cout<<"Construct FileLogger"<<std::endl;
        for (size_t i=0;i<2;++i) {
            std::string m_name = name + std::to_string(i+1);
            m_threads.emplace_back(std::thread(&FileLogger::Work,this,m_name));
        }
      
    }
    void FileLogger::Work(std::string postfix)
    {
        auto i = 0;
        std::string ss;
        while (!m_stop)
        {            
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_cv.wait(lk,[&]() {return !m_deque.empty() || m_stop;});

                if (!m_deque.empty()) {
                     ss = m_deque.front();
                     m_deque.pop();
                }
            }
            if (ss.size()>0)
            {
                i++;
                auto time = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
                std::stringstream fileName;
                fileName<< "bulk_"<<postfix<<"_"<<m_context<<"_"<<time<<"_"<<i<<".log";
                std::ofstream log(fileName.str(),std::ios::out);
                log << ss << std::endl;
                log.close();
                ss.clear();
            }
           
        }
    }
    void FileLogger::StopWork()
    {
        m_stop = true;
        m_cv.notify_all();
        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void FileLogger::SetCmdReader(std::shared_ptr<CmdReader>& _reader) {
        m_reader = _reader;
        auto ptr = m_reader.lock();
        if (ptr) {
            ptr->Subscribe(shared_from_this());
        }
    }