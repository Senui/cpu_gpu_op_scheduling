#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SimObject {};

enum OpComputeTarget { kCpu, kCuda, kOpenCl }; // ... kOpenClFPGA ...

// Param::compute_target
static const int kComputeTarget = kCuda;

struct OperationImpl {
  virtual ~OperationImpl() {}
  virtual void operator()(SimObject *so) = 0;
  virtual void operator()() = 0;
};

struct OperationImplGpu : public OperationImpl {
  virtual ~OperationImplGpu() {}
  // sub class to avoid overriding this operator
  void operator()(SimObject *so) override {
    // Log::Fatal
    throw "GPU operations do not support this function operator";
  }
};

// BioDynaMo operation
struct Operation {
  Operation(const std::string &name) : name_(name) {}
  ~Operation() {}

  void operator()(SimObject *so) { (*implementations_[active_target_])(so); }
  void operator()() { (*implementations_[active_target_])(); }

  void AddOperationImpl(OpComputeTarget target, OperationImpl *impl) {
    if (implementations_.size() <= target) {
      implementations_.resize(target + 1);
    }
    implementations_[target] = impl;
  }

  bool IsComputeTargetSupported(OpComputeTarget target) {
    if (implementations_.size() <= target) {
      return false;
    }
    return implementations_[target] != nullptr;
  }

  void SelectComputeTarget(OpComputeTarget target) {
    if (!IsComputeTargetSupported(target)) {
      throw "Compute target not supported";
    }
    active_target_ = target;
  }

  size_t freq_ = 1;
  std::string name_;
  OpComputeTarget active_target_;
  std::vector<OperationImpl *> implementations_;
};

struct OperationRegistry {
  static Operation *GetOperation(const std::string &op_name) {
    return operations_[op_name];
  }

  static bool AddOperationImpl(const std::string &op_name,
                               OpComputeTarget target, OperationImpl *impl) {
    auto *op = operations_[op_name];
    if (op == nullptr) {
      op = new Operation(op_name);
      operations_[op_name] = op;
    }
    op->AddOperationImpl(target, impl);
    return true;
  }

private:
  /// think about std::string as key - easy to make a typo
  /// FIXME memory leak
  static std::unordered_map<std::string, Operation *> operations_;
};

std::unordered_map<std::string, Operation *> OperationRegistry::operations_;

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

// --------------------------------------------------------------------------
// Example Operation Displacement

struct DisplacementOpCpu : public OperationImpl {
  virtual ~DisplacementOpCpu() {}
  void operator()(SimObject *so) override {
    std::cout << "CPU displacement op column wise" << std::endl;
  }

  void operator()() override {
    std::cout << "CPU displacement op row wise" << std::endl;
    // rm->ApplyOnAllElementsParallelDynamic(chunk, [this](SimObject* so){
    // (*this)(so); })
  }

  static bool registered_;
};

bool DisplacementOpCpu::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kCpu, new DisplacementOpCpu());

struct DisplacementOpCuda : public OperationImplGpu {
  virtual ~DisplacementOpCuda() {}
  void operator()() override {
    std::cout << "GPU (CUDA) displacement op" << std::endl;
  }

  static bool registered_;
};

bool DisplacementOpCuda::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kCuda, new DisplacementOpCuda());

struct DisplacementOpOpenCL : public OperationImplGpu {
  virtual ~DisplacementOpOpenCL() {}
  void operator()() override {
    std::cout << "GPU (OpenCL) displacement op" << std::endl;
  }

  static bool registered_;
};

bool DisplacementOpOpenCL::registered_ = OperationRegistry::AddOperationImpl(
    "DisplacementOp", kOpenCl, new DisplacementOpOpenCL());

// --------------------------------------------------------------------------
// Example Operation Cell growth

struct CellGrowthCpu : public OperationImpl {
  virtual ~CellGrowthCpu() {}
  void operator()(SimObject *so) override {
    std::cout << "CPU cell growth column wise" << std::endl;
  }

  void operator()() override {
    std::cout << "CPU cell growth row wise" << std::endl;
    // rm->ApplyOnAllElementsParallelDynamic(chunk, [this](SimObject* so){
    // (*this)(so); })
  }

  static bool registered_;
};

bool CellGrowthCpu::registered_ = OperationRegistry::AddOperationImpl(
    "CellGrowthOp", kCpu, new CellGrowthCpu());

int main() {
  Scheduler s;
  s.AddOperation(OperationRegistry::GetOperation("DisplacementOp"));
  s.AddOperation(OperationRegistry::GetOperation("CellGrowthOp"));
  s.ScheduleOps();
  size_t T = 1; // number of timesteps
  for (size_t t = 0; t < T; ++t) {
    s.RunScheduledOps();
  }
}
