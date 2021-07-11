```
#include <iosteram> //stdin,out
#include <fstream> //file stream
#incude <sstream> // memory stream
```

```
#include <iostream>
#include <fstream>
#include <sstream>
class LogFile {
public:
    LogFile(const std::string &file_name) : _log_file(file_name) {
        fs.open(_log_file, std::fstream::app);
    }
    void log(const std::string &rhs) {
        fs << rhs << std::endl;
    }
private:
    std::string _log_file;
    std::fstream fs;
};
int main(int argc, char* argv[]) {
    std::string tmp_buf;
    std::ifstream in("text.txt"); // in, out, app, ate,trunc, binary
    // std::ofstream out("text.txt", std::ofstream::app);
    LogFile log_file("text.txt");
    int cnt = 0;
    while(in >> tmp_buf) {
        std::cout << tmp_buf << std::endl;
        ++cnt;
        log_file.log(std::to_string(cnt));
    }

}
```