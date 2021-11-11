#ifndef AUTO_BUG_DIM_MAP_H
#define AUTO_BUG_DIM_MAP_H

#include <map>

namespace AutoBug
{

class DimMap
{
public:
    /*******************************************
     * @brief 获取一个全局公共实例
     * @return 对象实例
     * ****************************************/
    static DimMap& instance() noexcept;

    DimMap() noexcept;
    
    /*******************************************
     * @brief 输入一个汉字,获取对应的维度
     * @param ch 输入的汉字
     * @return 对应的维度
     * ****************************************/
    int dim(wchar_t ch) const noexcept;

    /*******************************************
     * @brief 输入一个维度,获取对应的汉字
     * @param ch 输入的维度
     * @return 对应的汉字
     * ****************************************/
    wchar_t word(int dim) const noexcept;

    /*******************************************
     * @brief 获取超空间总维数
     * @return 超空间的总维数
     * ****************************************/
    int dims() const noexcept;

private:
    std::map<wchar_t, int> m_dimMap;
    std::map<int, wchar_t> m_wordMap; 
};

}; // namespace AutoBug

#endif // AUTO_BUG_COMMONLY_USED_CHARACTERS_H