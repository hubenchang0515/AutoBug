__kernel void add(__global float* x, __global float* y, __global float* ret)
{
	/* 获取当前线程的全局id */
	const int idx = get_global_id(0);

	ret[idx] = x[idx] + y[idx];
}

__kernel void sub(__global float* x, __global float* y, __global float* ret)
{
	/* 获取当前线程的全局id */
	const int idx = get_global_id(0);

	ret[idx] = x[idx] - y[idx];
}

__kernel void mul(__global float* x, __global float* y, __global float* ret)
{
	/* 获取当前线程的全局id */
	const int idx = get_global_id(0);

	ret[idx] = x[idx] * y[idx];
}

__kernel void div(__global float* x, __global float* y, __global float* ret)
{
	/* 获取当前线程的全局id */
	const int idx = get_global_id(0);

	ret[idx] = x[idx] / y[idx];
}