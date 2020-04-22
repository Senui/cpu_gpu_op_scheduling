#ifndef OPERATION_REGISTRY_H_
#define OPERATION_REGISTRY_H_

#include "operation.h"

struct OperationRegistry {
  ~OperationRegistry();

  static OperationRegistry* GetInstance();

  Operation *GetOperation(const std::string &op_name);

  bool AddOperationImpl(const std::string &op_name,
                               OpComputeTarget target, OperationImpl *impl);

private:
  /// think about std::string as key - easy to make a typo
  std::unordered_map<std::string, Operation *> operations_;

  OperationRegistry();
};

#endif  // OPERATION_REGISTRY_H_

