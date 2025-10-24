#pragma once

#include <vector>
#include <string>

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
  CoeffD = 3,
  nCoefficientsPerKnot = 4
};

struct SplineIdentifier {
  std::vector<std::string> ParIdentifiers;
  std::vector<double> ParVals;
};

class InputHandler {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~InputHandler();

  InputHandler(std::string InputFileName_);

  virtual std::vector<std::string> ReturnSystNames() = 0;
  virtual int DetermineNumberOfSplines(std::string SystName_) = 0;
  virtual std::vector<double> DetermineKnotLocations(std::string SystName_) = 0;
  virtual std::vector<double> GetKnotCoeff(std::string SystName, int SplineIndex, int KnotIndex) = 0;
  virtual SplineIdentifier GetSplineIdentifier(std::string SystName_, int SplineIndex_) = 0;
  
protected:
  std::string InputFileName;

  bool IsSplineFlat(std::vector<double> EvaluationValues_);
private:
};
