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
  Coefficients = Eigen::MatrixXd(GetNSplines(),GetNKnots()*nCoefficientsPerKnot);
  Weights = Eigen::VectorXd(GetNSplines());
}

void SplineSet_Eigen::ReadSplines(std::string FileName_) {
  TFile* File = TFile::Open(FileName_.c_str());
  
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
    if (GetSystName() != SplineSystName) continue;

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
}

void SplineSet_Eigen::SetKnotCoefficients(int SplineIndex, int KnotIndex, std::vector<double> Coeffs_) {
  for (int iCoeffIndex=0;iCoeffIndex<SplineCoeff::nCoefficientsPerKnot;iCoeffIndex++) {
    Coefficients(SplineIndex,(KnotIndex*nCoefficientsPerKnot)+iCoeffIndex) = Coeffs_[iCoeffIndex];
  }
}

void SplineSet_Eigen::EvaluateSplines(double DialValue) {
  double DelX = DialValue-KnotLocations[GetSegmentIndex()];
  Eigen::VectorXd Y = Coefficients.col(GetSegmentIndex()*nCoefficientsPerKnot+SplineCoeff::CoeffY);
  Eigen::VectorXd B = Coefficients.col(GetSegmentIndex()*nCoefficientsPerKnot+SplineCoeff::CoeffB);
  Eigen::VectorXd C = Coefficients.col(GetSegmentIndex()*nCoefficientsPerKnot+SplineCoeff::CoeffC);
  Eigen::VectorXd D = Coefficients.col(GetSegmentIndex()*nCoefficientsPerKnot+SplineCoeff::CoeffD);

  Weights = Y+DelX*(B+DelX*(C+D*DelX));
}

double* SplineSet_Eigen::ReturnSplineWeightPointer(int Index) {
  return &Weights[Index];
}
