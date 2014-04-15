/*
  Calculator for substructure variables
  
  Author: Joshua Swanson, 2014
*/


#include <iostream>
#include "LJMet/Com/interface/BaseCalc.h"
#include "LJMet/Com/interface/LjmetFactory.h"
#include "LJMet/Com/interface/LjmetEventContent.h"
#include "LJMet/Com/interface/Nsubjettiness.hh"
#include "LJMet/Com/interface/Njettiness.hh"
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include "DataFormats/PatCandidates/interface/Jet.h"


#include "AnalysisDataFormats/TopObjects/interface/CATopJetTagInfo.h"

using std::cout;
using std::endl;
using namespace fastjet;

class LjmetFactory;

class JetSubCalc : public BaseCalc{
  
public:
  
  JetSubCalc();
  virtual ~JetSubCalc(){}
  
  virtual int BeginJob();
  virtual int AnalyzeEvent(edm::EventBase const & event, BaseEventSelector * selector);
  virtual int EndJob(){return 0;}

private:

  edm::InputTag             CA8TopJetColl_it;
  edm::InputTag             CA8PrunedJetColl_it;
  edm::InputTag             CA8JetColl_it;
  std::string               bDiscriminant;
  std::string				CA8TopTagInfo;

};


static int reg = LjmetFactory::GetInstance()->Register(new JetSubCalc(), "JetSubCalc");


JetSubCalc::JetSubCalc(){
}

int JetSubCalc::BeginJob(){

  if (mPset.exists("CA8TopJetColl")) CA8TopJetColl_it = mPset.getParameter<edm::InputTag>("CA8TopJetColl");
  else                               CA8TopJetColl_it = edm::InputTag("goodPatJetsCATopTagPFPacked");

  if (mPset.exists("CA8PrunedJetColl")) CA8PrunedJetColl_it = mPset.getParameter<edm::InputTag>("CA8PrunedJetColl");
  else                                  CA8PrunedJetColl_it = edm::InputTag("goodPatJetsCA8PrunedPFPacked");

  if (mPset.exists("CA8JetColl")) CA8JetColl_it = mPset.getParameter<edm::InputTag>("CA8JetColl");
  else                            CA8JetColl_it = edm::InputTag("goodPatJetsCA8PF");

  if (mPset.exists("bDiscriminant"))     bDiscriminant = mPset.getParameter<std::string>("bDiscriminant");
  else                                   bDiscriminant = "combinedSecondaryVertexBJetTags";

  if (mPset.exists("CA8TopTagInfo"))     CA8TopTagInfo = mPset.getParameter<std::string>("CA8TopTagInfo");
  else                                   CA8TopTagInfo = "CATop";

  return 0;
}

int JetSubCalc::AnalyzeEvent(edm::EventBase const & event,
			       BaseEventSelector * selector){

  
    //Get Top-like jets
    edm::Handle<std::vector<pat::Jet> > topJets;
    event.getByLabel(CA8TopJetColl_it, topJets);

    //Four vector
    std::vector <double> CATopJetPt;
    std::vector <double> CATopJetEta;
    std::vector <double> CATopJetPhi;
    std::vector <double> CATopJetEnergy;

    std::vector <double> CATopJetCSV;
    //std::vector <double> CATopJetRCN;

    //Identity
    std::vector <int> CATopJetIndex;
    std::vector <int> CATopJetnDaughters;

    //Top-like properties
    std::vector <double> CATopJetTopMass;
    std::vector <double> CATopJetMinPairMass;

    //Daughter four vector and index
    std::vector <double> CATopDaughterPt;
    std::vector <double> CATopDaughterEta;
    std::vector <double> CATopDaughterPhi;
    std::vector <double> CATopDaughterEnergy;

    std::vector <int> CATopDaughterMotherIndex;

    for (std::vector<pat::Jet>::const_iterator ijet = topJets->begin(); ijet != topJets->end(); ijet++){

      int index = (int)(ijet-topJets->begin());

      CATopJetPt     . push_back(ijet->pt());
      CATopJetEta    . push_back(ijet->eta());
      CATopJetPhi    . push_back(ijet->phi());
      CATopJetEnergy . push_back(ijet->energy());

      CATopJetCSV    . push_back(ijet->bDiscriminator( bDiscriminant ));
      //CATopJetRCN    . push_back((ijet->chargedEmEnergy()+ijet->chargedHadronEnergy()) / (ijet->neutralEmEnergy()+ijet->neutralHadronEnergy()));    

      CATopJetIndex      . push_back(index);
      CATopJetnDaughters . push_back((int)ijet->numberOfDaughters());

      reco::CATopJetTagInfo* jetInfo = (reco::CATopJetTagInfo*) ijet->tagInfo( CA8TopTagInfo );

      CATopJetTopMass     . push_back(jetInfo->properties().topMass);
      CATopJetMinPairMass . push_back(jetInfo->properties().minMass);

      for (size_t ui = 0; ui < ijet->numberOfDaughters(); ui++){
		CATopDaughterPt     . push_back(ijet->daughter(ui)->pt());
		CATopDaughterEta    . push_back(ijet->daughter(ui)->eta());
		CATopDaughterPhi    . push_back(ijet->daughter(ui)->phi());
		CATopDaughterEnergy . push_back(ijet->daughter(ui)->energy());        

		CATopDaughterMotherIndex . push_back(index);      
      }
    }

    //Four vector
    SetValue("CATopJetPt"     , CATopJetPt);
    SetValue("CATopJetEta"    , CATopJetEta);
    SetValue("CATopJetPhi"    , CATopJetPhi);
    SetValue("CATopJetEnergy" , CATopJetEnergy);

    SetValue("CATopJetCSV"    , CATopJetCSV);
    //SetValue("CATopJetRCN"    , CATopJetRCN);

    //Identity
    SetValue("CATopJetIndex"      , CATopJetIndex);
    SetValue("CATopJetnDaughters" , CATopJetnDaughters);

    //Properties
    SetValue("CATopJetTopMass"     , CATopJetTopMass);
    SetValue("CATopJetMinPairMass" , CATopJetMinPairMass);

    //Daughter four vector and index
    SetValue("CATopDaughterPt"     , CATopDaughterPt);
    SetValue("CATopDaughterEta"    , CATopDaughterEta);
    SetValue("CATopDaughterPhi"    , CATopDaughterPhi);
    SetValue("CATopDaughterEnergy" , CATopDaughterEnergy);

    SetValue("CATopDaughterMotherIndex"      , CATopDaughterMotherIndex);

    //Get CA8 jets for W's
    edm::Handle<std::vector<pat::Jet> > CAWJets;
    event.getByLabel(CA8PrunedJetColl_it, CAWJets);

    //Four vector
    std::vector <double> CAWJetPt;
    std::vector <double> CAWJetEta;
    std::vector <double> CAWJetPhi;
    std::vector <double> CAWJetEnergy;

    std::vector <double> CAWJetCSV;
    //std::vector <double> CAWJetRCN;

    //Identity
    std::vector <int> CAWJetIndex;
    std::vector <int> CAWJetnDaughters;

    //Mass
    std::vector <double> CAWJetMass;

    //Daughter four vector and index
    std::vector <double> CAWDaughterPt;
    std::vector <double> CAWDaughterEta;
    std::vector <double> CAWDaughterPhi;
    std::vector <double> CAWDaughterEnergy;

    std::vector <int> CAWDaughterMotherIndex;

    for (std::vector<pat::Jet>::const_iterator ijet = CAWJets->begin(); ijet != CAWJets->end(); ijet++){

      int index = (int)(ijet-CAWJets->begin());

      //Four vector
      CAWJetPt     . push_back(ijet->pt());
      CAWJetEta    . push_back(ijet->eta());
      CAWJetPhi    . push_back(ijet->phi());
      CAWJetEnergy . push_back(ijet->energy());        

      CAWJetCSV    . push_back(ijet->bDiscriminator( bDiscriminant ));
      //CAWJetRCN    . push_back((ijet->chargedEmEnergy()+ijet->chargedHadronEnergy()) / (ijet->neutralEmEnergy()+ijet->neutralHadronEnergy()));    

      //Identity
      CAWJetIndex      . push_back(index);
      CAWJetnDaughters . push_back((int)ijet->numberOfDaughters());

      //Mass
      CAWJetMass . push_back(ijet->mass());

      for (size_t ui = 0; ui < ijet->numberOfDaughters(); ui++){
		CAWDaughterPt     . push_back(ijet->daughter(ui)->pt());
		CAWDaughterEta    . push_back(ijet->daughter(ui)->eta());
		CAWDaughterPhi    . push_back(ijet->daughter(ui)->phi());
		CAWDaughterEnergy . push_back(ijet->daughter(ui)->energy());        

		CAWDaughterMotherIndex . push_back(index);      
      }
	}

    //Four vector
    SetValue("CAWJetPt"     , CAWJetPt);
    SetValue("CAWJetEta"    , CAWJetEta);
    SetValue("CAWJetPhi"    , CAWJetPhi);
    SetValue("CAWJetEnergy" , CAWJetEnergy);

    SetValue("CAWJetCSV"    , CAWJetCSV);
    //SetValue("CAWJetRCN"    , CAWJetRCN);

    //Identity
    SetValue("CAWJetIndex"      , CAWJetIndex);
    SetValue("CAWJetnDaughters" , CAWJetnDaughters);

    //Mass
    SetValue("CAWJetMass"     , CAWJetMass);

    //Daughter four vector and index
    SetValue("CAWDaughterPt"     , CAWDaughterPt);
    SetValue("CAWDaughterEta"    , CAWDaughterEta);
    SetValue("CAWDaughterPhi"    , CAWDaughterPhi);
    SetValue("CAWDaughterEnergy" , CAWDaughterEnergy);

    SetValue("CAWDaughterMotherIndex" , CAWDaughterMotherIndex);

    //Get all CA8 jets (not just for W and Top)
    edm::Handle<std::vector<pat::Jet> > CA8Jets;
    event.getByLabel(CA8JetColl_it, CA8Jets);

    //Four vector
    std::vector <double> CA8JetPt;
    std::vector <double> CA8JetEta;
    std::vector <double> CA8JetPhi;
    std::vector <double> CA8JetEnergy;
    std::vector <double> CA8JetMass;

    std::vector <double> CA8JetCSV;
    //std::vector <double> CA8JetRCN;
    std::vector <double> CA8Tau1;
    std::vector <double> CA8Tau2;
    std::vector <double> CA8Tau3;
    std::vector <double> CA8Tau4;
    std::vector <double> CA8Tau21;
    

  	// ---------------------------------------------------------------------------------------------------------
  	// Setup Nsubjettiness
  	// ---------------------------------------------------------------------------------------------------------

  	Nsubjettiness Nsubonepass1(1, Njettiness::AxesMode::onepass_kt_axes, 1.0, 0.8);
  	Nsubjettiness Nsubonepass2(2, Njettiness::AxesMode::onepass_kt_axes, 1.0, 0.8);
  	Nsubjettiness Nsubonepass3(3, Njettiness::AxesMode::onepass_kt_axes, 1.0, 0.8);
  	Nsubjettiness Nsubonepass4(4, Njettiness::AxesMode::onepass_kt_axes, 1.0, 0.8);

    for (std::vector<pat::Jet>::const_iterator ijet = CA8Jets->begin(); ijet != CA8Jets->end(); ijet++){

      //Four vector
      CA8JetPt     . push_back(ijet->pt());
      CA8JetEta    . push_back(ijet->eta());
      CA8JetPhi    . push_back(ijet->phi());
      CA8JetEnergy . push_back(ijet->energy());
      CA8JetMass   . push_back(ijet->mass());

      CA8JetCSV    . push_back(ijet->bDiscriminator( bDiscriminant ));
      //CA8JetRCN    . push_back((ijet->chargedEmEnergy()+ijet->chargedHadronEnergy()) / (ijet->neutralEmEnergy()+ijet->neutralHadronEnergy()));
      std::vector<fastjet::PseudoJet> FJparticles;
      for (unsigned i = 0; i < ijet->numberOfDaughters() ; i++){
      	const reco::PFCandidate* this_constituent = dynamic_cast<const reco::PFCandidate*>(ijet->daughter(i));
      	FJparticles.push_back( fastjet::PseudoJet( this_constituent->px(),
             	this_constituent->py(),
             	this_constituent->pz(),
             	this_constituent->energy() ) );
      }
	  
	  fastjet::PseudoJet combJet = fastjet::join(FJparticles);

	  CA8Tau1.push_back( Nsubonepass1.result(combJet) );
      CA8Tau2.push_back( Nsubonepass2.result(combJet) );
      CA8Tau3.push_back( Nsubonepass3.result(combJet) );
      CA8Tau4.push_back( Nsubonepass4.result(combJet) );
      CA8Tau21.push_back( Nsubonepass2.result(combJet) / Nsubonepass1.result(combJet) );
    
    }

    //Four vector
    SetValue("CA8JetPt"     , CA8JetPt);
    SetValue("CA8JetEta"    , CA8JetEta);
    SetValue("CA8JetPhi"    , CA8JetPhi);
    SetValue("CA8JetEnergy" , CA8JetEnergy);
    SetValue("CA8JetMass"	, CA8JetMass);

    SetValue("CA8JetCSV"    , CA8JetCSV);
    //SetValue("CA8JetRCN"    , CA8JetRCN);
    SetValue("CA8Tau1" , CA8Tau1);
    SetValue("CA8Tau2" , CA8Tau2);
    SetValue("CA8Tau3" , CA8Tau3);
    SetValue("CA8Tau4" , CA8Tau4);
    SetValue("CA8Tau21", CA8Tau21);

  	return 0;

}