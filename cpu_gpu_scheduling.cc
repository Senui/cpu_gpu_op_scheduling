#include <iostream>
#include <map>
#include <string>
#include <vector>

// Flag to enable GPU operations
static const bool kGpu = true;

class SimObject {};

// Base class for an BioDynaMo operation
struct Operation {
  Operation(const std::string &name) : name_(name) {}

  virtual Operation *GetCudaOperation() { return nullptr; }

  virtual ~Operation() {}

  size_t freq_ = 1;
  std::string name_;
};

// We either have this intermediate template class (because in the Scheduler we
// need to be able to store Operation* pointers in a single container). Or we
// make the Operation class templated, and store different template versions
// into a Variant container and check each entry with a Visitor
template <typename TReturn, typename... TParameters>
struct VariadicOperation : public Operation {
  VariadicOperation(const std::string &name) : Operation(name) {}
  virtual TReturn operator()(TParameters... parameters) = 0;
};

// Derived class for operations that affect all simulation objects
struct SimObjectOperation : VariadicOperation<void, SimObject *> {
  SimObjectOperation(const std::string &name) : VariadicOperation(name) {}
};

// Derived class for operations that must be executed on GPU
struct GpuOperation : VariadicOperation<void> {
  GpuOperation(const std::string &name) : VariadicOperation(name) {}
};

// Example of GpuOperation
struct DisplacementOpCuda : public GpuOperation {
  DisplacementOpCuda() : GpuOperation("displacement") {}

  // Cuda method
  void operator()() override {}
};

// Example of SimObjecOperation that is able to run on GPU
struct DisplacementOp : public SimObjectOperation {
  DisplacementOp() : SimObjectOperation("displacement") {}

  // We have a CUDA implementation of this operation, so we override this
  Operation *GetCudaOperation() override { return new DisplacementOpCuda(); }

  // CPU method
  void operator()(SimObject *) override {}
};

// Example of SimObjecOperation that is unable to be run on GPU
struct CellGrowth : public SimObjectOperation {
  CellGrowth() : SimObjectOperation("cell growth") {}

  // CPU method
  void operator()(SimObject *) override {}
};

// Scheduling different type of operations
class Scheduler {
public:
  Scheduler() {
    operations_.push_back(new CellGrowth());
    operations_.push_back(new DisplacementOp());
    ScheduleOps();
  }

  // Schedule the operations
  void ScheduleOps() {
    for (auto &op : operations_) {
      if (kGpu) {
        // TODO: keep track of this allocation; needs to be deleted in addition
        // to the operations in `operations_`
        if (auto cuda_op = op->GetCudaOperation()) {
          cuda_op->name_ += " (CUDA)";
          scheduled_ops_.push_back(cuda_op);
        } else {
          scheduled_ops_.push_back(op);
        }
      } else {
        scheduled_ops_.push_back(op);
      }
    }
  }

  void RunScheduledOps() {
    // In the future we could implement diferent scheduling policies here (e.g.
    // like interleaving CPU and GPU runtime)
    for (auto &op : scheduled_ops_) {
      if (timestep % op->freq_ == 0) {
        if (auto downcasted = dynamic_cast<SimObjectOperation *>(op)) {
          std::cout << "Running " << op->name_ << " for all cells..."
                    << std::endl;
        } else if (auto downcasted = dynamic_cast<GpuOperation *>(op)) {
          std::cout << "Running " << op->name_ << " as stand-alone operation..."
                    << std::endl;
        }
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
  size_t T = 1; // number of timesteps
  for (size_t t = 0; t < T; ++t) {
    s.RunScheduledOps();
  }
}
