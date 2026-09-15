#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

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
        // access(word.c_str(), X_OK) == 0 첫 if문 분기에 넣었지만, path를 고려 안하였다.
        // path에 간다음 확인을 하는 작업을 해야겠다.
        if (word == "type") {
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
            // stringstream 아까 맨 위에서 ss >> word가 나와 이미 단어 하나가 지나간 상태에서
            // 인자의 개수와 인자값이 정상적으로 출력될려면
            // stringstream을 초기화하고 처음 위치로 되돌려야한다.
            ss.clear();
            ss.seekg(0);
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
                        std::vector<char *> char_argv_vector;
                        std::vector<std::string> string_argv_vector;
                        // 벡터가 push_back을 통해 메모리 상의 원소들을 연속해서 보관한다면
                        // 확보된 공간이 없을경우 다른 곳으로 이동(주소값 바뀜) 더 큰 공간으로 이사한다.
                        // 따라서 `string_argv_vector.push_back(word) 이 한줄만 반복문에 쓰고
                        // 다음에 string 벡터의 값들의 주소값을 넣어준다.
                        while (ss >> word) {
                            string_argv_vector.push_back(word);
                        }
                        // execvp는 마지막에 종료를 알리는 nullptr이 필요해서 size() + 1을 하였다.
                        char *argv[string_argv_vector.size() + 1];
                        for (int i = 0; i < string_argv_vector.size(); i++) {
                            argv[i] = string_argv_vector[i].data();
                        }
                        argv[string_argv_vector.size()] = nullptr;
                        //파일 실행과 인자를 넣음
                        // c++ 17 이상에서는 string타입의 word가 data()를 붙이면 char* 된다. c_str()은 const char*이 되고, data()는 수정이 된다.
                        // data()가 조금 더 현대적이라고 한다.
                        std::string command = entry.path().filename().string();
                        // execvp (프로그램 실행과 인자들)를 실행하면 다음 코드가 멈추게 된다.
                        // 이에 fork()라는 함수를 사용하여 프로세스 복제를 한 뒤 pid
                        // pid < 0 : fork 실패, pid == 0 : 자식 프로세스, pid > 0 부모 프로세스
                        pid_t pid = fork();
                        if (pid == 0) {
                            wait(nullptr);
                            execvp(command.c_str(), argv);
                        }
                        fileFound = true;
                        break;
                    }
                }
                if (fileFound == false){
                    std::cout << word << ": not found";
                }
                break;
            }
        }
        std::cout << "\n";
    }
}
