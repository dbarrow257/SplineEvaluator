#include "SplineSet.h"

#include <string>
#include <iostream>

#include "TROOT.h"
#include "TKey.h"
#include "TSpline.h"
#include "TObjString.h"

SplineSet::SplineSet(YAML::Node Config_) {
  nCoefficientsPerKnot = 4;
}

SplineSet::~SplineSet() {
}

void SplineSet::DigestSplines(TFile* File) {
  nSplines = 0;
  nKnots = -1;

  bool ReadKnotLocations = true;

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
    nSplines += 1;

    TSpline3* Spl = Key->ReadObject<TSpline3>();
    if (nKnots == -1) {
      nKnots = Spl->GetNp();
    }
    if (nKnots != Spl->GetNp()) {
      std::cerr << "Found unequal number of knots for spline for systematic:" << SplineSystName << std::endl;
      throw;
    }

    if (ReadKnotLocations) {
      ReadKnotLocations = false;
      KnotLocations.resize(nKnots);
      
      for (int iKnot=0;iKnot<Spl->GetNp();iKnot++) {
        double X = -1;
        double Y = -1;
        double B = -1;
        double C = -1;
        double D = -1;
	
        Spl->GetCoeff(iKnot,X, Y, B, C, D);
        KnotLocations[iKnot] = X;
      }
    }
    
  }

  std::cout << "Found total of " << nSplines << ", each with " << nKnots << " knots, for systematic:" << SystName << std::endl;

  SplineIdentifiers.resize(nSplines);
  SetupSplineSet();
  
  ReadSplines(File);
}

std::vector<double*> SplineSet::ReturnSplineWeightPointers(std::vector<std::string> ParNames, std::vector<double> ParValues) {
  std::vector<double*> ReturnWeights;
  
  for (int iSpline=0;iSpline<nSplines;iSpline++) {
    bool Matched = true;
    
    if (ParNames.size() != SplineIdentifiers[iSpline].ParIdentifiers.size()) {
      continue;
    }
    for (size_t iParName=0;iParName<ParNames.size();iParName++) {
      if (ParNames[iParName] != SplineIdentifiers[iSpline].ParIdentifiers[iParName]) {
	Matched = false;
	break;
      }
    }
    if (!Matched) continue;
    
    if (ParValues.size() != SplineIdentifiers[iSpline].ParVals.size()) {
      continue;
    }
    for (size_t iParVal=0;iParVal<ParValues.size();iParVal++) {
      if (ParValues[iParVal] != SplineIdentifiers[iSpline].ParVals[iParVal]) {
	Matched = false;
        continue;
      }
    }
    if (!Matched) continue;

    ReturnWeights.push_back(ReturnSplineWeightPointer(iSpline));
  }

  return ReturnWeights;
}

std::vector<double> SplineSet::ReturnSplineWeights(std::vector<std::string> ParNames, std::vector<double> ParValues) {
  std::vector<double*> SplineWeightPointers = ReturnSplineWeightPointers(ParNames,ParValues);
  std::vector<double> SplineWeights = std::vector<double>(SplineWeightPointers.size());
  for (size_t iSpl=0;iSpl<SplineWeightPointers.size();iSpl++) {
    SplineWeights[iSpl] = *SplineWeightPointers[iSpl];
  }
  return SplineWeights;
}
