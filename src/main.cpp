#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;

  // TODO: Uncomment the code below to pass the first stage
  std::cout << "$ ";
  std::getline(std::cin, input);
  std::cout << input << "\n";
}
