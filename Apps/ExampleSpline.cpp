#include "yaml-cpp/yaml.h"

#include "SetHandler/SetHandler.h"

#include <iostream>
#include <chrono>

int main() {
  YAML::Node Node = YAML::Node();
  Node["InputFileName"] = "AtmosphericFluxShapeSplines.root";
  
  SetHandler* Splines = new SetHandler(Node);

  //Definition of systematics
  std::vector<double> SystDialValues = {           1,          1,          1};
  std::vector<std::string> SystNames = {"TotalRatio", "NuMRatio", "NuERatio"};

  //Spline evaluation
  int nThrows = 1000;
  srand(0); //Set Random seed to known value
  std::cout << "Evaluating all splines " << nThrows << " times.." << std::endl;

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  
  for (int iThrow=0;iThrow<nThrows;iThrow++) {
    for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
      SystDialValues[iSyst] = ((double) rand() / (RAND_MAX)) * 8; //Gives Random number between 0 and 8 which is range where spline is defined
    }
    Splines->EvaluateSplines(SystDialValues);
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Average time of evaluation = " << (std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count())/nThrows << "[ns]" << std::endl;
  std::cout << std::endl;

  //Do some printing of all weights at known value for comparison
  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    SystDialValues[iSyst] = 5.;
  }
  Splines->EvaluateSplines(SystDialValues);
  Splines->PrintWeights();

  //Used for weight pointer returning (e.g. let's return the weight for the XBin==1 and YBin==1 splines for the TotalRatio and NuERatio systematics)
  std::vector<std::string> SplineBinning_SystNames = {"TotalRatio", "NuERatio"};
  std::vector<std::string> SplineBinning_ParameterNames    = {"XBin","YBin"};
  std::vector<double> SplineBinning_ParameterValuesToPrint = {     1,     1};

  std::cout << "Weights for specified systematics and matched variables.." << std::endl;
  std::vector<double*> Weights = Splines->ReturnSplineWeightPointers(SplineBinning_SystNames,SplineBinning_ParameterNames,SplineBinning_ParameterValuesToPrint);
  for (size_t iWeight = 0;iWeight<Weights.size();iWeight++) {
    std::cout << "\t" << iWeight << " - " << *Weights[iWeight] << std::endl;
  }
  
  return 0;
}
