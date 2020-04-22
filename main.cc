#include "operation.h"
#include "operation_registry.h"

// Scheduling different type of operations
class Scheduler {
 public:
  Scheduler() {}

  ~Scheduler() {
    for (auto *op : operations_) {
      delete op;
    }
  }

  void AddOperation(Operation *op) { operations_.push_back(op); }

  // Schedule the operations
  void ScheduleOps() {
    for (auto *op : operations_) {
      if (kComputeTarget == kCuda && op->IsComputeTargetSupported(kCuda)) {
        op->SelectComputeTarget(kCuda);
      } else if (kComputeTarget == kOpenCl &&
                 op->IsComputeTargetSupported(kOpenCl)) {
        op->SelectComputeTarget(kOpenCl);
      } else {
        op->SelectComputeTarget(kCpu);
      }
      scheduled_ops_.push_back(op);
    }
  }

  void RunScheduledOps() {
    for (auto *op : scheduled_ops_) {
      if (timestep % op->freq_ == 0) {
        (*op)();
      }
    }
    timestep++;
  }

 private:
  size_t timestep = 0;
  std::vector<Operation *> scheduled_ops_;
  std::vector<Operation *> operations_;
};

int main() {
  Scheduler s;
  auto *op_registry = OperationRegistry::GetInstance();
  s.AddOperation(op_registry->GetOperation("DisplacementOp"));
  s.AddOperation(op_registry->GetOperation("CellGrowthOp"));
  s.AddOperation(op_registry->GetOperation("ExternalOp"));
  std::cout << "Heeee" << std::endl;
  s.ScheduleOps();
  size_t T = 1;  // number of timesteps
  for (size_t t = 0; t < T; ++t) {
    s.RunScheduledOps();
  }
}
