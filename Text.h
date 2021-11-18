#ifndef AUTO_BUG_TEXT_H
#define AUTO_BUG_TEXT_H

#include <string>
#include <functional>

#include "DimMap.h"

namespace AutoBug
{

class Text
{
public:
    ~Text() noexcept;
    Text(int dims=0) noexcept;
    Text(const Text& src) noexcept;
    Text(Text&& src) noexcept;

    /*******************************************
     * @brief 获取坐标
     * @return 坐标
     * ****************************************/
    float* pos() noexcept;

    /*******************************************
     * @brief 获取超空间总维数
     * @return 超空间的总维数
     * ****************************************/
    int dims() const noexcept;

    /*******************************************
     * @brief 设置超空间总维数
     * @param[in] dims 超空间的总维数
     * ****************************************/
    void setDims(int dims) noexcept;

    /*******************************************
     * @brief 获取UTF8解码后的文本
     * @return 解码后的文本
     * ****************************************/
    std::wstring text() const noexcept;

    /*******************************************
     * @brief 充填所有维度的坐标
     * @param[in] n 充填的值
     * ****************************************/
    void fill(float n) noexcept;

    /*******************************************
     * @brief 所有维度的坐标进行乘方运算
     * @param[in] n 幂指数
     * ****************************************/
    Text pow(int n) noexcept;

    /*******************************************
     * @brief 对所有维度的坐标执行一次相同的操作,并修
     *        改为返回值
     * @param[in] fn 要进行的操作
     * ****************************************/
    void map(std::function<float(float)> fn) noexcept;

    /*******************************************
     * @brief 对坐标向量进行一次标量运算
     * @param[in] obj 参与的另一个样本
     * @param[in] fn 进行运算的函数
     * @return 运算结果
     * ****************************************/
    Text scalar(const Text& obj, std::function<float(float, float)> fn) const noexcept;

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
     * @brief 计算内部向量的元素之和
     * @return 元素之和
     * ****************************************/
    float sum() const noexcept;

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

    /*******************************************
     * @brief 拷贝赋值
     * @param[in] src 源对象
     * @return 赋值后的当前对象
     * ****************************************/
    Text& operator = (const Text& src) noexcept;

    /*******************************************
     * @brief 移动赋值
     * @param[in] src 源对象
     * @return 赋值后的当前对象
     * ****************************************/
    Text& operator = (Text&& src) noexcept;

    /*******************************************
     * @brief 标量加法运算
     * @param[in] obj 参与运算的另一个对象
     * @return 运算结果
     * ****************************************/
    Text operator + (const Text& src) const;

    /*******************************************
     * @brief 标量减法运算
     * @param[in] obj 参与运算的另一个对象
     * @return 运算结果
     * ****************************************/
    Text operator - (const Text& src) const;

    /*******************************************
     * @brief 标量乘法运算
     * @param[in] obj 参与运算的另一个对象
     * @return 运算结果
     * ****************************************/
    Text operator * (const Text& src) const;

    /*******************************************
     * @brief 标量除法运算
     * @param[in] obj 参与运算的另一个对象
     * @return 运算结果
     * ****************************************/
    Text operator / (const Text& src) const;


private:
    int m_dims;
    float* m_pos;
    std::wstring m_text;
};

}; // namespace AutoBug

#endif // AUTO_BUG_TEXT_H