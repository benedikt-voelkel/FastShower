#include <iostream>


#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TLegend.h>
#include <TFile.h>
#include <TStyle.h>


void plotWithFit(const char* filename, const char* histName, const char* fitName)
{
  gStyle->SetOptStat(0);

  TFile file(filename, "READ");
  TH1D hist = *(dynamic_cast<TH1D*>(file.Get(histName)));
  TF1 fit = *(dynamic_cast<TF1*>(file.Get(fitName)));

  TCanvas c(histName, "", 600, 500);
  c.cd();
  TLegend legend(0.1, 0.7, 0.25, 0.9);
  legend.SetTextFont(62);
  legend.SetTextSize(0.025);
  hist.SetTitle("");
  hist.SetLineColor(kBlue + 3);

  hist.SetLineWidth(3);
  hist.GetXaxis()->SetTitle("E [GeV]");
  hist.GetYaxis()->SetTitle("#entries / 3.3 x 10^{-4} GeV");

  legend.AddEntry(&hist, "E_{dep}");
  legend.AddEntry(&fit, "fit E_{dep}");

  hist.Draw("hist");
  fit.Draw("same");
  legend.Draw("same");

  std::string saveName(histName);
  saveName += ".eps";

  c.SaveAs(saveName.c_str());




}


void plot(const char* filename1, const char* label1, const char* filename2, const char* label2)
{

  gStyle->SetOptStat(0);

  TFile file1(filename1, "READ");
  TH1D histStepsX1 = *(dynamic_cast<TH1D*>(file1.Get("histStepsX")));
  TH1D histStepsPerPDG1 = *(dynamic_cast<TH1D*>(file1.Get("histStepsPerPDG")));




  TFile file2(filename2, "READ");
  TH1D histStepsX2 = *(dynamic_cast<TH1D*>(file2.Get("histStepsX")));
  TH1D histStepsPerPDG2 = *(dynamic_cast<TH1D*>(file2.Get("histStepsPerPDG")));


  TCanvas cStepsX("histStepsX", "", 600, 500);
  cStepsX.cd();
  TLegend legendStepsX(0.1, 0.7, 0.45, 0.9);
  legendStepsX.SetTextFont(62);
  legendStepsX.SetTextSize(0.025);
  histStepsX1.SetTitle("");
  histStepsX1.SetLineColor(kBlue + 3);

  histStepsX1.SetLineWidth(3);
  histStepsX1.GetXaxis()->SetTitle("x [cm]");
  histStepsX1.GetYaxis()->SetTitle("#steps");
  histStepsX1.GetYaxis()->SetTitleOffset(1.1);
  histStepsX1.GetYaxis()->SetMaxDigits(3);
  histStepsX2.SetLineColor(kRed + 2);
  histStepsX2.SetLineStyle(7);
  histStepsX2.SetLineWidth(3);
  legendStepsX.AddEntry(&histStepsX1, label1);
  legendStepsX.AddEntry(&histStepsX2, label2);

  histStepsX1.Draw("hist");
  histStepsX2.Draw("hist same");
  legendStepsX.Draw("same");

  std::string nameStepsX = std::string(label1) + "_" + label2 + "_StepsX.eps";

  cStepsX.SaveAs(nameStepsX.c_str());


  TCanvas cStepsPerPDG("histStepsPerPDG", "", 600, 500);
  cStepsPerPDG.cd();
  TLegend legendStepsPerPDG(0.55, 0.7, 0.9, 0.9);
  legendStepsPerPDG.SetTextFont(62);
  legendStepsPerPDG.SetTextSize(0.025);
  histStepsPerPDG1.SetTitle("");
  histStepsPerPDG1.SetLineColor(kBlue + 3);
  histStepsPerPDG1.SetLineWidth(3);
  histStepsPerPDG1.GetXaxis()->SetTitle("PDG");
  histStepsPerPDG1.GetXaxis()->SetLabelFont(62);
  histStepsPerPDG1.GetXaxis()->SetLabelSize(0.04);
  histStepsPerPDG1.GetYaxis()->SetTitle("#steps");
  histStepsPerPDG1.GetYaxis()->SetTitleOffset(1.1);
  histStepsPerPDG1.GetYaxis()->SetMaxDigits(3);
  histStepsPerPDG2.SetLineColor(kRed + 2);
  histStepsPerPDG2.SetLineStyle(7);
  histStepsPerPDG2.SetLineWidth(3);
  legendStepsPerPDG.AddEntry(&histStepsPerPDG1, label1);
  legendStepsPerPDG.AddEntry(&histStepsPerPDG2, label2);

  histStepsPerPDG1.Draw("hist");
  histStepsPerPDG2.Draw("hist same");
  legendStepsPerPDG.Draw("same");

  std::string nameStepsPerPDG = std::string(label1) + "_" + label2 + "_StepsPerPDG.eps";

  cStepsPerPDG.SaveAs(nameStepsPerPDG.c_str());

}


void plot2D(const char* filename)
{
  gStyle->SetOptStat(0);
  TFile file(filename, "READ");
  TH2D hist = *(dynamic_cast<TH2D*>(file.Get("histEngineVsVolume")));
  TCanvas c("histEngineVsVolume", "", 600, 500);
  c.cd();
  hist.Draw("COLZ");
  c.SaveAs("engineVsVolume.eps");

  int binsX = hist.GetNbinsX();
  int binsY = hist.GetNbinsY();


  for(int i = 0; i <= hist.GetNbinsY(); i++) {
    for(int j = 0; j <= hist.GetNbinsX(); j++) {
      std::cout << "Bin " << i << ", " << j << " has content " << hist.GetBinContent(j,i) << "\n";
    }
  }
}
