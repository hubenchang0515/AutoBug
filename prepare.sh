#! /bin/bash

TEMPLATE="$(cat Accelerator.cxx)"
KERNEL="$(cat kernel.cl)"

echo "/********************************" > Accelerator.cpp
echo " * 这个文件基于 Accelerator.cxx 和" >> Accelerator.cpp
echo " * kernel.cl 自动生成,请不要直接修改" >> Accelerator.cpp
echo " * 这个文件 " >> Accelerator.cpp
echo " * *****************************/" >> Accelerator.cpp
echo "${TEMPLATE%'$AUTO_BUG_ACCELERATOR_OPEN_CL_CODE'*}" >> Accelerator.cpp
echo "${KERNEL}" >> Accelerator.cpp
echo "${TEMPLATE#*'$AUTO_BUG_ACCELERATOR_OPEN_CL_CODE'}" >> Accelerator.cpp