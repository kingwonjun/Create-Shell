#include <iostream>
#include <string>
#include <sstream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input1;
  std::string input2;

  std::string line;

  while (true)
  {
    std::cout << "$ ";  
    std::getline(cin, line);
    stringstream ss(line);
    std::string word;
    ss >> word;

    if (word == "exit")
    {
      return 0;
    }
    else if (word == "echo")
    {
      while (ss >> word)
      {
        cout << word;
      }
    }
    else {
      std::cout << word;
      std::cout << ": command not found";
    }
    std::cout << "\n";
  }
}

