#include "Accelerator.h"
#include <cmath>
#include <cstdarg>

namespace AutoBug
{

/*******************************************
 * @brief 获取一个全局公共实例
 * @return 对象实例
 * ****************************************/
Accelerator& Accelerator::instance() noexcept
{
    static Accelerator obj;
    return obj;
}

Accelerator::~Accelerator() noexcept
{
    clFinish(m_cmd);
    
    for (auto& kernel : m_kernels)
    {
        clReleaseKernel(kernel.second);
    }

    for (auto& buff : m_buffers)
    {
        clReleaseMemObject(buff.second);
    }

    clReleaseProgram(m_program);
    clReleaseCommandQueue(m_cmd);
    clReleaseContext(m_ctx);
    clReleaseDevice(m_did);
}

Accelerator::Accelerator() noexcept :
    m_enable(true),
    m_pid(nullptr),
    m_did(nullptr),
    m_ctx(nullptr),
    m_cmd(nullptr),
    m_program(nullptr),
    m_name(""),
    m_maxLocalSize(64)
{
    // 获取平台
    cl_int state = clGetPlatformIDs(1, &m_pid, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to get platform id\n");
        return;
    }

    // 获取设备
    state = clGetDeviceIDs(m_pid, CL_DEVICE_TYPE_GPU, 1, &m_did, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to get device id\n");
        return;
    }

    // 创建上下文
    m_ctx = clCreateContext(nullptr, 1, &m_did, nullptr, nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create context\n");
        return;
    }

    // 创建指令队列
    m_cmd = clCreateCommandQueueWithProperties(m_ctx, m_did, nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create command queue\n");
        return;
    }

    // 创建OpenCL程序
    size_t len = strlen(Accelerator::source);
    m_program = clCreateProgramWithSource(m_ctx, 1, &Accelerator::source, &len, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create program\n");
        return;
    }

    // 构建OpenCL程序
    state = clBuildProgram(m_program, 1, &m_did, nullptr, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        size_t len = 0;
        clGetProgramBuildInfo(m_program, m_did, CL_PROGRAM_BUILD_LOG, 0, nullptr, &len);
        char* msg = new char[len];
        clGetProgramBuildInfo(m_program, m_did, CL_PROGRAM_BUILD_LOG, len, msg, &len);
        fprintf(stderr, "failed to build program: %*s\n", static_cast<unsigned int>(len), msg);
        delete[] msg;
        return;
    }

    // 加载kernel函数
    for (auto& name : functions)
    {
        m_kernels[name] = clCreateKernel(m_program, name.c_str(), nullptr);
    }

    // 读取设备名称
    size_t n = 0;
    state = clGetDeviceInfo(m_did, CL_DEVICE_NAME, 0, nullptr, &n);
    if (state != CL_SUCCESS || n == 0)
    {
        m_name = "Unknown";
    }
    else
    {
        char* buffer = new char[n];
        clGetDeviceInfo(m_did, CL_DEVICE_NAME, n, buffer, nullptr);
        m_name = buffer;
        delete[] buffer;
    }

    // 读取设备一组任务的最大工作数量
    state = clGetDeviceInfo(m_did, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &m_maxLocalSize, nullptr);
    if (state != CL_SUCCESS)
    {
        m_maxLocalSize = 64;
    }
}

/*******************************************
 * @brief 开关加速器
 * @param[in] enable 是否启用
 * ****************************************/
void Accelerator::setEnable(bool enable) noexcept
{
    m_enable = enable;
}

/*******************************************
 * @brief 检查加速器是否可用
 * @return 加速器是否可用
 * ****************************************/
bool Accelerator::available() const noexcept
{
    for (auto& kernel : m_kernels)
    {
        if (kernel.second == nullptr)
        {
            printf("%s\n", kernel.first.c_str());
            return false;
        }
    }

    return  m_enable &&
            (m_pid != nullptr) &&
            (m_did != nullptr) &&
            (m_ctx != nullptr) &&
            (m_cmd != nullptr) &&
            (m_program != nullptr);
}

/*******************************************
 * @brief 获取加速器的名称(即显卡名称)
 * @return 名称
 * ****************************************/
std::string Accelerator::name() const noexcept
{
    return m_name;
}

/*******************************************
 * @brief 获取一组任务的工作数量
 * @return 一组任务的工作数量
 * ****************************************/
size_t Accelerator::maxLocalSize() const noexcept
{
    return m_maxLocalSize;
}

/*******************************************
 * @brief 计算一组任务的工作数量
 * @param[in] n 任务总数
 * @return 一组任务的工作数量
 * ****************************************/
size_t Accelerator::localSize(size_t n) const noexcept
{
    if (m_maxLocalSize <= n)
    {
        return m_maxLocalSize;
    }
    else
    {
        return n;
    }
}

/*******************************************
 * @brief 计算总工作数量
 * @param[in] n 任务总数
 * @return 总工作数量
 * ****************************************/
size_t Accelerator::globalSize(size_t n) const noexcept
{
    if (m_maxLocalSize <= n)
    {
        return m_maxLocalSize * ((n - 1 + m_maxLocalSize)/ m_maxLocalSize);
    }
    else
    {
        return n;
    }
}

/*******************************************
 * @brief 获取一个核函数
 * @param[in] name 核函数的名字
 * @return 核函数
 * ****************************************/
cl_kernel Accelerator::kernel(const std::string& name) const noexcept
{
    try
    {
        return m_kernels.at(name);
    }
    catch (std::out_of_range&)
    {
        return nullptr;
    }
}

/*******************************************
 * @brief 获取一个缓存
 * @param[in] name 缓存的名字
 * @return 缓存
 * ****************************************/
cl_mem Accelerator::buffer(const std::string& name) const noexcept
{
    try
    {
        return m_buffers.at(name);
    }
    catch (std::out_of_range&)
    {
        return nullptr;
    }
}

/*******************************************
 * @brief 创建一个缓存
 * @param[in] name 缓存的名字
 * @param[in] bytes 缓存的大小
 * @return 创建的缓存
 * ****************************************/
cl_mem Accelerator::createBuffer(const std::string& name, size_t bytes) noexcept
{
    int state;
    cl_mem buff = clCreateBuffer(m_ctx, CL_MEM_READ_WRITE, bytes, nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create buffer\n");
        return nullptr;
    }

    const auto& iter = m_buffers.find(name);
    if (iter != m_buffers.end())
    {
        clReleaseMemObject(iter->second);
    }
    m_buffers[name] = buff;
    return buff;
}

/*******************************************
 * @brief 写一个缓存
 * @param[in] name 缓存名字
 * @param[in] offset 缓存内位置
 * @param[in] ptr 数据
 * @param[in] byets 数据大小
 * @param[in] block 是否阻塞
 * @return 是否成功
 * ****************************************/
bool Accelerator::writeBuffer(const std::string& name, size_t offset, void* ptr, size_t bytes, bool block) noexcept
{
    try
    {
        int state = clEnqueueWriteBuffer(m_cmd, m_buffers.at(name), block, offset, bytes, ptr, 0, nullptr, nullptr);
        if (state != CL_SUCCESS)
        {
            fprintf(stderr, "failed to write buffer\n");
            return false;
        }
    }
    catch (std::out_of_range&)
    {
        return false;
    }

    return true;
}

/*******************************************
 * @brief 读一个缓存
 * @param[in] name 缓存名字
 * @param[in] offset 缓存内位置
 * @param[in] ptr 数据
 * @param[in] byets 数据大小
 * @param[in] block 是否阻塞
 * @return 是否成功
 * ****************************************/
bool Accelerator::readBuffer(const std::string& name, size_t offset, void* ptr, size_t bytes, bool block) const noexcept
{
    try
    {
        int state = clEnqueueReadBuffer(m_cmd, m_buffers.at(name), block, offset, bytes, ptr, 0, nullptr, nullptr);
        if (state != CL_SUCCESS)
        {
            fprintf(stderr, "failed to write buffer\n");
            return false;
        }
    }
    catch (std::out_of_range&)
    {
        return false;
    }

    return true;
}

/*******************************************
 * @brief 给核函数设置参数
 * @param[in] kernel 核函数的名字
 * @param[in] i 参数序号,从0开始
 * @param[in] arg 参数
 * @param[in] size 参数的大小
 * @return 是否成功
 * ****************************************/
bool Accelerator::setArg(cl_kernel kernel, int i, void* arg, size_t size) noexcept
{
    int state = clSetKernelArg(kernel, i, size, arg);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to set arg\n");
        return false;
    }
    return true;
}

/*******************************************
 * @brief 调用一个核函数
 * @param[in] kernel 运算核函数
 * @param[in] localSize 一组工作项的数量
 * @param[in] globalSize 总工作项的数量
 * @return 是否成功
 * ****************************************/
bool Accelerator::invoke(cl_kernel kernel, size_t localSize, size_t globalSize) const noexcept
{
    int state = clEnqueueNDRangeKernel(m_cmd, kernel, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to invoke kernel\n");
        return false;
    }
    return true;
}

/*******************************************
 * @brief 调用一个核函数
 * @param[in] kernel 运算核函数
 * @param[in] localSize 一组工作项的数量
 * @param[in] globalSize 总工作项的数量
 * @param[in] argc 参数个数
 * @param[in] ... 传给核函数的参数
 * @return 是否成功
 * ****************************************/
bool Accelerator::invoke(cl_kernel kernel, size_t localSize, size_t globalSize, size_t argc, ...) const noexcept
{
    bool success = true;                    // 返回值
    int state;

    va_list args;
    va_start(args, argc);

    // 设置调用参数
    for (size_t i = 0; i < argc; i++)
    {
        cl_mem arg = va_arg(args, cl_mem);
        state = clSetKernelArg(kernel, i, sizeof(cl_mem), (void*)&arg);
        if (state != CL_SUCCESS)
        {
            fprintf(stderr, "failed to set arg\n");
            success = false;
            goto EXIT;
        }
    }

    // 调用核函数
    state = clEnqueueNDRangeKernel(m_cmd, kernel, 1, nullptr, &globalSize, &localSize, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to invoke kernel\n");
        success = false;
        goto EXIT;
    }

EXIT:
    va_end(args);
    return success;
}

/*******************************************
 * @brief 对向量进行一次标量运算
 * @param[in] v1 进行运算的向量1
 * @param[in] v2 进行运算的向量2
 * @param[in] n 向量长度
 * @param[in] kernel 运算核函数
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::scalar(const float* v1, const float* v2, size_t n, cl_kernel kernel, float* ret) const noexcept
{
    size_t localSize = this->localSize(n);
    size_t globalSize = this->globalSize(n);
    bool success = true;
    int state;

    // 参数
    cl_mem arg1 = nullptr;
    cl_mem arg2 = nullptr;
    cl_mem arg3 = nullptr;
    
    arg1 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    arg2 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    arg3 = clCreateBuffer(m_ctx, CL_MEM_READ_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    state = clEnqueueWriteBuffer(m_cmd, arg1, CL_TRUE, 0, n * sizeof(float), v1, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write arg\n");
        success = false;
        goto EXIT;
    }

    state = clEnqueueWriteBuffer(m_cmd, arg2, CL_TRUE, 0, n * sizeof(float), v2, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write arg\n");
        success = false;
        goto EXIT;
    }

    success = invoke(kernel, localSize, globalSize, 3, arg1, arg2, arg3);

    state = clEnqueueReadBuffer(m_cmd, arg3, CL_TRUE, 0, n * sizeof(float), ret, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to read arg\n");
        success = false;
        goto EXIT;
    }

EXIT:
    if (arg1 != nullptr)
        clReleaseMemObject(arg1);

    if (arg2 != nullptr)
        clReleaseMemObject(arg2);

    if (arg3 != nullptr)
        clReleaseMemObject(arg3);

    return success;
}

/*******************************************
 * @brief 对向量进行一次标量加法运算
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::add(const float* v1, const float* v2, size_t n, float* ret) const noexcept
{
    try
    {
        return scalar(v1, v2, n, m_kernels.at("add"), ret);
    }
    catch (std::out_of_range&)
    {
        return false;
    }
}

/*******************************************
 * @brief 对向量进行一次标量减法运算
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::sub(const float* v1, const float* v2, size_t n, float* ret) const noexcept
{
    try
    {
        return scalar(v1, v2, n, m_kernels.at("sub"), ret);
    }
    catch (std::out_of_range&)
    {
        return false;
    }
}

/*******************************************
 * @brief 对向量进行一次标量乘法运算
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::mul(const float* v1, const float* v2, size_t n, float* ret) const noexcept
{
    try
    {
        return scalar(v1, v2, n, m_kernels.at("mul"), ret);
    }
    catch (std::out_of_range&)
    {
        return false;
    }
}

/*******************************************
 * @brief 对向量进行一次标量除法运算
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::div(const float* v1, const float* v2, size_t n, float* ret) const noexcept
{
    try
    {
        return scalar(v1, v2, n, m_kernels.at("div"), ret);
    }
    catch (std::out_of_range&)
    {
        return false;
    }
}

/*******************************************
 * @brief 对向量内的元素进行求和
 * @param[in] v 要计算的向量
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::reduction(const float* v, size_t n, float* ret) const noexcept
{
    size_t localSize = this->localSize(n);
    size_t globalSize = this->globalSize(n);
    bool success = true;
    int state;

    // 参数
    cl_mem arg1 = nullptr;
    cl_mem arg2 = nullptr;
    float* temp = new float[n];

    arg1 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    arg2 = clCreateBuffer(m_ctx, CL_MEM_READ_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    state = clEnqueueWriteBuffer(m_cmd, arg1, CL_TRUE, 0, n * sizeof(float), v, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write arg\n");
        success = false;
        goto EXIT;
    }

    try
    {
        success = invoke(m_kernels.at("reduction"), localSize, globalSize, 2, arg1, arg2);
    }
    catch (std::out_of_range&)
    {
        success = false;
        goto EXIT;
    }
    

    state = clEnqueueReadBuffer(m_cmd, arg2, CL_TRUE, 0, n * sizeof(float), temp, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to read arg\n");
        success = false;
        goto EXIT;
    }

    *ret = 0.0f;
    for (size_t i = 0; i < n; i += localSize)
    {
        *ret += temp[i];
    }

EXIT:
    if (arg1 != nullptr)
        clReleaseMemObject(arg1);

    if (arg2 != nullptr)
        clReleaseMemObject(arg2);

    if (temp != nullptr)
        delete[] temp;

    return success;
}

/*******************************************
 * @brief 计算两个坐标元素的差的平方
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::distance(const float* v1, float* v2, size_t n, float* ret) const noexcept
{
    size_t localSize = this->localSize(n);
    size_t globalSize = this->globalSize(n);
    bool success = true;
    int state;

    // 参数
    cl_mem arg1 = nullptr;
    cl_mem arg2 = nullptr;
    cl_mem arg3 = nullptr;
    
    arg1 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    arg2 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    arg3 = clCreateBuffer(m_ctx, CL_MEM_READ_ONLY, globalSize * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to create arg\n");
        success = false;
        goto EXIT;
    }

    state = clEnqueueWriteBuffer(m_cmd, arg1, CL_FALSE, 0, n * sizeof(float), v1, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write arg\n");
        success = false;
        goto EXIT;
    }

    state = clEnqueueWriteBuffer(m_cmd, arg2, CL_FALSE, 0, n * sizeof(float), v2, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write arg\n");
        success = false;
        goto EXIT;
    }

    try
    {
        success = invoke(m_kernels.at("distanceStep1"), localSize, globalSize, 3, arg1, arg2, arg3);
    }
    catch (std::out_of_range&)
    {
        success = false;
        goto EXIT;
    }    

    state = clEnqueueReadBuffer(m_cmd, arg3, CL_TRUE, 0, n * sizeof(float), ret, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to read arg\n");
        success = false;
        goto EXIT;
    }

    for (size_t i = localSize; i < n; i += localSize)
    {
        ret[0] += ret[i];
    }

    ret[0] = std::sqrt(ret[0]);

EXIT:
    if (arg1 != nullptr)
        clReleaseMemObject(arg1);

    if (arg2 != nullptr)
        clReleaseMemObject(arg2);

    if (arg3 != nullptr)
        clReleaseMemObject(arg3);

    return success;
}

/* 函数列表 */
const std::vector<std::string> Accelerator::functions = {
    "add", "sub", "div", "mul", "reduction",
    "distanceStep1", "findNearest", "updatePoints"
};

/* OpenCL源码 */
const char* Accelerator::source = R"AutoBug($AUTO_BUG_ACCELERATOR_OPEN_CL_CODE)AutoBug";

}; // namespace AutoBug