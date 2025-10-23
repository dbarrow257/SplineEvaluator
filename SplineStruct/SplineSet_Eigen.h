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

  void SetupSplineSet();
  void ReadSplines(TFile* File);
  void EvaluateSpline(double DialValue);
  double* ReturnSplineWeightPointer(int Index);
  
protected:

private:

  Eigen::MatrixXd Coefficients;
  Eigen::VectorXd Weights;
};
