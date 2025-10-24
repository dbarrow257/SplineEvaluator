#include "InputHandler_SingleRootDir.h"

#include <iostream>

#include "TFile.h"
#include "TKey.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TSpline.h"

InputHandler_SingleRootDir::InputHandler_SingleRootDir(std::string InputFileName_) : InputHandler(InputFileName_) {
}

InputHandler_SingleRootDir::~InputHandler_SingleRootDir() {
}

std::vector<std::string> InputHandler_SingleRootDir::ReturnSystNames() {
  TFile* File = TFile::Open(InputFileName.c_str());

  std::vector<std::string> SystNames;

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

    std::string SystName = std::string(((TObjString*)(tokens->At(TokenEnum::SystName)))->GetString());

    bool AlreadyFound = false;
    for (size_t iSystName=0;iSystName<SystNames.size();iSystName++) {
      if (SystNames[iSystName] == SystName) {
        AlreadyFound = true;
        break;
      }
    }
    if (!AlreadyFound) {
      SystNames.push_back(SystName);
    }

  }

  std::cout << "Found " << SystNames.size() << " Systematics:" << std::endl;
  for (size_t iSystName=0;iSystName<SystNames.size();iSystName++) {
    std::cout << "\t" << iSystName << " : " << SystNames[iSystName] << std::endl;
  }
  std::cout << std::endl;

  return SystNames;
}

int InputHandler_SingleRootDir::DetermineNumberOfSplines(std::string SystName_) {
  TFile* File = TFile::Open(InputFileName.c_str());
  int nSpls = 0;
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
    if (SystName_ != SplineSystName) continue;
    nSpls += 1;
  }

  File->Close();  
  return nSpls;
}

std::vector<double> InputHandler_SingleRootDir::DetermineKnotLocations(std::string SystName_) {
  TFile* File = TFile::Open(InputFileName.c_str());

  std::vector<double> KnotLocs;

  bool ReadKnotLocations = true;
  int nKnots = -1;
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
    if (SystName_ != SplineSystName) continue;

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

  File->Close();
  return KnotLocs;
}
