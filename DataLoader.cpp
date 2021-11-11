#include "DataLoader.h"
#include <cstdio>
#include <cstring>
#include <cerrno>

namespace AutoBug
{

/*******************************************
 * @brief 从文本文件中加载一个数据集,每行为一个样本
 * @param[in] file 文件名
 * @param[in] dimMap 超空间维度映射
 * @return 样本集
 * ****************************************/
std::vector<Text> DataLoader::load(const char* file, const DimMap& dimMap) noexcept
{
    std::vector<Text> data;

    FILE* fp = fopen(file, "rb");
    if (fp == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return data;
    }

    do
    {
        auto line = readline(fp);
        if (line == "")
            continue;
        Text text;
        text.setText(line, dimMap);
        data.push_back(text);
    }while (!feof(fp));

    fclose(fp);
    return data;
}

/*******************************************
 * @brief 从文本文件中读取一行
 * @param[in] fp 文件指针
 * @return 一行数据
 * ****************************************/
std::string DataLoader::readline(FILE* fp) noexcept
{
    #ifndef BUFFER_SIZE
    #define BUFFER_SIZE 8

    std::string line;
    char buffer[BUFFER_SIZE];

    while(true)
    {
        fgets(buffer, BUFFER_SIZE, fp);
        line += buffer;
        int len = strlen(buffer);
        if (feof(fp) || len == 0 || buffer[len-1] == '\n' || buffer[len-1] == '\r')
        {
            return trimSpace(line);
        }
    }

    #undef BUFFER_SIZE
    #endif // BUFFER_SIZE
}

/*******************************************
 * @brief 删除字符串两端的空白字符
 * @param[in] str 原字符串
 * @return 去除两端空白后的字符串
 * ****************************************/
std::string DataLoader::trimSpace(const std::string& str) noexcept
{
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == str.npos)
        return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

}; // namespace AutoBug