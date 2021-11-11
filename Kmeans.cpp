#include "Kmeans.h"

namespace AutoBug
{

Kmeans::Kmeans() noexcept :
    m_k(0)
{

}

Kmeans::Kmeans(const std::vector<Text>& dataset, size_t k) noexcept :
    m_k(k),
    m_dataset(dataset)
{
    m_groupCenters.resize(m_k);
    m_groups.resize(m_k);
}

/*******************************************
 * @brief 设置数据集
 * @param[in] dataset 数据集
 * ****************************************/
void Kmeans::setData(const std::vector<Text>& dataset) noexcept
{
    m_dataset = dataset;
}

/*******************************************
 * @brief 设置分组数量
 * @param[in] k 分组数量
 * ****************************************/
void Kmeans::setGroupCount(size_t k) noexcept
{
    m_k = k;
    m_groupCenters.resize(m_k);
    m_groups.resize(m_k);
}

/*******************************************
 * @brief 开始进行学习
 * ****************************************/
void Kmeans::learn() noexcept
{
    int dims = m_dataset[0].dims();

    // 随机选取k个样本作为初始中心点,这里直接选取前k个样本
    for (size_t i = 0; i < m_k; i++)
    {
        m_groupCenters[i] = m_dataset[i];
    }

    while (true)
    {
        // 清空上次的分组
        for (size_t i = 0; i < m_groups.size(); i++)
        {
            m_groups[i].clear();
        }

        // 将所有样本划分到距离最近的中心点
        for (size_t sample = 0; sample < m_dataset.size(); sample++)
        {
            auto& s = m_dataset[sample];
            size_t groupId = 0;
            for (size_t group = 1; group < m_k; group++)
            {
                auto& g0 = m_groupCenters[groupId];
                auto& g1 = m_groupCenters[group];
                if (s.distance(g1) < s.distance(g0))
                {
                    groupId = group;
                }
            }
            m_groups[groupId].push_back(s);
        }

        // 标记中心点移动距离的最大值
        float maxDelta = 0.0f;

        // 更新中心点的坐标为该组所有点坐标的平均值
        for (size_t group = 0; group < m_k; group++)
        {
            Text newCenter{dims};
            for (size_t i = 0; i < m_groups[group].size(); i++)
            {
                newCenter.add(m_groups[group][i]);
            }
            newCenter.map([this, group](float n) -> float {return n/m_groups[group].size();});

            // 计算中心点的移动距离
            float delta = newCenter.distance(m_groupCenters[group]);
            if (delta > maxDelta)
            {
                maxDelta = delta;
            }
            m_groupCenters[group] = newCenter;
        }

        // 中心点位置稳定,结束学习
        printf("MaxDelta: %f\n", maxDelta);
        if (maxDelta < 0.1)
            break;
    }
}

/*******************************************
 * @brief 打印学习后的各个分组
 * ****************************************/
void Kmeans::print() noexcept
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
size_t Kmeans::groupCount() noexcept
{
    return m_k;
}

/*******************************************
 * @brief 获取指定的分组
 * @param[in] idx 分组序号
 * @return 分组的数据
 * ****************************************/
std::vector<Text> Kmeans::group(size_t idx) noexcept
{
    return m_groups[idx];
}

}; // namespace AutoBug