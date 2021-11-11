#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "Text.h"
#include "DimMap.h"

namespace AutoBug
{

Text::~Text() noexcept
{
    if (m_pos == nullptr)
        return;

    delete[] m_pos;
    m_dims = 0;
    m_pos = nullptr;
    m_text = L"";
}

Text::Text() noexcept :
    m_dims(0),
    m_pos(nullptr),
    m_text(L"")
{

}

Text::Text(const Text& src) noexcept :
    m_dims(src.m_dims),
    m_pos(new float[m_dims]),
    m_text(src.m_text)
{
    memcpy(m_pos, src.m_pos, sizeof(float) * m_dims);
}

/*******************************************
 * @brief 获取超空间总维数
 * @return 超空间的总维数
 * ****************************************/
int Text::dims() const noexcept
{
    return m_dims;
}

/*******************************************
 * @brief 获取UTF8解码后的文本
 * @return 解码后的文本
 * ****************************************/
std::wstring Text::text() const noexcept
{
    return m_text;
}

/*******************************************
 * @brief 设置文本,采用UTF8解码,扫描并设置超空间坐标
 * @param[in] text 文本原始数据
 * @param[in] dimMap 超空间维度映射
 * ****************************************/
void Text::setText(const char* text, const DimMap& dimMap) noexcept
{
    if (m_pos != nullptr)
        delete[] m_pos;

    m_dims = dimMap.dims();
    m_pos = new float[m_dims];
    memset(static_cast<void*>(m_pos), 0, sizeof(float) * m_dims);

    m_text = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(text);
    for (wchar_t ch : m_text)
    {
        int dim = dimMap.dim(ch);
        if (dim >= 0 && dim < m_dims)
            m_pos[dim] += 1;
    }
}

/*******************************************
 * @brief 设置文本,采用UTF8解码,扫描并设置超空间坐标
 * @param[in] text 文本原始数据
 * @param[in] dimMap 超空间维度映射
 * ****************************************/
void Text::setText(const std::string& text, const DimMap& dimMap) noexcept
{
    setText(text.c_str(), dimMap);
}

/*******************************************
 * @brief 打印超空间坐标
 * @param[in] dimMap 超空间维度映射
 * ****************************************/
void Text::print(const DimMap& dimMap) const noexcept
{
    static wchar_t str[2] = {0, 0};
    for (int i = 0; i < m_dims; i++)
    {
        if (m_pos[i] <= 0)
            continue;

        str[0] = dimMap.word(i);
        printf("%ls: %f\n", str, m_pos[i]);
    }
}

/*******************************************
 * @brief 计算与另一个文本之间的欧氏距离
 * @param[in] text 另一个文本
 * @return 两个文本之间的欧氏距离
 * ****************************************/
float Text::distance(const Text& text) const noexcept
{
    if (m_dims != text.dims())
        return -1;

    float sum = 0.0;
    for (int i = 0; i < m_dims; i++)
    {
        sum += std::pow(m_pos[i] - text.m_pos[i], 2);
    }

    return std::sqrt(sum);
}

/*******************************************
 * @brief 索引一个维度的坐标
 * @param[in] dim 维度
 * @return 该维度上的坐标
 * ****************************************/
float& Text::operator [] (int dim)
{
    if (dim >= m_dims)
        throw std::out_of_range("oversize dim");
    return m_pos[dim];
}

/*******************************************
 * @brief 索引一个维度的坐标
 * @param[in] dim 维度
 * @return 该维度上的坐标
 * ****************************************/
const float& Text::operator [] (int dim) const
{
    if (dim >= m_dims)
        throw std::out_of_range("oversize dim");
    return m_pos[dim];
}

}; // namespace AutoBug