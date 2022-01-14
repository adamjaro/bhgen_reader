
//C++
#include <fstream>
#include <iostream>

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

  //bhgen input
  string input = "./evt.txt";
  //if( opt_map.count("input") ) {
    //input = opt_map["input"].as<string>();
  //}
  ifstream in(input);

  //HepMC3 output
  string output = "evt.hepmc";
  std::shared_ptr<GenRunInfo> run = std::make_shared<GenRunInfo>();
  WriterAscii file(output, run);

  //ROOT output
  TFile out_root("evt.root", "recreate");
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
  int npart = 3;

  //input loop
  long iev = 0; // line index
  long nmax = -1; // maximal number of events

  string line; // read line
  char_separator<char> sep(" "); // for tokenizer
  while( getline(in, line) ) {
    if( ++iev > nmax and nmax >= 0 ) break;

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
      double pz = stod(*it++);

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

      cout << pdg << " " << en << " " << px << " " << py << " " << pz << endl;

    }//particle loop

    file.write_event(evt);
    otree.Fill();

  }//input loop

  file.close();
  in.close();
  otree.Write();
  out_root.Close();

  return 0;

}

















