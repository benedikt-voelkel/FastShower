#include <vector>
#include <string>

#include <boost/program_options.hpp>

#include <TFile.h>
#include <TH1D.h>
#include <TGeoManager.h>

#include "FastShowerMCApplication.h"
#include "FastShowerPrimaryGenerator.h"

#include "FastShower.h"

#include "TG4RunConfiguration.h"
#include "TGeant4.h"


void convertToBinEdges(std::vector<double>& binEdges, TH1D* histo)
{
  binEdges.resize(histo->GetNbinsX(), -1.);
  double sum = 0.;
  for(int i = 0; i < histo->GetNbinsX(); i++) {
    // These will be the future bin widths
    int binX = histo->GetBin(i+1);
    binEdges[i] = histo->GetBinContent(binX);
    sum += binEdges[i];
  }
  if(sum <= 0.) {
    std::cerr << "Passed histogram was empty" << std::endl;
    exit(1);
  }
  // That is the first upper bin edge assuming the first lower bin edge is 0.
  binEdges[0] /= sum;
  for(int i = 1; i < binEdges.size() - 1; i++) {
    binEdges[i] = binEdges[i-1] + binEdges[i] / sum;
  }
  // Set the last upper bin edge explicitly to 1.
  binEdges.back() = 1.;
}

std::vector<std::string> availableCommands = { "run" };


namespace bpo = boost::program_options;

// print help message
void helpMessage(const bpo::options_description& desc)
{
  std::cout << desc << std::endl;
}

int run(const bpo::variables_map& vm, std::string& errorMessage)
{

  #ifdef G4MULTITHREADED
    errorMessage += "WARNING: Not running multithreaded.\n";
    return 1;
  #endif

  FastShowerMCApplication* appl;
  TGeant4* geant4;
  FastShower* fastShower;
  // RunConfiguration for Geant4
  TG4RunConfiguration* runConfiguration  = new TG4RunConfiguration("geomRoot", "FTFP_BERT",
                                                                   "specialCuts+specialControls");

  if(vm.count("fast") && !vm.count("in")) {
    errorMessage += "If \"fast\" option is specified an input file is required.\n";
  }

  std::string filenameOut = vm["out"].as<std::string>();
  std::string histNElectronsName = "histNElectrons";
  char** argv = {};
  int argc = 0;

  if(!vm.count("fast")) { // That's just a G4 run
    appl = new FastShowerMCApplication("ExampleFastShower",  "The exampleFastShower MC application");
    // TGeant4 is needed in any case
    geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration, argc, argv);
  } else { // That's with fast sim
    appl = new FastShowerMCApplication("ExampleFastShower",  "The exampleFastShower MC application", kTRUE, kTRUE);
    // TGeant4 is needed in any case
    geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration, argc, argv);
    // Take first cmd arg as path to ROOT file
    TFile file(vm["in"].as<std::string>().c_str(), "READ");
    // Extract the histogram
    TH1D* histNElectrons = dynamic_cast<TH1D*>(file.Get(histNElectronsName.c_str()));
    std::vector<double> binEdges;
    convertToBinEdges(binEdges, histNElectrons);
    file.Close();
    fastShower = new FastShower(binEdges, 0.51099906*1e-03);
    //appl->SetTransferTrack()
  }

  if(vm.count("export-geometry")) {
    gGeoManager->Export(vm["export-geometry"].as<std::string>().c_str());
  }


  // Run example
  appl->InitMC("");

  appl->GetPrimaryGenerator()->SetNofPrimaries(vm["part-per-event"].as<int>());

  appl->RunMC(vm["nevents"].as<int>());

  appl->WriteHistograms(filenameOut);

  delete appl;

  return 0;

}

// Initialize everything for the final run depending on the command
void initializeForRun(const std::string& cmd, bpo::options_description& cmdOptionsDescriptions, std::function<int(const bpo::variables_map&, std::string&)>& cmdFunction)
{
  if (cmd == "run") {
    cmdOptionsDescriptions.add_options()("help,h", "show this help message and exit")(
                                         "nevents,n", bpo::value<int>()->default_value(5), "choose number of generated events")(
                                         "part-per-event,p", bpo::value<int>()->default_value(1), "choose number of primary particles events")(
                                         "single-g4,s", bpo::value<std::string>(), "run only GEANT4")("fast,f", "run GEANT4 with fast sim")(
                                         "in,i", bpo::value<std::string>(), "ROOT input file containing histograms for fast sim")(
                                         "out,o", bpo::value<std::string>()->default_value("./histograms.root"), "ROOT output file histograms should be written to")(
                                         "export-geometry,e", bpo::value<std::string>()->default_value("./geometry.root"), "export geometry");
    cmdFunction = run;
  }
}






/// Application main program
int main(int argc, char** argv)
{

  // Parse cmd

  // Global variables map mapping the command line options to their values
  bpo::variables_map vm;
  // Description of the available top-level commands/options
  bpo::options_description desc("Available commands/options");
  desc.add_options()("help,h", "show this help message and exit")("command", bpo::value<std::string>(), "command to be executed (\"run\")")("positional", bpo::value<std::vector<std::string>>(), "positional arguments");
  // Dedicated description for positional arguments
  bpo::positional_options_description pos;
  // First positional argument is actually the command, all others are real positional arguments "( "positional", -1 )"
  pos.add("command", 1).add("positional", -1);
  // Parse and store in variables map, allow unregistered since these are not yet specified but will be so as soon as
  // the command name is checked
  bpo::parsed_options parsed = bpo::command_line_parser(argc, argv).options(desc).positional(pos).allow_unregistered().run();
  bpo::store(parsed, vm);

  // check whether 'command' is missing or if there is even no option at all; show global help message in that case
  // and return
  if (!vm.count("command") || vm.empty()) {
    if (!vm.count("help")) {
      std::cerr << "ERROR: Need command to execute.";
    }
    // Print global help message in any case...
    helpMessage(desc);
    // ...but check whther it was explicitly asked for it and return accordingly
    return (!vm.count("help"));
  }

  // Extract command
  const std::string& cmd = vm["command"].as<std::string>();
  if (std::find(availableCommands.begin(), availableCommands.end(), cmd) == availableCommands.end()) {
    std::cerr << "ERROR: Command \"" << cmd << "\" unknown.\n";
    helpMessage(desc);
    return 1;
  }

  // extract all positional arguments passed which are unrecognized by desc
  std::vector<std::string> opts = bpo::collect_unrecognized(parsed.options, bpo::include_positional);
  // since 'command' as positional argument is unknown, it needs to be erased explicitly
  opts.erase(opts.begin());

  // Preprocessing done.
  // Track return value
  int returnValue = 0;
  // function to be run
  std::function<int(const bpo::variables_map&, std::string&)> cmdFunction;
  // corresponding options with description
  bpo::options_description cmdOptionsDescriptions;
  // initialize command function and corresponding options description
  initializeForRun(cmd, cmdOptionsDescriptions, cmdFunction);
  // Parse again to get positional arguments correctly labelled for analysis run
  bpo::store(bpo::command_line_parser(opts).options(cmdOptionsDescriptions).run(), vm);
  // Notify and fix variables map
  bpo::notify(vm);
  // To collect error messsages
  std::string errorMessage("");
  if (vm.size() == 1) {
    errorMessage += "Options required...\n";
    returnValue = 1;
  }
  // check if specific help message is desired
  else if (!vm.count("help")) {
    returnValue = cmdFunction(vm, errorMessage);
  }
  // check return value
  if (returnValue > 0) {
    std::cerr << "ERRORS occured:";
    std::cerr << errorMessage << "\n";
  }
  if (returnValue > 0 || vm.count("help")) {
    helpMessage(cmdOptionsDescriptions);
  }
  return returnValue;
}
