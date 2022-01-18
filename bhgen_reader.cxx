
//C++
#include <fstream>
#include <iostream>
#include <algorithm>

//Boost
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>

//HepMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"

//ROOT
#include "TTree.h"
#include "TFile.h"

using namespace std;
using namespace boost;
using namespace HepMC3;

//_____________________________________________________________________________
int main(int argc, char* argv[]) {

  //configuration file
  program_options::options_description opt("opt");
  opt.add_options()
    ("main.input", program_options::value<string>(), "bhgen input")
    ("main.hepmc", program_options::value<string>(), "hepmc output")
    ("main.root", program_options::value<string>(), "ROOT output")
    ("main.npart", program_options::value<int>()->default_value(3), "Particles per event")
    ("main.nmax", program_options::value<long>()->default_value(-1), "Maximal number of events")
    ("main.invert_z", program_options::value<bool>(), "Inversion along z axis")
  ;

  //load the configuration file
  ifstream config(argv[1]);
  program_options::variables_map opt_map;
  program_options::store(program_options::parse_config_file(config, opt), opt_map);

  //bhgen input
  string input = opt_map["main.input"].as<string>();
  input.erase(remove(input.begin(), input.end(), '\"'), input.end());
  cout << "Input: " << input << endl;
  ifstream in(input);

  //HepMC3 output
  string hepmc_output = opt_map["main.hepmc"].as<string>();
  hepmc_output.erase(remove(hepmc_output.begin(), hepmc_output.end(), '\"'), hepmc_output.end());
  cout << "HepMC3: " << hepmc_output << endl;
  std::shared_ptr<GenRunInfo> run = std::make_shared<GenRunInfo>();
  WriterAscii file(hepmc_output, run);

  //ROOT output
  string root_output = opt_map["main.root"].as<string>();
  root_output.erase(remove(root_output.begin(), root_output.end(), '\"'), root_output.end());
  cout << "ROOT: " << root_output << endl;
  TFile out_root(root_output.c_str(), "recreate");
  TTree otree("bhgen_tree", "bhgen_tree");
  Double_t phot_en, phot_theta, phot_phi, el_en, el_theta, el_phi, p_en, p_theta, p_phi, p_pt;
  otree.Branch("phot_en", &phot_en, "phot_en/D");
  otree.Branch("phot_theta", &phot_theta, "phot_theta/D");
  otree.Branch("phot_phi", &phot_phi, "phot_phi/D");
  otree.Branch("el_en", &el_en, "el_en/D");
  otree.Branch("el_theta", &el_theta, "el_theta/D");
  otree.Branch("el_phi", &el_phi, "el_phi/D");
  otree.Branch("p_en", &p_en, "p_en/D");
  otree.Branch("p_theta", &p_theta, "p_theta/D");
  otree.Branch("p_phi", &p_phi, "p_phi/D");
  otree.Branch("p_pt", &p_pt, "p_pt/D");

  //particles per event
  int npart = opt_map["main.npart"].as<int>();

  //inversion along z axis
  int invert_z = 1;
  if( opt_map.count("main.invert_z") and opt_map["main.invert_z"].as<bool>() ) {
    invert_z = -1;
  }

  //input loop
  long iev = 0; // line index
  long nmax = opt_map["main.nmax"].as<long>(); // maximal number of events

  string line; // read line
  char_separator<char> sep(" "); // for tokenizer
  while( getline(in, line) ) {
    if( nmax >= 0 and iev >= nmax ) break;
    iev++;

    //HepMC3 event
    GenEvent evt(Units::GEV, Units::MM);
    evt.set_event_number(iev-1);

    //particle loop
    for(int i=0; i<npart; i++) {

      getline(in, line);

      //split the line
      tokenizer< char_separator<char> > lin(line, sep);
      auto it = lin.begin();

      //pdg
      int pdg = stoi(*it++);

      //energy and momentum
      double en = stod(*it++);
      double px = stod(*it++);
      double py = stod(*it++);
      double pz = invert_z*stod(*it++);

      //HepMC particle
      FourVector vec(px, py, pz, en);
      GenParticlePtr p0 = std::make_shared<GenParticle>(vec, pdg, 1);
      evt.add_particle(p0);

      //ROOT tree
      //gamma
      if(pdg == 22) {
        phot_en = vec.e();
        phot_theta = vec.theta();
        phot_phi = vec.phi();
      }
      //electron
      if(TMath::Abs(pdg) == 11) {
        el_en = vec.e();
        el_theta = vec.theta();
        el_phi = vec.phi();
      }
      //proton
      if(pdg == 2212) {
        p_en = vec.e();
        p_theta = vec.theta();
        p_phi = vec.phi();
        p_pt = vec.pt();
      }

      //cout << pdg << " " << en << " " << px << " " << py << " " << pz << endl;

    }//particle loop

    file.write_event(evt);
    otree.Fill();

  }//input loop

  file.close();
  in.close();
  otree.Write();
  out_root.Close();

  cout << "All done, number of events: " << iev << endl;

  return 0;

}

















