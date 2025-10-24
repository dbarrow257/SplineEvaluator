#pragma once

#include "SplineSet.h"
#include "yaml-cpp/yaml.h"

#include <Eigen/Dense>

/**
 * @file SplineSet_Eigen.h
 *
 * @class SplineSet_Eigen
 *
 * @brief 
 */
class SplineSet_Eigen : public SplineSet {
public:
  // ========================================================================================================================================================================
  SplineSet_Eigen(YAML::Node Config);
  
  /**
   * @brief Destructor
   */
  virtual ~SplineSet_Eigen();
  
protected:
  void SetupMemoryStructure();
  void ReadSplines(std::string FileName_);
  void EvaluateSplines(double DialValue_);
  double* ReturnSplineWeightPointer(int Index_);

private:
  Eigen::MatrixXd Coefficients;
  Eigen::VectorXd Weights;
};
