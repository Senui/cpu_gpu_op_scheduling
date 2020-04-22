#include "cpu_gpu_scheduling.h"

struct ExternalOp : public OperationImpl {
  virtual ~ExternalOp() {}
  void operator()(SimObject *so) override {
    std::cout << "CPU external op column wise" << std::endl;
  }

  void operator()() override {
    std::cout << "CPU external op row wise" << std::endl;
  }

  static bool registered_;
};

