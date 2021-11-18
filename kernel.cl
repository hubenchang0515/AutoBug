/*******************************************
 * @brief 加法计算
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] ret 输出元素的和
 * ****************************************/
__kernel void add(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] + y[idx];
}

/*******************************************
 * @brief 减法计算
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] ret 输出元素的差
 * ****************************************/
__kernel void sub(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] - y[idx];
}

/*******************************************
 * @brief 乘法计算
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] ret 输出元素的积
 * ****************************************/
__kernel void mul(__global float* x, __global float* y, __global float* ret)
{
    /* 获取当前线程的全局id */
    size_t idx = get_global_id(0);

    ret[idx] = x[idx] * y[idx];
}


/*******************************************
 * @brief 除法计算
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] ret 输出元素的商
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
__kernel void reduction(__global float* input, __global float* output)
{
    size_t globalId = get_global_id(0);   	// 全局id
    size_t localId = get_local_id(0);     	// 组内id
    size_t localSize = get_local_size(0); 	// 一组工作的数量(即线程数)

    output[globalId] = input[globalId];

    // 各个元素同步归并相加
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


/*******************************************
 * @brief 计算两个向量之间的欧氏距离
          步骤1:
          计算两个向量对应元素差的平方,并在各个分组
          内求和,结果放在各个分组的开头
          示例: [0] + [1] + [2] + [3] 
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] ret 输出计算结果
 * ****************************************/
__kernel void distanceStep1(__global float* x, __global float* y, __global float* ret)
{
    size_t globalId = get_global_id(0);   	// 全局id
    size_t localId = get_local_id(0);     	// 组内id
    size_t localSize = get_local_size(0); 	// 一组工作的数量(即线程数)

    ret[globalId] = pow(x[globalId] - y[globalId], 2);

    // 各个元素同步归并相加
    for (size_t count = localSize; count > 1; count = (count + 1) / 2)
    {
        barrier(CLK_LOCAL_MEM_FENCE); // 组内同步
        size_t step = (count + 1) / 2;
        if (localId < step && localId + step < count)
        {
            ret[globalId] += ret[globalId + step];
        }
    }
}

/*******************************************
 * @brief 计算两个向量之间的欧氏距离
 * @param[in] x 输入向量
 * @param[in] y 输入向量
 * @param[in] dims 维度
 * @return 两个向量的距离
 * ****************************************/
float getDistance(__global float* x, __global float* y, int dims)
{
    float sum = 0.0f;
    for (int i = 0; i <dims; i++)
    {
        sum += pow(x[i] - y[i], 2);
    }
    return sqrt(sum);
}

/*******************************************
 * @brief 寻找距离最近的分组,每个样本一个线程
 * @param[in] items 数据样本
 * @param[in] points 分组中心
 * @param[out] assignment 输出分组索引
 * @param[in] dims 维度
 * @param[in] k 分组数量
 * @param[in] n 样本数量
 * ****************************************/
__kernel void findNearest(__global float* items,
                          __global float* points, 
                          __global int* assignment, 
                          int dims,
                          int k,
                          int n)
{
    const size_t idx = get_global_id(0);

    // 对齐线程,直接返回
    if (idx >= n)
        return;

    // 找到线程对应的样本
    __global float* item = items + idx * dims;

    // 查找最近的分组中心
    int p = 0;
    float nearest = getDistance(item, points, dims);
    for (int i = 1; i < k; i++)
    {
        float n = getDistance(item, points + i * dims, dims);
        if (n < nearest)
        {
            p = i;
            nearest = n;
        }
    }
    assignment[idx] = p;
}

/*******************************************
 * @brief 更新分组中心,每个分组一个线程
 * @param[in] items 数据样本
 * @param[out] points 分组中心
 * @param[in] assignment 分组索引
 * @param[in] dims 维度
 * @param[in] k 分组数量
 * @param[in] n 样本数量
 * ****************************************/
__kernel void updatePoints(__global float* items,
                           __global float* points, 
                           __global int* assignment, 
                           int dims,
                           int k,
                           int n)
{
    const size_t idx = get_global_id(0);

    // 对齐线程,直接返回
    if (idx >= k)
        return;

    // 找到线程对应的分组中心点
    __global float* point = points + idx * dims;

    //清零
    for (int i = 0; i < dims; i++)
    {
        point[i] = 0.0f;
    }

    // 计算组内样本的坐标和
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (assignment[i] != idx)
            continue;

        __global float* item = items + i * dims;
        for (int j = 0; j < dims; j++)
        {
            point[j] += item[j];
        }
        count += 1;
    }

    // 除以样本数量获得平均坐标
    for (int i = 0; i < dims; i++)
    {
        point[i] /= count;
    }
}