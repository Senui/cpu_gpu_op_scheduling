#ifndef OPERATION_H_
#define OPERATION_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SimObject {};

enum OpComputeTarget { kCpu, kCuda, kOpenCl };  // ... kOpenClFPGA ...

// Param::compute_target
static const int kComputeTarget = kCuda;

struct OperationImpl {
  virtual ~OperationImpl() {}
  virtual void operator()(SimObject *so) = 0;
  virtual void operator()() = 0;
  virtual bool IsGpuOperation() { return false; }
};

struct OperationImplGpu : public OperationImpl {
  virtual void InitializeGpuData() = 0;
  virtual void UpdateCpuData() = 0;

  bool IsGpuOperation() override { return true; }

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
  void operator()() {
    auto op_impl = implementations_[active_target_];
    if (op_impl->IsGpuOperation()) {
      static_cast<OperationImplGpu*>(op_impl)->InitializeGpuData();
    }
    (*op_impl)();
    if (op_impl->IsGpuOperation()) {
      static_cast<OperationImplGpu*>(op_impl)->UpdateCpuData();
    }
  }

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

struct DisplacementOpCuda : public OperationImplGpu {
  void InitializeGpuData() override {
    std::cout << "Initialize GPU buffers" << std::endl;
  }

  void UpdateCpuData() override {
    std::cout << "Update CPU data" << std::endl;
  }

  virtual ~DisplacementOpCuda() {}
  void operator()() override {
    std::cout << "GPU (CUDA) displacement op" << std::endl;
  }

  static bool registered_;
};

struct DisplacementOpOpenCL : public OperationImplGpu {
  void InitializeGpuData() override {
    std::cout << "Initialize GPU buffers" << std::endl;
  }

  void UpdateCpuData() override {
    std::cout << "Update CPU data" << std::endl;
  }

  virtual ~DisplacementOpOpenCL() {}
  void operator()() override {
    std::cout << "GPU (OpenCL) displacement op" << std::endl;
  }

  static bool registered_;
};

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

#endif  // OPERATION_H_
