#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <vector>

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
        // istringstream은 읽기용 stringstream은 쓰기용 의도가 잘 전달되게 하기위하여 구분해서 쓴다.
        std::istringstream ss(line);
        std::string word;
        ss >> word;

        // 필요한곳에 쓴다. getenv로 Path를 받아서 string으로 변환 stirng을 파싱을 하여서 각각의 path를 directory_iterator에 넣는다.
        std::string type_path = std::getenv("PATH");
        std::istringstream pp(type_path);
        std::string path_env;
        if (access(word.c_str(), X_OK) == 0) {
            bool fileFound = false;
            while (getline(pp, path_env, ':')) {
                for (const auto &entry: fs::directory_iterator(path_env)) {
                    // 파일이름이 word와 일치하고, 그냥 파일일 때 `is regular_file()`, 실행권한이 있는 파일일 때
                    if (entry.path().filename() == word && entry.is_regular_file() == true) {
                        // shell에서 실행파일과 인자를 받았을 때 그 프로그램에 인자를 넣어주는 코드(가변적인 인자)
                        /**
                         * execvp 특성상 char *로 다 받기 때문에 vector를 사용해서 가변인자를 넣었고,
                         * vector의 size를 세서 다시 char *argv에 넣었다.
                         **/
                        std::string command;
                        std::vector<char*> argv_vector;
                        while (ss >> word) {
                            argv_vector.push_back(word.data());
                        }
                        // execvp는 마지막에 종료를 알리는 nullptr이 필요해서 size() + 1을 하였다.
                        char* argv[argv_vector.size() + 1];
                        for (int i = 0; i < argv_vector.size(); i++) {
                            argv[i] = argv_vector[i];
                        }
                        argv[argv_vector.size()] = nullptr;
                        //파일 실행과 인자를 넣음
                        // c++ 17 이상에서는 string타입의 word가 data()를 붙이면 char* 된다. c_str()은 const char*이 되고, data()는 수정이 된다.
                        // data()가 조금 더 현대적이라고 한다.
                        execvp(command.data(), argv);
                        fileFound = true;
                        break;
                    }
                }
            }
            if (fileFound == true) {
                break;
            }
        } else if (word == "type") {
            bool fileFound = false;
            ss >> word;
            if (word == "echo" || word == "exit" || word == "type") {
                std::cout << word << " is a shell builtin";
                std::cout << "\n";
                continue;
            }
            while (getline(pp, path_env, ':')) {
                for (const auto &entry: fs::directory_iterator(path_env)) {
                    // 파일이름이 word와 일치하고, 그냥 파일일 때 `is regular_file()`, 실행권한이 있는 파일일 때
                    if (entry.path().filename() == word && entry.is_regular_file() == true && access(
                            entry.path().c_str(), X_OK) == 0) {
                        std::cout << word << " is " << entry.path().string();
                        fileFound = true;
                        break;
                    }
                }
                if (fileFound == true) {
                    break;
                }
            }
            if (fileFound == false) {
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
