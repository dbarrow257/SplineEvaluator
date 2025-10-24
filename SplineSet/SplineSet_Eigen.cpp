#include "SplineSet_Eigen.h"

#include <iostream>

#include "TKey.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSpline.h"
#include "TObjString.h"

SplineSet_Eigen::SplineSet_Eigen(YAML::Node InputConfig_) : SplineSet(InputConfig_) {
}

SplineSet_Eigen::~SplineSet_Eigen() {
}

void SplineSet_Eigen::SetupMemoryStructure() {
  Coefficients = Eigen::MatrixXd(GetNSplines(),GetNKnots()*SplineCoeff::nCoefficientsPerKnot);
  Weights = Eigen::VectorXd(GetNSplines());
}

void SplineSet_Eigen::SetKnotCoefficients(int SplineIndex, int KnotIndex, std::vector<double> Coeffs_) {
  for (int iCoeffIndex=0;iCoeffIndex<SplineCoeff::nCoefficientsPerKnot;iCoeffIndex++) {
    Coefficients(SplineIndex,(KnotIndex*SplineCoeff::nCoefficientsPerKnot)+iCoeffIndex) = Coeffs_[iCoeffIndex];
  }
}

void SplineSet_Eigen::EvaluateSplines(double DialValue) {
  double DelX = DialValue-KnotLocations[GetSegmentIndex()];
  Eigen::VectorXd Y = Coefficients.col(GetSegmentIndex()*SplineCoeff::nCoefficientsPerKnot+SplineCoeff::CoeffY);
  Eigen::VectorXd B = Coefficients.col(GetSegmentIndex()*SplineCoeff::nCoefficientsPerKnot+SplineCoeff::CoeffB);
  Eigen::VectorXd C = Coefficients.col(GetSegmentIndex()*SplineCoeff::nCoefficientsPerKnot+SplineCoeff::CoeffC);
  Eigen::VectorXd D = Coefficients.col(GetSegmentIndex()*SplineCoeff::nCoefficientsPerKnot+SplineCoeff::CoeffD);

  Weights = Y+DelX*(B+DelX*(C+D*DelX));
}

double* SplineSet_Eigen::ReturnSplineWeightPointer(int Index) {
  return &Weights[Index];
}
