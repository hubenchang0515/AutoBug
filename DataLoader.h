#ifndef AUTO_BUG_DATA_LOADER_H
#define AUTO_BUG_DATA_LOADER_H

#include <vector>

#include "DimMap.h"
#include "Text.h"

namespace AutoBug
{

class DataLoader
{
public:
    /*******************************************
     * @brief 从文本文件中加载一个数据集,每行为一个样本
     * @param[in] file 文件名
     * @param[in] dimMap 超空间维度映射
     * @return 样本集
     * ****************************************/
    static std::vector<Text> load(const char* file, const DimMap& dimMap) noexcept;

private:
    /*******************************************
     * @brief 从文本文件中读取一行
     * @param[in] fp 文件指针
     * @return 一行数据
     * ****************************************/
    static std::string readline(FILE* fp) noexcept;

    /*******************************************
     * @brief 删除字符串两端的空白字符
     * @param[in] str 原字符串
     * @return 去除两端空白后的字符串
     * ****************************************/
    static std::string trimSpace(const std::string& str) noexcept;
};

}; // namespace AutoBug

#endif // AUTO_BUG_DATA_LOADER_H