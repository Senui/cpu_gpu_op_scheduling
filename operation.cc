#include "operation.h"
#include "operation_registry.h"

bool CellGrowthCpu::registered_ =
    OperationRegistry::GetInstance()->AddOperationImpl("CellGrowthOp", kCpu,
                                                       new CellGrowthCpu());

bool DisplacementOpCpu::registered_ =
    OperationRegistry::GetInstance()->AddOperationImpl("DisplacementOp", kCpu,
                                                       new DisplacementOpCpu());

bool DisplacementOpCuda::registered_ =
    OperationRegistry::GetInstance()->AddOperationImpl(
        "DisplacementOp", kCuda, new DisplacementOpCuda());

bool DisplacementOpOpenCL::registered_ =
    OperationRegistry::GetInstance()->AddOperationImpl(
        "DisplacementOp", kOpenCl, new DisplacementOpOpenCL());
