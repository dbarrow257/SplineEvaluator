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

class InputHandler {
public:
  // ========================================================================================================================================================================
  
  /**
   * @brief Destructor
   */
  virtual ~InputHandler();

  InputHandler(std::string InputFileName_);
  
  virtual int DetermineNumberOfSplines(std::string SystName_) = 0;
  virtual std::vector<double> DetermineKnotLocations(std::string SystName_) = 0;
  virtual std::vector<std::string> ReturnSystNames() = 0;
  
protected:
  std::string InputFileName;
  
private:
};
