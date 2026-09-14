#include <iostream>
#include <string>
#include <sstream>

int main() {
  // 출력이 버퍼에 남지 않도록 즉시 flush 한다.
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input1;
  std::string input2;

  std::string line;

  // 쉘은 명령어를 계속 받기 때문에 while문을 돌렸다.
  while (true)
  {
    std::cout << "$ "; 
    // 그냥 cin을 쓰면 공백을 만나면 멈추기 때문에 getline을 썼다.
    std::getline(std::cin, line);
    // 공백을 기준으로 세기 때문에 stringstream을 썼다.
    std::stringstream ss(line);
    std::string word;
    ss >> word;
    
    if (word == "type")
    {
      ss >> word;
      if (word == "echo" || word == "exit" || word == "type")
      {
        std::cout << word << " is a shell builtin";
      }
      else 
      {
        std::cout << word << ": not found";
      }
    }
    else if (word == "exit")
    {
      return 0;
    }
    else if (word == "echo")
    {
      while (ss >> word)
      {
        std::cout << word << " ";
      }
    }
    else {
      std::cout << word;
      std::cout << ": command not found";
    }
    std::cout << "\n";
  }
}

