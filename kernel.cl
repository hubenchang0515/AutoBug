/*******************************************
 * @brief 加法计算
 * @param[in] input 输入向量
 * @param[in] output 输出元素的和
 * ****************************************/
__kernel void add(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] + y[idx];
}

/*******************************************
 * @brief 减法计算
 * @param[in] input 输入向量
 * @param[in] output 输出元素的和
 * ****************************************/
__kernel void sub(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] - y[idx];
}

/*******************************************
 * @brief 乘法计算
 * @param[in] input 输入向量
 * @param[in] output 输出元素的和
 * ****************************************/
__kernel void mul(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] * y[idx];
}


/*******************************************
 * @brief 除法计算
 * @param[in] input 输入向量
 * @param[in] output 输出元素的和
 * ****************************************/
__kernel void div(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] / y[idx];
}


/*******************************************
 * @brief 计算向量元素之和
 * @param[in] input 输入向量
 * @param[out] output 输出元素的和
 * ****************************************/
__kernel void groupSum(__global float* input, __global float* output)
{
    size_t globalId = get_global_id(0);   	// 全局id
    size_t localId = get_local_id(0);     	// 组内id
    size_t localSize = get_local_size(0); 	// 一组工作的数量(即线程数)

    output[globalId] = input[globalId];

    // 各个元素同步归并相加
    int i = 0;
    for (size_t count = localSize; count > 1; count = (count + 1) / 2)
    {
        barrier(CLK_LOCAL_MEM_FENCE); // 组内同步
        size_t step = (count + 1) / 2;
        if (localId < step && localId + step < count)
        {
            output[globalId] += output[globalId + step];
        }
    }
}