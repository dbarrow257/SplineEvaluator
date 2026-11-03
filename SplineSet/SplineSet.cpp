#include "SplineSet.h"

#include <string>
#include <iostream>
#include <iomanip>

#include "TROOT.h"
#include "TKey.h"
#include "TSpline.h"
#include "TObjString.h"

SplineSet::SplineSet(YAML::Node Config_) {
  SegmentIndex = 0;
}

SplineSet::~SplineSet() {
}

void SplineSet::DigestSplines() {
  std::cout << "Digesting Splines for systematic:" << SystName << std::endl;
  nSplines = Inputs->DetermineNumberOfSplines(SystName);
  if (nSplines == 0) {
    std::cerr << "No splines found - likely an issue" << std::endl;
    throw;
  }
  std::cout << "Found " << nSplines << " splines" << std::endl;
  SplineIdentifiers.resize(nSplines);

  KnotLocations = Inputs->DetermineKnotLocations(SystName);
  nKnots = KnotLocations.size();
  std::cout << "Found " << nKnots << " knots per spline" << std::endl;
  std::cout << "Knot locations:" << std::endl;
  std::cout << "\t";
  for (int iKnot=0;iKnot<nKnots;iKnot++) {
    std::cout << KnotLocations[iKnot] << ", ";
  }
  std::cout << std::endl;

  SetupMemoryStructure();

  for (int iSpline=0;iSpline<nSplines;iSpline++) {
    SplineIdentifiers[iSpline] = Inputs->GetSplineIdentifier(SystName,iSpline);
    for (int iKnot=0;iKnot<nKnots;iKnot++) {
      SetKnotCoefficients(iSpline,iKnot,Inputs->GetKnotCoeff(SystName,iSpline,iKnot));
    }
  }

  std::vector<std::string> ReferenceParIdentifiers = SplineIdentifiers[0].ParIdentifiers;
  for (int iSpline=1;iSpline<nSplines;iSpline++) {
    if (ReferenceParIdentifiers.size() != SplineIdentifiers[iSpline].ParIdentifiers.size()) {
      std::cerr << "Splines created with different numbers of parameter names which indicates a problem" << std::endl;
      throw;
    }
    for (size_t iParName=0;iParName<ReferenceParIdentifiers.size();iParName++) {
      if (ReferenceParIdentifiers[iParName] != SplineIdentifiers[iSpline].ParIdentifiers[iParName]) {
	std::cerr << "Splines created with different parameter names which indicates a problem" << std::endl;
	throw;
      }
    }
  }
  std::cout << "Spline parameter names:" << std::endl;
  for (size_t iParName=0;iParName<ReferenceParIdentifiers.size();iParName++) {
    std::cout << "\t" << iParName << ":" << ReferenceParIdentifiers[iParName] << std::endl;
  }
  
  std::cout << "Successfully read coefficients for systematic:" << SystName << "\n" << std::endl;
}

std::vector<const double*> SplineSet::ReturnSplineWeightPointers(std::vector<std::string> ParNames_, std::vector<double> ParValues_) {
  std::vector<const double*> ReturnWeights;

  if (ParNames_.size() != SplineIdentifiers[0].ParIdentifiers.size()) {
    throw;
  }
  for (size_t iParName=0;iParName<ParNames_.size();iParName++) {
    if (ParNames_[iParName] != SplineIdentifiers[0].ParIdentifiers[iParName]) {
      throw;
    }
  }
  if (ParValues_.size() != SplineIdentifiers[0].ParVals.size()) {
    throw;
  }
  
  for (int iSpline=0;iSpline<nSplines;iSpline++) {
    bool Matched = true;
    
    for (size_t iParVal=0;iParVal<ParValues_.size();iParVal++) {
      if (ParValues_[iParVal] != SplineIdentifiers[iSpline].ParVals[iParVal]) {
	Matched = false;
        break;
      }
    }
    if (!Matched) continue;

    ReturnWeights.push_back(ReturnSplineWeightPointer(iSpline));
  }

  return ReturnWeights;
}

std::vector<double> SplineSet::ReturnSplineWeights(std::vector<std::string> ParNames_, std::vector<double> ParValues_) {
  std::vector<const double*> SplineWeightPointers = ReturnSplineWeightPointers(ParNames_,ParValues_);
  std::vector<double> SplineWeights = std::vector<double>(SplineWeightPointers.size());
  for (size_t iSpl=0;iSpl<SplineWeightPointers.size();iSpl++) {
    SplineWeights[iSpl] = *SplineWeightPointers[iSpl];
  }
  return SplineWeights;
}

void SplineSet::Evaluate(double DialValue_) {
  UpdateSplineSegment(DialValue_);
  EvaluateSplines(DialValue_);
}

double SplineSet::ReturnSplineWeight(int Index_) {
  return *ReturnSplineWeightPointer(Index_);
}

void SplineSet::UpdateSplineSegment(double DialValue_) {
  //Haven't moved out of the segment last found
  if ((DialValue_ >= KnotLocations[SegmentIndex]) && (DialValue_ < KnotLocations[SegmentIndex+1])) {
    return;
  }
  
  if (DialValue_ < KnotLocations[0] || DialValue_ > KnotLocations[nKnots-1]) {
    std::cerr << "Invalid Dial Value - Outside of Spline Range [" << KnotLocations[0] << ", " << KnotLocations[nKnots-1] << "]" << std::endl;
    throw;
  }

  SegmentIndex = static_cast<int>(std::distance(KnotLocations.begin(), std::upper_bound(KnotLocations.begin(), KnotLocations.end(), DialValue_)) - 1);
}

void SplineSet::PrintWeights() {
  std::cout << "=========================================================================================" << std::endl;
  std::cout << "Systematic: " << GetSystName() << std::endl;
  for (int iSpline=0;iSpline<GetNSplines();iSpline++) {
    std::cout << std::setw(5) << iSpline << " : ";
    for (size_t iPar=0;iPar<SplineIdentifiers[iSpline].ParIdentifiers.size();iPar++) {
      std::cout << std::setw(15) << SplineIdentifiers[iSpline].ParIdentifiers[iPar] << " = " << std::setw(10) << SplineIdentifiers[iSpline].ParVals[iPar] << ", ";
    }
    std::cout << " -> Weight = " << ReturnSplineWeight(iSpline) << std::endl;
  }
  std::cout << std::endl;
}
