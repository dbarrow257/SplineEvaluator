#pragma once

#include "yaml-cpp/yaml.h"
#include "TFile.h"
#include <vector>

enum TokenEnum {
  SystIdentifier = 0,
  SystName = 1,
  XBinIdentifier = 2,
  XBinValue = 3,
  YBinIdentifier = 4,
  YBinValue = 5,
  nTokenEnums = 6
};

enum SplineCoeff {
  CoeffY = 0,
  CoeffB = 1,
  CoeffC = 2,
  CoeffD = 3
};

struct SplineIdentifier {
  std::vector<std::string> ParIdentifiers;
  std::vector<double> ParVals;
};

/**
 * @file SplineSet.h
 *
 * @class SplineSet
 *
 * @brief 
 */
class SplineSet {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~SplineSet();

  virtual void SetupSplineSet() = 0;
  virtual void ReadSplines(TFile* File) = 0;
  virtual void EvaluateSpline(double DialValue) = 0;
  virtual double* ReturnSplineWeightPointer(int Index) = 0;

  void DigestSplines(TFile* File);
  void SetSystName(std::string SystName_) {SystName = SystName_;}
  std::vector<double*> ReturnSplineWeightPointers(std::vector<std::string> ParNames, std::vector<double> ParValues);
  std::vector<double> ReturnSplineWeights(std::vector<std::string> ParNames, std::vector<double> ParValues);
  
protected:
  SplineSet(YAML::Node Config_);

  int nSplines;
  int nKnots;
  int nCoefficientsPerKnot;
  
  std::string SystName;
  std::vector<double> KnotLocations;
  std::vector<SplineIdentifier> SplineIdentifiers;
};
