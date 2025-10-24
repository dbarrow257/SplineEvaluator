#include "InputHandler_SingleRootDir.h"

#include <iostream>

#include "TFile.h"
#include "TKey.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TSpline.h"

InputHandler_SingleRootDir::InputHandler_SingleRootDir(std::string InputFileName_) : InputHandler(InputFileName_) {
  File = TFile::Open(InputFileName.c_str());

  int Counter = 0;
  for (auto k : *File->GetListOfKeys()) {
    auto Key = static_cast<TKey*>(k);
    TClass *Class = gROOT->GetClass(Key->GetClassName(), false);
    if(!Class->InheritsFrom("TSpline3")) {
      continue;
    }

    //Check if flat spline
    TSpline3* Spl = Key->ReadObject<TSpline3>();
    std::vector<double> EvalValues(Spl->GetNp());
    double X;
    double Y;
    for (int iKnot = 0; iKnot < Spl->GetNp(); iKnot++) {
      Spl->GetKnot(iKnot, X, Y);
      EvalValues[iKnot] = Y;
    }
    if (IsSplineFlat(EvalValues)) continue;

    TString FullSplineName = Key->GetName();
    TObjArray *tokens = TString(FullSplineName).Tokenize("_");
    if (tokens->GetEntries() != TokenEnum::nTokenEnums) {
      std::cerr << "Unrecognised Spline Name format!:" << FullSplineName << std::endl;
      throw;
    }

    std::string SystName = std::string(((TObjString*)(tokens->At(TokenEnum::SystName)))->GetString());
    
    bool Found = false;
    for (size_t iFoundSystNames=0;iFoundSystNames<FoundSystNames.size();iFoundSystNames++) {
      if (SystName == FoundSystNames[iFoundSystNames]) {
	Found = true;
	break;
      }
    }
    if (!Found) {
      FoundSystNames.push_back(SystName);
      SystIndexOffsets.push_back(Counter);
    }
    
    Keys.push_back((TKey*)Key->Clone());
    Counter += 1;
  }
  
}

InputHandler_SingleRootDir::~InputHandler_SingleRootDir() {
}

int InputHandler_SingleRootDir::FindSystIndex(std::string SystName_) {
  int SystIndex = -1;
  for (size_t iSyst=0;iSyst<FoundSystNames.size();iSyst++) {
    if (FoundSystNames[iSyst] == SystName_) {
      SystIndex = iSyst;
      break;
    }
  }
  if (SystIndex == -1) {
    std::cerr << "Did not find systematic:" << SystName_ << std::endl;
    throw;
  }

  return SystIndex;
}

int InputHandler_SingleRootDir::DetermineNumberOfSplines(std::string SystName_) {
  int SystIndex = FindSystIndex(SystName_);

  int nSplines;
  if (SystIndex == (FoundSystNames.size()-1)) {
    nSplines = Keys.size()-SystIndexOffsets[SystIndex];
  } else {
    nSplines = SystIndexOffsets[SystIndex+1]-SystIndexOffsets[SystIndex];
  }

  return nSplines;
}

std::vector<double> InputHandler_SingleRootDir::DetermineKnotLocations(std::string SystName_) {
  std::vector<double> KnotLocs;
  
  bool ReadKnotLocations = true;
  int nKnots = -1;

  int Start;
  int End;

  int SystIndex = FindSystIndex(SystName_);
  if (SystIndex == (FoundSystNames.size()-1)) {
    Start = SystIndexOffsets[SystIndex];
    End = Keys.size();
  } else {
    Start = SystIndexOffsets[SystIndex];
    End = SystIndexOffsets[SystIndex+1];
  }
  
  for (int iSpline=Start;iSpline<End;iSpline++) {
    TKey* Key = Keys[iSpline];
    
    TSpline3* Spl = Key->ReadObject<TSpline3>();
    if (nKnots == -1) {
      nKnots = Spl->GetNp();
    }
    if (nKnots != Spl->GetNp()) {
      std::cerr << "Found unequal number of knots for splines" << std::endl;
      throw;
    }

    if (ReadKnotLocations) {
      ReadKnotLocations = false;
      KnotLocs.resize(nKnots);

      for (int iKnot=0;iKnot<Spl->GetNp();iKnot++) {
        double X = -1;
        double Y = -1;
        double B = -1;
        double C = -1;
        double D = -1;

        Spl->GetCoeff(iKnot,X, Y, B, C, D);
        KnotLocs[iKnot] = X;
      }
    } else {
      for (int iKnot=0;iKnot<Spl->GetNp();iKnot++) {
        double X = -1;
        double Y = -1;
        double B = -1;
        double C = -1;
        double D = -1;

        Spl->GetCoeff(iKnot,X, Y, B, C, D);
        if (X != KnotLocs[iKnot]) {
          std::cerr << "Found two splines with different knot locations" << std::endl;
          throw;
        }
      }
    }
  }

  return KnotLocs;
}

std::vector<double> InputHandler_SingleRootDir::GetKnotCoeff(std::string SystName_, int SplineIndex_, int KnotIndex_) {

  int SystIndex = FindSystIndex(SystName_);

  TKey* Key = Keys[SystIndexOffsets[SystIndex]+SplineIndex_];
  TSpline3* Spl = Key->ReadObject<TSpline3>();
  std::vector<double> Coeffs = std::vector<double>(nCoefficientsPerKnot);
  
  double X = -1;
  double Y = -1;
  double B = -1;
  double C = -1;
  double D = -1;
  Spl->GetCoeff(KnotIndex_,X, Y, B, C, D);
  
  Coeffs[SplineCoeff::CoeffY] = Y;
  Coeffs[SplineCoeff::CoeffB] = B;
  Coeffs[SplineCoeff::CoeffC] = C;
  Coeffs[SplineCoeff::CoeffC] = D;

  return Coeffs;
}

SplineIdentifier InputHandler_SingleRootDir::GetSplineIdentifier(std::string SystName_, int SplineIndex_) {
  int SystIndex = FindSystIndex(SystName_);

  TKey* Key = Keys[SystIndexOffsets[SystIndex]+SplineIndex_];
  TSpline3* Spl = Key->ReadObject<TSpline3>();

  TString FullSplineName = Key->GetName();
  TObjArray *tokens = TString(FullSplineName).Tokenize("_");
  if (tokens->GetEntries() != TokenEnum::nTokenEnums) {
    std::cerr << "Unrecognised Spline Name format!:" << FullSplineName << std::endl;
    throw;
  }
  
  std::string SplineXBinVarName = std::string(((TObjString*)(tokens->At(TokenEnum::XBinIdentifier)))->GetString());
  std::string SplineXBinIndex = std::string(((TObjString*)(tokens->At(TokenEnum::XBinValue)))->GetString());
  std::string SplineYBinVarName = std::string(((TObjString*)(tokens->At(TokenEnum::YBinIdentifier)))->GetString());
  std::string SplineYBinIndex = std::string(((TObjString*)(tokens->At(TokenEnum::YBinValue)))->GetString());
  
  SplineIdentifier SplIden = SplineIdentifier();
  SplIden.ParIdentifiers = {SplineXBinVarName,SplineYBinVarName};
  SplIden.ParVals = {std::stod(SplineXBinIndex),std::stod(SplineYBinIndex)};
  
  return SplIden;
}
