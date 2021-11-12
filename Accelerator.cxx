#include "Accelerator.h"

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
    m_add(nullptr),
    m_sub(nullptr),
    m_mul(nullptr),
    m_div(nullptr),
    m_localWorkSize(64)
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
    m_add = clCreateKernel(m_program, "add", nullptr);
    m_sub = clCreateKernel(m_program, "sub", nullptr);
    m_mul = clCreateKernel(m_program, "mul", nullptr);
    m_div = clCreateKernel(m_program, "div", nullptr);

    // 读取设备一组任务的最大工作数量
    state = clGetDeviceInfo(m_did, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &m_localWorkSize, nullptr);
    if (state != CL_SUCCESS)
    {
        m_localWorkSize = 64;
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
    return  m_enable &&
            (m_pid != nullptr) &&
            (m_did != nullptr) &&
            (m_ctx != nullptr) &&
            (m_cmd != nullptr) &&
            (m_program != nullptr) &&
            (m_add != nullptr) &&
            (m_sub != nullptr) &&
            (m_mul != nullptr) &&
            (m_div != nullptr);
}

/*******************************************
 * @brief 获取设备名称
 * @return 设备名
 * ****************************************/
std::string Accelerator::deviceName() const noexcept
{
    static std::string name;
    if (name.empty())
    {
        size_t n = 0;
        clGetDeviceInfo(m_did, CL_DEVICE_NAME, 0, nullptr, &n);
        char* buffer = new char[n];
        clGetDeviceInfo(m_did, CL_DEVICE_NAME, n, buffer, nullptr);
        name = buffer;
        delete[] buffer;
    }
    return name;
}

/*******************************************
 * @brief 获取一组任务的工作数量
 * @return 一组任务的工作数量
 * ****************************************/
size_t Accelerator::workSize() const noexcept
{
    return m_localWorkSize;
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
    bool success = true;                    // 返回值
    cl_mem m1 = nullptr;                    // 显存
    cl_mem m2 = nullptr;
    cl_mem m3 = nullptr;
    int state;

    // 一组任务的数量
    size_t localSize = m_localWorkSize <= n ? m_localWorkSize : n;

    // 分配显存
    m1 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, n * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS || m1 == nullptr)
    {
        fprintf(stderr, "failed to create buffer\n");
        success = false;
        goto EXIT;
    }

    m2 = clCreateBuffer(m_ctx, CL_MEM_WRITE_ONLY, n * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS || m2 == nullptr)
    {
        fprintf(stderr, "failed to create buffer\n");
        success = false;
        goto EXIT;
    }

    m3 = clCreateBuffer(m_ctx, CL_MEM_READ_ONLY, n * sizeof(float), nullptr, &state);
    if (state != CL_SUCCESS || m3 == nullptr)
    {
        fprintf(stderr, "failed to create buffer\n");
        success = false;
        goto EXIT;
    }

    // 将内存上的数据写到显存上
    state = clEnqueueWriteBuffer(m_cmd, m1, CL_TRUE, 0, n * sizeof(cl_float), v1, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write buffer\n");
        success = false;
        goto EXIT;
    }
    
    state = clEnqueueWriteBuffer(m_cmd, m2, CL_TRUE, 0, n * sizeof(cl_float), v2, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to write buffer\n");
        success = false;
        goto EXIT;
    }

    // 设置调用参数
    state = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&m1);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to set arg\n");
        success = false;
        goto EXIT;
    }

    state = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&m2);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to set arg\n");
        success = false;
        goto EXIT;
    }

    state = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&m3);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to set arg\n");
        success = false;
        goto EXIT;
    }

    // 调用核函数
    state = clEnqueueNDRangeKernel(m_cmd, kernel, 1, nullptr, &n, &localSize, 0, nullptr, nullptr);
    if (state != CL_SUCCESS || m2 == nullptr)
    {
        fprintf(stderr, "failed to invoke kernel\n");
        success = false;
        goto EXIT;
    }

    // 读取运算结果
    state = clEnqueueReadBuffer(m_cmd, m3, CL_TRUE, 0, n * sizeof(cl_float), ret, 0, nullptr, nullptr);
    if (state != CL_SUCCESS)
    {
        fprintf(stderr, "failed to read buffer\n");
        success = false;
        goto EXIT;
    }

EXIT:
    if (m1 != nullptr)
        clReleaseMemObject(m1);

    if (m2 != nullptr)
        clReleaseMemObject(m2);

    if (m3 != nullptr)
        clReleaseMemObject(m3);

    return success;
}

/*******************************************
 * @brief 对向量进行一次标量加法运算,结果保存在v1
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::add(const float* v1, float* v2, size_t n, float* ret) const noexcept
{
    return scalar(v1, v2, n, m_add, ret);
}

/*******************************************
 * @brief 对向量进行一次标量减法运算,结果保存在v1
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::sub(const float* v1, float* v2, size_t n, float* ret) const noexcept
{
    return scalar(v1, v2, n, m_sub, ret);
}

/*******************************************
 * @brief 对向量进行一次标量乘法运算,结果保存在v1
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::mul(const float* v1, float* v2, size_t n, float* ret) const noexcept
{
    return scalar(v1, v2, n, m_mul, ret);
}

/*******************************************
 * @brief 对向量进行一次标量除法运算,结果保存在v1
 * @param[in] v1 向量1
 * @param[in] v2 向量2
 * @param[in] n 向量长度
 * @param[out] ret 运算结果
 * @return 是否成功
 * ****************************************/
bool Accelerator::div(const float* v1, float* v2, size_t n, float* ret) const noexcept
{
    return scalar(v1, v2, n, m_div, ret);
}

/* OpenCL源码 */
const char* Accelerator::source = R"AutoBug($AUTO_BUG_ACCELERATOR_OPEN_CL_CODE)AutoBug";

}; // namespace AutoBug