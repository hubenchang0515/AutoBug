#ifndef AUTO_BUG_ACCELERATOR_H
#define AUTO_BUG_ACCELERATOR_H

#ifndef CL_HPP_TARGET_OPENCL_VERSION 
#define CL_HPP_TARGET_OPENCL_VERSION 200
#endif // CL_HPP_TARGET_OPENCL_VERSION
#include <CL/cl2.hpp>

#include <map>
#include <vector>
#include <string>

namespace AutoBug
{

class Accelerator
{
public:
    /*******************************************
     * @brief 获取一个全局公共实例
     * @return 对象实例
     * ****************************************/
    static Accelerator& instance() noexcept;

    ~Accelerator() noexcept;
    Accelerator() noexcept;
    Accelerator(const Accelerator&) = delete;
    Accelerator(Accelerator&&) = delete;

    /*******************************************
     * @brief 检查加速器是否可用
     * @return 加速器是否可用
     * ****************************************/
    bool available() const noexcept;

    /*******************************************
     * @brief 开关加速器
     * @param[in] enable 是否启用
     * ****************************************/
    void setEnable(bool enable) noexcept;

    /*******************************************
     * @brief 获取设备名称
     * @return 设备名
     * ****************************************/
    std::string name() const noexcept;

    /*******************************************
     * @brief 获取一组任务的最大工作数量
     * @return 一组任务的最大工作数量
     * ****************************************/
    size_t maxLocalSize() const noexcept;

    /*******************************************
     * @brief 计算一组任务的工作数量
     * @param[in] n 任务总数
     * @return 一组任务的工作数量
     * ****************************************/
    size_t localSize(size_t n) const noexcept;

    /*******************************************
     * @brief 计算总工作数量
     * @param[in] n 任务总数
     * @return 总工作数量
     * ****************************************/
    size_t globalSize(size_t n) const noexcept;

    /*******************************************
     * @brief 获取一个核函数
     * @param[in] name 核函数的名字
     * @return 核函数
     * ****************************************/
    cl_kernel kernel(const std::string& name) noexcept;

    /*******************************************
     * @brief 获取一个缓存
     * @param[in] name 缓存的名字
     * @return 缓存
     * ****************************************/
    cl_mem buffer(const std::string& name) const noexcept;

    /*******************************************
     * @brief 创建一个缓存
     * @param[in] name 缓存的名字
     * @param[in] bytes 缓存的大小
     * @return 创建的缓存
     * ****************************************/
    cl_mem createBuffer(const std::string& name, size_t bytes) noexcept;

    /*******************************************
     * @brief 写一个缓存
     * @param[in] name 缓存名字
     * @param[in] offset 缓存内位置
     * @param[in] ptr 数据
     * @param[in] byets 数据大小
     * @param[in] block 是否阻塞
     * @return 是否成功
     * ****************************************/
    bool writeBuffer(const std::string& name, size_t offset, void* ptr, size_t bytes, bool block) noexcept;

    /*******************************************
     * @brief 读一个缓存
     * @param[in] name 缓存名字
     * @param[in] offset 缓存内位置
     * @param[in] ptr 数据
     * @param[in] byets 数据大小
     * @param[in] block 是否阻塞
     * @return 是否成功
     * ****************************************/
    bool readBuffer(const std::string& name, size_t offset, void* ptr, size_t bytes, bool block) const noexcept;

    /*******************************************
     * @brief 给核函数设置参数
     * @param[in] kernel 核函数的名字
     * @param[in] i 参数序号,从0开始
     * @param[in] arg 参数
     * @param[in] size 参数的大小
     * @return 是否成功
     * ****************************************/
    bool setArg(cl_kernel kernel, int i, void* arg, size_t size) noexcept;

    /*******************************************
     * @brief 调用一个核函数
     * @param[in] kernel 运算核函数
     * @param[in] localSize 一组工作项的数量
     * @param[in] globalSize 总工作项的数量
     * @return 是否成功
     * ****************************************/
    bool invoke(cl_kernel kernel, size_t localSize, size_t globalSize) const noexcept;

    /*******************************************
     * @brief 调用一个核函数
     * @param[in] kernel 运算核函数
     * @param[in] localSize 一组工作项的数量
     * @param[in] globalSize 总工作项的数量
     * @param[in] argc 参数个数
     * @param[in] ... 传给核函数的参数,必须是
     * @return 是否成功
     * ****************************************/
    bool invoke(cl_kernel kernel, size_t localSize, size_t globalSize, size_t argc, ...) const noexcept;

    /*******************************************
     * @brief 对向量进行一次标量运算
     * @param[in] v1 进行运算的向量1
     * @param[in] v2 进行运算的向量2
     * @param[in] n 向量长度
     * @param[in] kernel 运算核函数
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool scalar(const float* v1, const float* v2, size_t n, cl_kernel kernel, float* ret) const noexcept;


    /*******************************************
     * @brief 对向量进行一次标量加法运算
     * @param[in] v1 向量1
     * @param[in] v2 向量2
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool add(const float* v1, const float* v2, size_t n, float* ret) const noexcept;

    /*******************************************
     * @brief 对向量进行一次标量减法运算
     * @param[in] v1 向量1
     * @param[in] v2 向量2
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool sub(const float* v1, const float* v2, size_t n, float* ret) const noexcept;

    /*******************************************
     * @brief 对向量进行一次标量乘法运算
     * @param[in] v1 向量1
     * @param[in] v2 向量2
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool mul(const float* v1, const float* v2, size_t n, float* ret) const noexcept;

    /*******************************************
     * @brief 对向量进行一次标量除法运算
     * @param[in] v1 向量1
     * @param[in] v2 向量2
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool div(const float* v1, const float* v2, size_t n, float* ret) const noexcept;

    /*******************************************
     * @brief 对向量内的元素进行求和
     * @param[in] v 要计算的向量
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool reduction(const float* v1, size_t n, float* ret) const noexcept;

    /*******************************************
     * @brief 计算两个坐标之间的距离
     * @param[in] v1 向量1
     * @param[in] v2 向量2
     * @param[in] n 向量长度
     * @param[out] ret 运算结果
     * @return 是否成功
     * ****************************************/
    bool distance(const float* v, float* v2, size_t n, float* ret) const noexcept;

private:
    static const std::vector<std::string> functions;
    static const char* source;

    bool m_enable;

    cl_platform_id m_pid;
    cl_device_id m_did;
    cl_context m_ctx;
    cl_command_queue m_cmd;
    cl_program m_program;

    std::string m_name;
    size_t m_maxLocalSize;


    std::map<std::string, cl_mem> m_buffers;

    std::map<std::string, cl_kernel> m_kernels;
    
};

}; // namespace AutoBug

#endif // AUTO_BUG_ACCELERATOR_H