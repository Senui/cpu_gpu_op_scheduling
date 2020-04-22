#include "cpu_gpu_scheduling.h"

std::unordered_map<std::string, Operation *> OperationRegistry::operations_;

bool DisplacementOpCpu::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kCpu, new DisplacementOpCpu());

bool DisplacementOpCuda::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kCuda, new DisplacementOpCuda());

bool DisplacementOpOpenCL::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kOpenCl, new DisplacementOpOpenCL());

