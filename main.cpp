#include <cstdio>
#include "DimMap.h"
#include "Text.h"
#include "DataLoader.h"
#include "Kmeans.h"
#include "Accelerator.h"

using namespace AutoBug;

class Classifier
{
public:
    ~Classifier() noexcept = default;
    Classifier() noexcept = default;

    /*******************************************
     * @brief 输入数据集进行学习,会清空以前的数据
     * @param[in] dataset 数据集
     * @param[in] n 初始分类数量
     * @return 最终分类数量
     * ****************************************/
    size_t learn(std::vector<Text> dataset, size_t n=0) noexcept
    {
        size_t k = n == 0 ? (dataset.size() + 4) / 5 : n;
        Kmeans kmeans{dataset, k};
        kmeans.learn(0.1f);

        for (size_t idx = 0; idx < k; idx++)
        {
            auto group = kmeans.group(idx);
            m_groupCenters.push_back(kmeans.groupCenter(idx));
            m_groups.push_back(group);
        }

        for (size_t idx = 0; idx < m_groups.size();)
        {
            if (m_groups[idx].size() > 3)
            {
                m_split(idx);
            }
            else
            {
                idx++;
            }
        }

        return m_groups.size();
    }

    /*******************************************
     * @brief 打印学习后的各个分组
     * ****************************************/
    void print() noexcept
    {
        for (size_t i = 0; i < m_groups.size(); i++)
        {
            printf("Group %zu:\n", i);
            for (auto& item : m_groups[i])
            {
                printf("\t%ls\n", item.text().c_str());
            }
        }
    }

    /*******************************************
     * @brief 获取分组数量
     * @return 分组数量
     * ****************************************/
    size_t groupCount() const noexcept
    {
        return m_groups.size();
    }

    /*******************************************
     * @brief 获取指定的分组中心
     * @param[in] idx 分组序号
     * @return 分组的中心
     * ****************************************/
    Text groupCenter(size_t idx) const noexcept
    {
        return m_groupCenters[idx];
    }

    /*******************************************
     * @brief 获取指定的分组
     * @param[in] idx 分组序号
     * @return 分组的数据
     * ****************************************/
    std::vector<Text> group(size_t idx) const noexcept
    {
        return m_groups[idx];
    }

private:
    std::vector<Text> m_groupCenters;
    std::vector<std::vector<Text>> m_groups;

    /*******************************************
     * @brief 对一个分组进行拆分,分成多个新的分组,会
     *        在末尾添加新的分组,并删除当前分组,因此
     *        当前索引会引用到下一个分组
     * @param[in] idx 要拆分的分组都序号
     * @param[in] n 期望的平均分组大小,默认为3
     * ****************************************/
    void m_split(size_t idx, int n=3)
    {
        auto dataset = m_groups[idx];
        size_t k = (dataset.size() + n - 1) / n;
        Kmeans kmeans{dataset, k};
        kmeans.learn(0.1f);

        for (size_t i = 0; i < k; i++)
        {
            auto group = kmeans.group(i);
            m_groupCenters.push_back(kmeans.groupCenter(i));
            m_groups.push_back(group);
        }

        m_groupCenters.erase(m_groupCenters.begin() + idx);
        m_groups.erase(m_groups.begin() + idx);
    }
};

int main()
{
    setlocale(LC_ALL, "");
    // Accelerator::instance().setEnable(false);
    if (Accelerator::instance().available())
    {
        printf("Use GPU: %s\n", Accelerator::instance().name().c_str());
        printf("Max Work Size: %zu\n", Accelerator::instance().maxLocalSize());
    }
    auto dataset = DataLoader::load("train.txt", DimMap::instance());
    Classifier classifier;
    classifier.learn(dataset);
    classifier.print();

    // Kmeans k{dataset, 10};
    // k.learn(0.1);
    // k.print();
}