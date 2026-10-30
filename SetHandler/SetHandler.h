#pragma once

#include "yaml-cpp/yaml.h"

#include "SplineSet/SplineSet.h"
#include "InputHandler/InputHandler.h"

/**
 * @file SetHandler.h
 *
 * @class SetHandler
 *
 * @brief 
 */
class SetHandler {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~SetHandler();
  SetHandler(YAML::Node Config_);

  void PrintWeights();
  void EvaluateSplines(const std::vector<double>& SystDialValues_);
  std::vector<double> ReturnSplineWeights(const std::vector<std::string>& SystNames_, const std::vector<std::string>& ParNames_, const std::vector<double>& ParVals_);
  std::vector<double*> ReturnSplineWeightPointers(const std::vector<std::string>& SystNames_, const std::vector<std::string>& ParNames_, const std::vector<double>& ParVals_);
  
protected:
  std::vector<std::string> ReturnSystNames(std::string FileName_);
  int ReturnSplineSetIndex(std::string SystName_);
  
  std::vector<std::string> SystNames;
  std::vector<SplineSet*> SplineSets;

  std::string InputFileName;
  InputHandler* Inputs;
};
