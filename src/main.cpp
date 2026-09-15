#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

int main() {
    // 출력이 버퍼에 남지 않도록 즉시 flush 한다.
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string input1;
    std::string input2;

    std::string line;

    // 쉘은 명령어를 계속 받기 때문에 while문을 돌렸다.
    while (true) {
        std::cout << "$ ";
        // 그냥 cin을 쓰면 공백을 만나면 멈추기 때문에 getline을 썼다.
        std::getline(std::cin, line);
        // 공백을 기준으로 세기 때문에 stringstream을 썼다.
        std::istringstream ss(line);
        std::string word;
        ss >> word;

        if (word == "type") {
            while (getline(ss, word, ':')) {
                if (word == "echo" || word == "exit" || word == "type") {
                    std::cout << word << " is a shell builtin";
                    break;
                }
                // printf("Word : %s\n", word.c_str());
                for (const auto &entry: fs::directory_iterator(word)) {
                    // 파일이름이 word와 일치하고, 그냥 파일일 때 `is regular_file()`, 실행권한이 있는 파일일 때
                    if (entry.path().filename() == word && entry.is_regular_file() == true && access(
                            entry.path().c_str(), X_OK) == 0) {
                        std::cout << word << " is " << entry.path();
                        break;
                    }
                }
                std::cout << word << ": not found";
            }
        } else if (word == "exit") {
            return 0;
        } else if (word == "echo") {
            while (ss >> word) {
                std::cout << word << " ";
            }
        } else {
            std::cout << word;
            std::cout << ": command not found";
        }
        std::cout << "\n";
    }
}
