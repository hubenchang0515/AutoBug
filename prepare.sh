#! /bin/bash

TEMPLATE="$(cat Accelerator.cxx)"
KERNEL="$(cat kernel.cl)"

echo "${TEMPLATE%'$AUTO_BUG_ACCELERATOR_OPEN_CL_CODE'*}" > Accelerator.cpp
echo "${KERNEL}" >> Accelerator.cpp
echo "${TEMPLATE#*'$AUTO_BUG_ACCELERATOR_OPEN_CL_CODE'}" >> Accelerator.cpp