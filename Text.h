#ifndef AUTO_BUG_TEXT_H
#define AUTO_BUG_TEXT_H

#include <string>

#include "DimMap.h"

namespace AutoBug
{

class Text
{
public:
    ~Text() noexcept;
    Text() noexcept;
    Text(const Text& src) noexcept;

    /*******************************************
     * @brief 获取超空间总维数
     * @return 超空间的总维数
     * ****************************************/
    int dims() const noexcept;

    /*******************************************
     * @brief 获取UTF8解码后的文本
     * @return 解码后的文本
     * ****************************************/
    std::wstring text() const noexcept;

    /*******************************************
     * @brief 设置文本,采用UTF8解码,扫描并设置超空间坐标
     * @param[in] text 文本原始数据
     * @param[in] dimMap 超空间维度映射
     * ****************************************/
    void setText(const char* text, const DimMap& dimMap) noexcept;

    /*******************************************
     * @brief 设置文本,采用UTF8解码,扫描并设置超空间坐标
     * @param[in] text 文本原始数据
     * @param[in] dimMap 超空间维度映射
     * ****************************************/
    void setText(const std::string& text, const DimMap& dimMap) noexcept;

    /*******************************************
     * @brief 打印超空间坐标
     * @param[in] dimMap 超空间维度映射
     * ****************************************/
    void print(const DimMap& dimMap) const noexcept;

    /*******************************************
     * @brief 计算与另一个文本之间的欧氏距离
     * @param[in] text 另一个文本
     * @return 两个文本之间的欧氏距离
     * ****************************************/
    float distance(const Text& text) const noexcept;

    /*******************************************
     * @brief 索引一个维度的坐标
     * @param[in] dim 维度
     * @return 该维度上的坐标
     * ****************************************/
    float& operator [] (int dim); 

    /*******************************************
     * @brief 索引一个维度的坐标
     * @param[in] dim 维度
     * @return 该维度上的坐标
     * ****************************************/
    const float& operator [] (int dim) const; 


private:
    int m_dims;
    float* m_pos;
    std::wstring m_text;
};

}; // namespace AutoBug

#endif // AUTO_BUG_TEXT_H