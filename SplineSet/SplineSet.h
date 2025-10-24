#pragma once

#include "yaml-cpp/yaml.h"
#include <vector>

#include "InputHandler/InputHandler.h"

enum SplineCoeff {
  CoeffY = 0,
  CoeffB = 1,
  CoeffC = 2,
  CoeffD = 3,
  nCoefficientsPerKnot = 4
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

  void DigestSplines(std::string FileName_);
  void SetInputHandler(InputHandler* Inputs_) {Inputs = Inputs_;}
  void SetSystName(std::string SystName_) {SystName = SystName_;}
  inline std::string GetSystName() {return SystName;}

  void PrintWeights();
  void Evaluate(double DialValue_);
  std::vector<double*> ReturnSplineWeightPointers(std::vector<std::string> ParNames_, std::vector<double> ParValues_);
  std::vector<double> ReturnSplineWeights(std::vector<std::string> ParNames_, std::vector<double> ParValues_);
  
protected:
  SplineSet(YAML::Node Config_);

  inline int GetNSplines() {return nSplines;}
  inline int GetNKnots() {return nKnots;}
  inline int GetSegmentIndex() {return SegmentIndex;}
  
  void UpdateSplineSegment(double DialValue_);
  int DetermineNumberOfSplines(std::string FileName_);
  std::vector<double> DetermineKnotLocations(std::string FileName_);
  double ReturnSplineWeight(int Index_);

  virtual void SetupMemoryStructure() = 0;
  virtual void EvaluateSplines(double DialValue_) = 0;
  virtual double* ReturnSplineWeightPointer(int Index_) = 0;
  virtual void ReadSplines(std::string FileName_) = 0;
  
  std::vector<double> KnotLocations;
  std::vector<SplineIdentifier> SplineIdentifiers;

  InputHandler* Inputs;
  
private:
  int nSplines;
  int nKnots;
  int SegmentIndex;
  
  std::string SystName;
  std::string InputFileName;
};
