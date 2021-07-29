#ifndef CmdReader_H
#define CmdReader_H
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <chrono>
#include "Observer.h"
/// @file
/// @brief Class for processing commands and notifying subscribers.
/// @author btv<example@example.com>
class CmdReader : public Observable {
public:
/// @brief Create object class - PatternCreater
    static std::shared_ptr<CmdReader> Create(size_t num_cmds,std::istream& istream=std::cin);
    ~CmdReader() {}
/// @brief Adds subscribers to list
    void Subscribe(const std::shared_ptr<Observer>& obs) override;    
/// @brief Notifies subscribers and sends batch of cmds to their   
    void Notify() override;
    void Work();
/// @brief Processes cmds and inform subscribers
    void NewCmd(const std::string& cmd);
private:
/// @brief Private Constructor
    CmdReader(size_t num_cmds,std::istream& istream=std::cin);
/// @brief Forms batch to stringstream
    std::stringstream FormBatch();

    size_t m_num_cmds;
    std::istream& m_istream;
    size_t m_cnt_braces;
    std::vector<std::string> m_cmds;
    std::list<std::weak_ptr<Observer>> m_observers;
};

#endif