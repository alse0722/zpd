#include <iostream>
#include "md5.h"
#include <fstream>
#include <sstream>
#include <string>
#include <limits.h>
#include <unistd.h>

using namespace std;

string getexepath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return string(result, (count > 0) ? count : 0);
}

string getfilestring(string file_path)
{
    string data;
    ifstream input_file(file_path.c_str(), ios::binary);
    
    if (!input_file)
    {
        printf("\n\n!!! Failed to open file %s\n\n", file_path.c_str());
        return "FILE_ERROR";
    }

    std::ostringstream ss;
    ss << input_file.rdbuf();
    data = ss.str();

    input_file.close();

    return data;
}

bool checkhash(string file_path, string hash_path){
    
}

// g++ main.cpp md5.cpp -o md5_sample && ./md5_sample

int main(int argc, char *argv[])
{
    string path_to_file(getexepath());
    string path_to_hash(path_to_file + "_hash");
    string file(getfilestring(path_to_file));
    string hash(getfilestring(path_to_hash));

    if (file == "FILE_ERROR" || hash == "FILE_ERROR")
        return 0;
    

    printf("\nPath to bin: %s\n", path_to_file.c_str());
    
    printf("\nMD5 of file: %s\n", md5(file).c_str());

    return 0;
}