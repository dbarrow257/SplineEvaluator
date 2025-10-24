#include "InputHandler.h"

InputHandler::InputHandler(std::string InputFileName_) {
  InputFileName = InputFileName_;
}

InputHandler::~InputHandler() {
}

bool InputHandler::IsSplineFlat(std::vector<double> EvaluationPoints_) {
  bool IsFlat = true;
  for (size_t iEvalPoint=0;iEvalPoint<EvaluationPoints_.size();iEvalPoint++) {
    if (EvaluationPoints_[iEvalPoint] < 0.99999 || EvaluationPoints_[iEvalPoint] > 1.00001) {
      IsFlat = false;
      break;
    }
  }
  return IsFlat;
}
