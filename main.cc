#include "cpu_gpu_scheduling.h"

int main() {
  Scheduler s;
  s.AddOperation(OperationRegistry::GetOperation("DisplacementOp"));
  s.AddOperation(OperationRegistry::GetOperation("CellGrowthOp"));
  s.AddOperation(OperationRegistry::GetOperation("ExternalOp"));
  std::cout << "Heeee" << std::endl;
  s.ScheduleOps();
  size_t T = 1; // number of timesteps
  for (size_t t = 0; t < T; ++t) {
    s.RunScheduledOps();
  }
}