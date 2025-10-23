#include "SplineSet_Eigen.h"

#include <iostream>

#include "TKey.h"
#include "TROOT.h"
#include "TSpline.h"
#include "TObjString.h"

SplineSet_Eigen::SplineSet_Eigen(YAML::Node InputConfig_) : SplineSet(InputConfig_) {
}

SplineSet_Eigen::~SplineSet_Eigen() {
}

void SplineSet_Eigen::SetupSplineSet() {
  Coefficients = Eigen::MatrixXd(nSplines,nKnots*nCoefficientsPerKnot);
  Weights = Eigen::VectorXd(nSplines);
}

void SplineSet_Eigen::ReadSplines(TFile* File) {
  int SplineCounter = 0;
  for (auto k : *File->GetListOfKeys()) {
    auto Key = static_cast<TKey*>(k);
    TClass *Class = gROOT->GetClass(Key->GetClassName(), false);
    if(!Class->InheritsFrom("TSpline3")) {
      continue;
    }

    TString FullSplineName = Key->GetName();
    TObjArray *tokens = TString(FullSplineName).Tokenize("_");
    if (tokens->GetEntries() != TokenEnum::nTokenEnums) {
      std::cerr << "Unrecognised Spline Name format!:" << FullSplineName << std::endl;
      throw;
    }

    std::string SplineSystName = std::string(((TObjString*)(tokens->At(TokenEnum::SystName)))->GetString());
    if (SystName != SplineSystName) continue;

    std::string SplineXBinVarName = std::string(((TObjString*)(tokens->At(TokenEnum::XBinIdentifier)))->GetString());
    std::string SplineXBinIndex = std::string(((TObjString*)(tokens->At(TokenEnum::XBinValue)))->GetString());
    std::string SplineYBinVarName = std::string(((TObjString*)(tokens->At(TokenEnum::YBinIdentifier)))->GetString());
    std::string SplineYBinIndex = std::string(((TObjString*)(tokens->At(TokenEnum::YBinValue)))->GetString());
    
    TSpline3* Spl = Key->ReadObject<TSpline3>();

    for (int iKnot=0;iKnot<Spl->GetNp();iKnot++) {
      double X = -1;
      double Y = -1;
      double B = -1;
      double C = -1;
      double D = -1;

      Spl->GetCoeff(iKnot,X, Y, B, C, D);
      
      Coefficients(SplineCounter,(iKnot*nCoefficientsPerKnot)+SplineCoeff::CoeffY) = Y;
      Coefficients(SplineCounter,(iKnot*nCoefficientsPerKnot)+SplineCoeff::CoeffB) = B;
      Coefficients(SplineCounter,(iKnot*nCoefficientsPerKnot)+SplineCoeff::CoeffC) = C;
      Coefficients(SplineCounter,(iKnot*nCoefficientsPerKnot)+SplineCoeff::CoeffD) = D;
    }

    SplineIdentifier SplIden = SplineIdentifier();
    SplIden.ParIdentifiers = {SplineXBinVarName,SplineYBinVarName};
    SplIden.ParVals = {std::stod(SplineXBinIndex),std::stod(SplineYBinIndex)};
    SplineIdentifiers[SplineCounter] = SplIden;
    
    SplineCounter += 1;
  }

  std::cout << "Knot locations for systematic:" << SystName << " - " << std::endl;
  std::cout << "\t";
  for (int iKnot=0;iKnot<nKnots;iKnot++) {    
    std::cout << KnotLocations[iKnot] << ", ";
  }
  std::cout << std::endl;
  
  std::cout << "Successfully read coefficients for systematic:" << SystName << "\n" << std::endl;
}

void SplineSet_Eigen::EvaluateSpline(double DialValue) {
  if (DialValue < KnotLocations[0] || DialValue > KnotLocations[nKnots-1]) {
    std::cerr << "Invalid Dial Value - Outside of Spline Range [" << KnotLocations[0] << ", " << KnotLocations[nKnots-1] << "]" << std::endl;
    throw;
  }

  int SegmentIndex = static_cast<int>(std::distance(KnotLocations.begin(), std::upper_bound(KnotLocations.begin(), KnotLocations.end(), DialValue)) - 1);

  double DelX = DialValue-KnotLocations[SegmentIndex];
  Eigen::VectorXd Y = Coefficients.col(SegmentIndex*nCoefficientsPerKnot+SplineCoeff::CoeffY);
  Eigen::VectorXd B = Coefficients.col(SegmentIndex*nCoefficientsPerKnot+SplineCoeff::CoeffB);
  Eigen::VectorXd C = Coefficients.col(SegmentIndex*nCoefficientsPerKnot+SplineCoeff::CoeffC);
  Eigen::VectorXd D = Coefficients.col(SegmentIndex*nCoefficientsPerKnot+SplineCoeff::CoeffD);

  Weights = Y+DelX*(B+DelX*(C+D*DelX));
}

double* SplineSet_Eigen::ReturnSplineWeightPointer(int Index) {
  return &Weights[Index];
}
