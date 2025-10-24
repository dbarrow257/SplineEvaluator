#pragma once

#include "InputHandler.h"

class InputHandler_SingleRootDir : public InputHandler {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~InputHandler_SingleRootDir();

  InputHandler_SingleRootDir(std::string InputFileName_);
  
  int DetermineNumberOfSplines(std::string SystName_);
  std::vector<double> DetermineKnotLocations(std::string SystName_);
  std::vector<std::string> ReturnSystNames();
  
protected:

private:
};
