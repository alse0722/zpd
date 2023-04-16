#include <iostream>
#include "md5.h"
#include <fstream>
#include <sstream>
#include <limits.h>
#include <unistd.h>

using namespace std;

string getFileExePath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return string(result, (count > 0) ? count : 0);
}

bool existsFile(string file_path)
{
    ifstream input_file(file_path.c_str(), ios::binary);

    if (!input_file)
    {
        printf("\n\n[ERROR] Failed to open file %s", file_path.c_str());
        return false;
    }
    else
    {
        input_file.close();
        printf("\n\n[SUCCESS] File %s is OK", file_path.c_str());
        return true;
    }
}

string getFileContent(string file_path)
{
    string data;
    ifstream input_file(file_path.c_str(), ios::binary);
    std::ostringstream ss;

    ss << input_file.rdbuf();
    data = ss.str();

    input_file.close();

    return data;
}

bool checkFileHash(string file_path, string hash_path)
{
    string data = getFileContent(file_path);
    string hash = getFileContent(hash_path);

    if (md5(data) == hash)
    {
        printf("\n\n[STATUS] Hash is correct\n", hash_path.c_str());
        return true;
    }
    else
    {
        printf("\n\n[STATUS] Hash is incorrect\n", hash_path.c_str());
        return false;
    }
}

void makeFileHash(string file_path, string hash_path)
{
    string data = getFileContent(file_path);
    ofstream out(hash_path);
    out << md5(data);
    out.close();
    printf("\n\n[STATUS] Created hash %s for file %s", hash_path.c_str(), file_path.c_str());
}

void mainMain()
{
    printf("\n[MAIN] Hello World!\n");
}

// g++ main.cpp md5.cpp -o md5_sample && ./md5_sample

int main()
{

    string path_to_file(getFileExePath());
    string path_to_hash(path_to_file + "_hash");
    bool first_try(!existsFile(path_to_hash));

    if (first_try)
    {
        printf("\n\n[WARNING] Program has no HASH. Make new hash and run the program? [y/n]\n");

        char answer;
        cin >> answer;

        if (answer == 'y')
        {
            makeFileHash(path_to_file, path_to_hash);
            mainMain();
        }
        else
            return -1;
    }
    else
    {
        if (checkFileHash(path_to_file, path_to_hash))
            mainMain();
        else
            return 0;
    }

    return 0;
}