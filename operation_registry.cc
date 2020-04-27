#include "operation_registry.h"

OperationRegistry::~OperationRegistry() {
  for (auto &pair : operations_) {
    delete pair.second;
  }
}

OperationRegistry *OperationRegistry::GetInstance() {
  static OperationRegistry operation_registry;
  return &operation_registry;
}

Operation *OperationRegistry::GetOperation(const std::string &op_name) {
  auto search = operations_.find(op_name);
  if (search == operations_.end()) {
    std::string msg = "Operation not found in registry: " + op_name;
    throw std::runtime_error(msg);
  }
  return search->second;
}

bool OperationRegistry::AddOperationImpl(const std::string &op_name,
                                         OpComputeTarget target,
                                         OperationImpl *impl) {
  // workaround for bug in unordered_map:
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61143
  if (operations_.size() == 0) {
    operations_.reserve(10);
  }
  auto *op = operations_[op_name];
  if (op == nullptr) {
    op = new Operation(op_name);
    operations_[op_name] = op;
  }
  op->AddOperationImpl(target, impl);
  return true;
}

OperationRegistry::OperationRegistry() {
  std::cout << "Create OperationRegistry" << std::endl;
}
