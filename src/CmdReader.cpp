#include "../inc/CmdReader.h"

    std::shared_ptr<CmdReader> CmdReader::Create(size_t num_cmds,std::istream& istream) {
        auto ptr = std::shared_ptr<CmdReader>{ new CmdReader{num_cmds,istream}};
        return ptr;
    }
    CmdReader::~CmdReader() {
        std::cout<<"Destroy CmdReader"<<std::endl;
    }

    void CmdReader::Subscribe(const std::shared_ptr<Observer>& obs) {
        m_observers.emplace_back(obs);
    }
    
/// @brief Notifies subscribers and sends batch of cmds to their   
    void CmdReader::Notify() {
        if (m_cmds.empty()) return;   
        
        for (auto it=m_observers.begin();it!=m_observers.end();++it) {
            auto ptr = it->lock();
            if (ptr) {
                std::stringstream ss = FormBatch();
                ptr->Update(ss);
            }
            else
                m_observers.erase(it);
        }
        m_cmds.resize(0);
    }
    
    void CmdReader::NewCmd(const std::string& cmd)
    { 
        if (cmd.empty()) return;
     
            if (cmd=="{") {
               if ((m_cnt_braces==0) && !m_cmds.empty()) Notify();
               ++m_cnt_braces;
            }
            else {
                if (cmd=="}") {
                    if (m_cnt_braces>0) {
                        --m_cnt_braces;
                        if (m_cnt_braces==0 && !m_cmds.empty()) Notify();
                    }
                    else {
                        throw std::runtime_error("Unnecessary symbol '}' in input stream");
                    }
                }
                else {
                    m_cmds.emplace_back(cmd);
                    if (m_cnt_braces==0 && m_cmds.size()==m_num_cmds) Notify();
                }
            }
             std::cout<<m_context<<std::endl;
    }
    void CmdReader::SetContext(void* a_context)
    {
        m_context = a_context;
    }

    CmdReader::CmdReader(size_t num_cmds,std::istream& istream) 
        : m_num_cmds{num_cmds},
          m_istream(istream),
          m_cnt_braces{0}   {
                 m_cmds.reserve(m_num_cmds);
                 std::cout<<"Construct CmdReader"<<std::endl;
    }

    std::stringstream CmdReader::FormBatch() {
        std::stringstream ss;
        for (auto it_cmd = m_cmds.cbegin();it_cmd!=m_cmds.cend();it_cmd++) {
            if (it_cmd !=m_cmds.cbegin())
                ss<< ", "<<*it_cmd;
            else
                ss<<"bulk: "<<*it_cmd;
        }
       // std::cout<<ss.str()<<std::endl;
        return ss;     
    }