#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <sys/stat.h>
#include "md5.h"
#include <limits.h>
#include <unistd.h>
#include <filesystem>
#include <stdio.h>

namespace fs = std::filesystem;
using namespace std;

struct info_type
{
    string type;
    string path;
    string modTime;
    string hash;
    friend istream &operator>>(istream &is, info_type &info)
    {
        getline(is, info.type);
        getline(is, info.path);
        getline(is, info.modTime);
        getline(is, info.hash);
        return is;
    }
};

string getDirectory()
{
    printf("\nAdd path to check\n");
    string path;
    scanf("%s", &path);

    return path;
}

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
        printf("\n[ERROR] Failed to open file %s\n", file_path.c_str());
        return false;
    }
    else
    {
        input_file.close();
        // printf("\n[SUCCESS] File %s is OK\n", file_path.c_str());
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

vector<info_type> getInfoFile(string info_path)
{
    auto file = ifstream(info_path);
    vector<info_type> v;

    for (info_type info; file >> info;)

        v.push_back(info);

    file.close();
    return v;
}

void writeInfoFile(string info_path, vector<info_type> info)
{
    if (existsFile(info_path))
    {
        printf("\n[STATUS] Updating old versioned control file %s\n", info_path.c_str());
        remove(info_path.c_str());
    }
    else
        printf("\n[STATUS] Creating control file %s\n", info_path.c_str());

    ofstream out(info_path);

    for (auto i : info)
    {
        out << i.type << "\n";
        out << i.path << "\n";
        out << i.modTime << "\n";
        out << i.hash << "\n";
    }

    printf("\n[SUCCESS] Updated control file %s\n", info_path.c_str());

    out.close();
}

vector<info_type> getCurrentFilesInfo(string path)
{
    vector<info_type> v;
    info_type current_file;

    for (const auto &entry : fs::directory_iterator(path))
        if (entry.path() != getFileExePath() && entry.path() != path + "/control_file.txt")
        {
            if (entry.is_directory())
                current_file.type = "Folder";
            if (entry.is_regular_file())
                current_file.type = "File";
            current_file.path = entry.path();
            current_file.hash = md5(getFileContent(current_file.path));

            struct stat result;
            time_t modTime;
            if (stat(current_file.path.c_str(), &result) == 0)
                modTime = result.st_mtime;
            stringstream ss;
            ss << put_time(localtime(&modTime), "%Y-%m-%d %H:%M:%S");
            current_file.modTime = ss.str();

            v.push_back(current_file);
        }

    return v;
}

void toLogs(vector<info_type> *old_info, vector<info_type> *current_info)
{

    bool created;
    bool updated;
    bool deleted;

    // vector<string> logs;
    // string log;

    for (auto cur : *current_info)
    {
        created = true;

        for (auto old : *old_info)
            if (cur.path == old.path)
            {

                created = false;

                if (cur.modTime != old.modTime || cur.hash != old.hash)
                {
                    printf("\n[LOG] %s %s was modified at %s\n",
                           cur.type.c_str(),
                           cur.path.c_str(),
                           cur.modTime.c_str());
                }
            }
        if (created)
        {
            printf("\n[LOG] %s %s was created at %s\n",
                   cur.type.c_str(),
                   cur.path.c_str(),
                   cur.modTime.c_str());
        }
    }

    for (auto old : *old_info)
    {
        deleted = true;
        for (auto cur : *current_info)
            if (cur.path == old.path)
                deleted = false;

        if (deleted)
            printf("\n[LOG] %s %s was deleted\n",
                   old.type.c_str(),
                   old.path.c_str());
    }
}

vector<info_type> recursedController(string main_path)
{

    vector<info_type> all_objects(getCurrentFilesInfo(main_path));
    vector<info_type> tmp;

    for (auto obj : all_objects)
    {
        if (obj.type == "Folder")
        {
            tmp = recursedController(obj.path);
            all_objects.insert(all_objects.end(), tmp.begin(), tmp.end());
        }
    }

    return all_objects;
}

int main()
{
    // string directory(getDirectory());
    string directory("/home/surelye/Downloads/sil/t");
    string info_file(directory + "/control_file.txt");

    if (!existsFile(info_file))
    {
        printf("\n\n[WARNING] Directory has no control file. Make new control file and run the program? [y/n]\n");

        char answer;
        cin >> answer;

        if (answer == 'y')
        {
            writeInfoFile(info_file, getCurrentFilesInfo(directory));
        }
        else
            return -1;
    }
    else
    {
        printf("\n\n[WARNING] Do you want to controll folder's siblings? [y/n]\n");
        char answer;
        cin >> answer;

        if (answer == 'y')
        {
            vector<info_type> old = getInfoFile(info_file);
            vector<info_type> neww = recursedController(directory);
            toLogs(&old, &neww);
            writeInfoFile(info_file, neww);
        }
        else
        {
            vector<info_type> old = getInfoFile(info_file);
            vector<info_type> neww = getCurrentFilesInfo(directory);
            toLogs(&old, &neww);
            writeInfoFile(info_file, getCurrentFilesInfo(directory));
        }
    }

    return 0;
}
