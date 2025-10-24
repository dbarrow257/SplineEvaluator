#pragma once

#include "InputHandler.h"
#include "TFile.h"
#include "TKey.h"

class InputHandler_SingleRootDir : public InputHandler {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~InputHandler_SingleRootDir();

  InputHandler_SingleRootDir(std::string InputFileName_);

  std::vector<std::string> ReturnSystNames() {return FoundSystNames;}
  int DetermineNumberOfSplines(std::string SystName_);
  std::vector<double> DetermineKnotLocations(std::string SystName_);
  std::vector<double> GetKnotCoeff(std::string SysteName_, int SplineIndex_, int KnotIndex_);
  SplineIdentifier GetSplineIdentifier(std::string SystName_, int SplineIndex_);
  
protected:

private:
  int FindSystIndex(std::string SystName_);
  
  TFile* File;
  std::vector<TKey*> Keys;

  std::unordered_map<std::string, int> SystematicOffsets;
  std::vector<std::string> FoundSystNames;
  std::vector<int> SystIndexOffsets;
};
