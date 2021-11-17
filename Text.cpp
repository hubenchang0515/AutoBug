#include <cstdlib>
#include <cstring>
#include <cmath>

#include <string>
#include <locale>
#include <stdexcept>
#include <codecvt>

#include "Text.h"
#include "DimMap.h"
#include "Accelerator.h"

namespace AutoBug
{

Text::~Text() noexcept
{
    if (m_pos != nullptr)
        delete[] m_pos;

    m_dims = 0;
    m_pos = nullptr;
    m_text = L"";
}

Text::Text(int dims) noexcept :
    m_dims(dims),
    m_pos(dims == 0 ? nullptr : new float[dims]),
    m_text(L"")
{
    memset(m_pos, 0, sizeof(float) *m_dims);
}

Text::Text(const Text& src) noexcept :
    m_dims(src.m_dims),
    m_pos(new float[m_dims]),
    m_text(src.m_text)
{
    memcpy(m_pos, src.m_pos, sizeof(float) * m_dims);
}

Text::Text(Text&& src) noexcept :
    m_dims(src.m_dims),
    m_pos(src.m_pos),
    m_text(src.m_text)
{
    src.m_dims = 0;
    src.m_pos = nullptr;
    src.m_text = L"";
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
 * @brief 设置超空间总维数
 * @param[in] 超空间的总维数
 * ****************************************/
void Text::setDims(int dims) noexcept
{
    m_dims = dims;
    m_text = L"";
    if (m_pos != nullptr)
        delete[] m_pos;
    m_pos = new float[m_dims];
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
 * @brief 充填所有维度的坐标
 * @param[in] n 充填的值
 * ****************************************/
void Text::fill(float n) noexcept
{
    for (int i = 0; i < m_dims; i++)
    {
        m_pos[i] = n;
    }
}

/*******************************************
 * @brief 所有维度的坐标进行乘方运算
 * @param[in] n 幂指数
 * ****************************************/
Text Text::pow(int n) noexcept
{
    Text result{m_dims};
    for (int i = 0; i < m_dims; i++)
    {
        result[i] = std::pow(m_pos[i], n);
    }
    return result;
}

/*******************************************
 * @brief 对所有维度的坐标执行一次相同的操作,并修
 *        改为返回值
 * @param[in] fn 要进行的操作
 * ****************************************/
void Text::map(std::function<float(float)> fn) noexcept
{
    for (int i = 0; i < m_dims; i++)
    {
        m_pos[i] = fn(m_pos[i]);
    }
}

/*******************************************
 * @brief 对坐标向量进行一次标量运算
 * @param[in] obj 参与的另一个样本
 * @param[in] fn 进行运算的函数
 * @return 运算结果
 * ****************************************/
Text Text::scalar(const Text& obj, std::function<float(float, float)> fn) const noexcept
{
    Text result{m_dims};
    for (int i = 0; i < m_dims; i++)
    {
        result.m_pos[i] = fn(m_pos[i], obj.m_pos[i]);
    }

    return result;
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
 * @brief 计算内部向量的元素之和
 * @return 元素之和
 * ****************************************/
float Text::sum() const noexcept
{
    float n = 0.0f;
    if (Accelerator::instance().available())
    {
        Accelerator::instance().reduction(m_pos, m_dims, &n);
        return n;
    }

    for (int i = 0; i < m_dims; i++)
    {
        n += m_pos[i];
    }
    return n;
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

    Text diff = *this - text;
    diff = diff * diff;
    return std::sqrt(diff.sum());
}

/*******************************************
 * @brief 索引一个维度的坐标
 * @param[in] dim 维度
 * @return 该维度上的坐标
 * ****************************************/
float& Text::operator [] (int dim)
{
    if (dim >= m_dims)
        throw std::out_of_range("dimension oversize");

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
        throw std::out_of_range("dimension oversize");

    return m_pos[dim];
}

/*******************************************
 * @brief 拷贝赋值
 * @param[in] src 源对象
 * @return 赋值后的当前对象
 * ****************************************/
Text& Text::operator = (const Text& src) noexcept
{
    if (m_pos != nullptr)
        delete[] m_pos;

    m_dims = src.m_dims;
    m_pos = new float[m_dims];
    m_text = src.m_text;
    memcpy(m_pos, src.m_pos, sizeof(float) * m_dims);
    return *this;
}

/*******************************************
 * @brief 移动赋值
 * @param[in] src 源对象
 * @return 赋值后的当前对象
 * ****************************************/
Text& Text::operator = (Text&& src) noexcept
{
    if (m_pos != nullptr)
        delete[] m_pos;

    m_dims = src.m_dims;
    m_pos = src.m_pos;
    m_text = src.m_text;

    src.m_dims = 0;
    src.m_pos = nullptr;
    src.m_text = L"";

    return *this;
}

/*******************************************
 * @brief 标量加法运算
 * @param[in] obj 参与运算的另一个对象
 * @return 运算结果
 * ****************************************/
Text Text::operator + (const Text& obj) const
{
    if (m_dims != obj.m_dims)
        throw std::runtime_error("different dimensions");

    if (Accelerator::instance().available())
    {
        Text result{m_dims};
        Accelerator::instance().add(m_pos, obj.m_pos, m_dims, result.m_pos);
        return result;
    }

    return scalar(obj, [](float x, float y) -> float {return x+y;});
}

/*******************************************
 * @brief 标量减法运算
 * @param[in] obj 参与运算的另一个对象
 * @return 运算结果
 * ****************************************/
Text Text::operator - (const Text& obj) const
{
    if (m_dims != obj.m_dims)
        throw std::runtime_error("different dimensions");

    if (Accelerator::instance().available())
    {
        Text result{m_dims};
        Accelerator::instance().sub(m_pos, obj.m_pos, m_dims, result.m_pos);
        return result;
    }

    return scalar(obj, [](float x, float y) -> float {return x-y;});
}

/*******************************************
 * @brief 标量乘法运算
 * @param[in] obj 参与运算的另一个对象
 * @return 运算结果
 * ****************************************/
Text Text::operator * (const Text& obj) const
{
    if (m_dims != obj.m_dims)
        throw std::runtime_error("different dimensions");

    if (Accelerator::instance().available())
    {
        Text result{m_dims};
        Accelerator::instance().mul(m_pos, obj.m_pos, m_dims, result.m_pos);
        return result;
    }

    return scalar(obj, [](float x, float y) -> float {return x*y;});
}

/*******************************************
 * @brief 标量除法运算
 * @param[in] obj 参与运算的另一个对象
 * @return 运算结果
 * ****************************************/
Text Text::operator / (const Text& obj) const
{
    if (m_dims != obj.m_dims)
        throw std::runtime_error("different dimensions");

    if (Accelerator::instance().available())
    {
        Text result{m_dims};
        Accelerator::instance().div(m_pos, obj.m_pos, m_dims, result.m_pos);
        return result;
    }

    return scalar(obj, [](float x, float y) -> float {return x/y;});
}

}; // namespace AutoBug