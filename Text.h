#ifndef AUTO_BUG_TEXT_H
#define AUTO_BUG_TEXT_H

#include <string>

#include "DimMap.h"

namespace AutoBug
{

class Text
{
public:
    ~Text();
    Text();

    /*******************************************
     * @brief 获取超空间总维数
     * @return 超空间的总维数
     * ****************************************/
    int dims();

    /*******************************************
     * @brief 获取UTF8解码后的文本
     * @return 解码后的文本
     * ****************************************/
    std::wstring text();

    /*******************************************
     * @brief 设置文本,采用UTF8解码,扫描并设置超空间坐标
     * @param[in] text 文本原始数据
     * @param[in] dimMap 超空间维度映射
     * ****************************************/
    void setText(const char* text, DimMap& dimMap);

    /*******************************************
     * @brief 打印超空间坐标
     * @param[in] dimMap 超空间维度映射
     * ****************************************/
    void print(DimMap& dimMap);

    /*******************************************
     * @brief 计算与另一个文本之间的欧氏距离
     * @param[in] text 另一个文本
     * @return 两个文本之间的欧氏距离
     * ****************************************/
    double distance(Text& text);


private:
    int m_dims;
    int* m_pos;
    std::wstring m_text;
};

}; // namespace AutoBug

#endif // AUTO_BUG_TEXT_H