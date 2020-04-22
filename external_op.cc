#include "external_op.h"

bool ExternalOp::registered_ = OperationRegistry::AddOperationImpl(
    "ExternalOp", kCpu, new ExternalOp());
