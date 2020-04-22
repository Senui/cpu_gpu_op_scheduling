#include "external_op.h"
#include "operation_registry.h"

bool ExternalOp::registered_ = OperationRegistry::GetInstance()->AddOperationImpl(
    "ExternalOp", kCpu, new ExternalOp());
