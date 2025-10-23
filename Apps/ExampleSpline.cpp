#include "SplineStruct/SplineSet_Eigen.h"

#include "yaml-cpp/yaml.h"

#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TKey.h"
#include "TH2.h"
#include "TObjString.h"

std::vector<std::string> ReturnSystNames(TFile* File) {
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

TH2* ReturnTemplateHistogram(TFile* File) {
  TH2* TemplateHistogram = File->Get<TH2>("HistogramTemplate_Interp");

  if (!TemplateHistogram) {
    std::cerr << "Did not find 'HistogramTemplate_Interp' in the input file" << std::endl;
    throw;
  }

  return TemplateHistogram;
}

int main() {
  std::string InputFileName = "AtmosphericFluxShapeSplines.root";
  TFile* File = TFile::Open(InputFileName.c_str());

  std::vector<std::string> SystNames = ReturnSystNames(File);
  TH2* TemplateHistogram = ReturnTemplateHistogram(File);

  int nXBins = TemplateHistogram->GetNbinsX();
  int nYBins = TemplateHistogram->GetNbinsY();

  std::vector<SplineSet_Eigen*> SplineSets(SystNames.size());
  for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
    SplineSets[iSyst] = new SplineSet_Eigen(YAML::Node());
    SplineSets[iSyst]->SetSystName(SystNames[iSyst]);
    SplineSets[iSyst]->DigestSplines(File);
  }

  int nThrows = 1000;

  std::vector<std::string> ParNames = {"XBin","YBin"};
  std::vector<double> ParVales = {1,1};
  
  for (int iThrow=0;iThrow<nThrows;iThrow++) {
    for (size_t iSyst=0;iSyst<SystNames.size();iSyst++) {
      double ParamVal = ((double) rand() / (RAND_MAX)) * 8;
      SplineSets[iSyst]->EvaluateSpline(ParamVal);
      std::vector<double> Weights = SplineSets[iSyst]->ReturnSplineWeights(ParNames,ParVales);
      for (size_t iWeight=0;iWeight<Weights.size();iWeight++) {
	std::cout << iSyst << " " << iWeight << " " << Weights[iWeight] << std::endl;
      }
    }
  }
  
  return 0;
}
