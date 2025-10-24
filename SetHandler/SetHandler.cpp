#include "SetHandler.h"

#include <iostream>

#include "SplineSet/SplineSet_Eigen.h"
#include "InputHandler/InputHandler_SingleRootDir.h"

SetHandler::SetHandler(YAML::Node Config_) {
  InputFileName = "AtmosphericFluxShapeSplines.root";
  Inputs = new InputHandler_SingleRootDir(InputFileName);
  
  SystNames = Inputs->ReturnSystNames();

  SplineSets.resize(SystNames.size());
  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    SplineSets[iSyst] = new SplineSet_Eigen(YAML::Node());
    SplineSets[iSyst]->SetSystName(SystNames[iSyst]);
    SplineSets[iSyst]->SetInputHandler(Inputs);
    SplineSets[iSyst]->DigestSplines(InputFileName);
  }
}

SetHandler::~SetHandler() {
}

void SetHandler::EvaluateSplines(std::vector<double> SystDialValues_) {
  if (SystDialValues_.size() != SystNames.size()) {
    std::cerr << "Number of dial values passed does not equal that expected by the number of syst names found from the spline names" << std::endl;
    throw;
  }

  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    SplineSets[iSyst]->Evaluate(SystDialValues_[iSyst]);
  }
}

int SetHandler::ReturnSplineSetIndex(std::string SystName_) {
  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    if (SystNames[iSyst] == SystName_) {
      return iSyst;
    }
  }
  return -1;
}

std::vector<double> SetHandler::ReturnSplineWeights(std::vector<std::string> SystNames_, std::vector<std::string> ParNames_, std::vector<double> ParVals_) {
  std::vector<double> ReturnVector;
  std::vector<double*> WeightPointers = ReturnSplineWeightPointers(SystNames_, ParNames_,ParVals_);
  for (size_t iPtr=0;iPtr<WeightPointers.size();iPtr++) {
    ReturnVector[iPtr] = *WeightPointers[iPtr];
  }
  return ReturnVector;
}

void SetHandler::PrintWeights() {
  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    SplineSets[iSyst]->PrintWeights();
  }
}

std::vector<double*> SetHandler::ReturnSplineWeightPointers(std::vector<std::string> SystNames_, std::vector<std::string> ParNames_, std::vector<double> ParVals_) {
  std::vector<double*> ReturnVector;

  for (size_t iSyst=0;iSyst<SystNames_.size();iSyst++) {
    int SystIndex = ReturnSplineSetIndex(SystNames_[iSyst]);
    if (SystIndex == -1) {
      continue;
    }
    
    std::vector<double*> Weights = SplineSets[SystIndex]->ReturnSplineWeightPointers(ParNames_,ParVals_);
    for (size_t iPtr=0;iPtr<Weights.size();iPtr++) {
      ReturnVector.push_back(Weights[iPtr]);
    }
  }
  
  return ReturnVector;
}
