#include "inc/async.h"
#include "iostream"
#include <chrono>
#include <thread>
#include <future>
#include <memory>
/// @file
/// @brief Main function. Test for processing commands.
/// @author btv<example@example.com>
void Test_1() {
  std::size_t bulk = 5;
  auto h = async::connect(bulk);
  auto h2 = async::connect(1);        
  async::receive(h, "1", 1);
  async::receive(h2, "1\n", 2);
  async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
  async::receive(h, "b\nc\nd\n}\n89\n", 11);
  async::disconnect(h);
  async::disconnect(h2);
}

void Test_2()
{
  async::handle_t h2 = async::connect(2);
  async::receive(h2, "21\n", 3);
  async::receive(h2, "22\n", 3);
  async::receive(h2, "23\n", 3);
  async::receive(h2, "24\n", 3);
  async::receive(h2, "25\n", 3);
  async::receive(h2, "26\n", 3);
  async::disconnect(h2);
  std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main(int argc, const char** argv) 
{  
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
    return 1;
  }

  long lBlockSize{0};
  try {
    lBlockSize = std::stoi(argv[1]);
  }
  catch (const std::invalid_argument& ex) {
    std::cerr << "Error: Block size is incorrect!" << std::endl;
    return 1;
  }
  catch (const std::out_of_range& ex) {
    std::cerr << "Error: Block size is out of range!" << std::endl;
    return 1;
  }

  if (lBlockSize == 1) {
    std::cerr << "Error: Block size must be greater then zero!" << std::endl;
    return 1;
  }

  if (lBlockSize < 0) {
    std::cerr << "Error: Block size must not be negative!" << std::endl;
    return 1;
  }

  async::handle_t h1 = async::connect(lBlockSize);

  auto result_1 = std::async(std::launch::async, Test_1);
  auto result_2 = std::async(std::launch::async, Test_2);

  std::string strLine;
  while ( std::getline(std::cin, strLine) ) {
    async::receive(h1, strLine.c_str(), strLine.size());
  }
  
  async::disconnect(h1);

  result_1.wait();
  result_2.wait();

  return 0;
}
