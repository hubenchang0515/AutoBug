#ifndef AUTO_BUG_KMEANS_H
#define AUTO_BUG_KMEANS_H

#include <vector>
#include "Text.h"

namespace AutoBug
{

class Kmeans
{
public:
    ~Kmeans() noexcept = default;
    Kmeans() noexcept;
    Kmeans(const std::vector<Text>& dataset, size_t k) noexcept;

    /*******************************************
     * @brief 设置数据集
     * @param[in] dataset 数据集
     * ****************************************/
    void setData(const std::vector<Text>& dataset) noexcept;

    /*******************************************
     * @brief 设置分组数量
     * @param[in] k 分组数量
     * ****************************************/
    void setGroupCount(size_t k) noexcept;

    /*******************************************
     * @brief 进行学习
     * ****************************************/
    void learn() noexcept;

    /*******************************************
     * @brief 打印学习后的各个分组
     * ****************************************/
    void print() noexcept;

    /*******************************************
     * @brief 获取分组数量
     * @return 分组数量
     * ****************************************/
    size_t groupCount() noexcept;

    /*******************************************
     * @brief 获取指定的分组
     * @param[in] idx 分组序号
     * @return 分组的数据
     * ****************************************/
    std::vector<Text> group(size_t idx) noexcept;

private:
    size_t m_k;
    std::vector<Text> m_dataset;
    std::vector<Text> m_groupCenters;
    std::vector<std::vector<Text>> m_groups;
    
};

}; // namespace AutoBug

#endif // AUTO_BUG_KMEANS_H