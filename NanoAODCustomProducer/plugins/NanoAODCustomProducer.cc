#include "JetCust/NanoAODCustomProducer/plugins/NanoAODCustomProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"  // Ensure this is included
#include <iostream>  // For debugging output

NanoAODCustomProducer::NanoAODCustomProducer(const edm::ParameterSet& iConfig) :
    jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"))),
    jetTokenOut_(produces<pat::JetCollection>()) {}

void NanoAODCustomProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
    edm::Handle<pat::JetCollection> jets;
    iEvent.getByToken(jetToken_, jets);

    auto outputJets = std::make_unique<pat::JetCollection>();

    auto constituentTable = std::make_unique<nanoaod::FlatTable>(jets->size(), "constituents", false);

    std::vector<float> constituentPt;
    std::vector<float> constituentEta;
    std::vector<float> constituentPhi;
    std::vector<float> constituentMass;
    std::vector<float> constituentCharge;
    std::vector<int> constituentPdgId;

    for (const auto& jet : *jets) {
        pat::Jet newJet = jet;

        for (unsigned i = 0; i < jet.numberOfDaughters(); ++i) {
            const reco::Candidate* constituent = jet.daughter(i);
            constituentPt.push_back(constituent->pt());
            constituentEta.push_back(constituent->eta());
            constituentPhi.push_back(constituent->phi());
            constituentMass.push_back(constituent->mass());
            constituentCharge.push_back(constituent->charge());
            constituentPdgId.push_back(constituent->pdgId());

            // Debugging output
            std::cout << "Added constituent with pt: " << constituent->pt() 
                      << ", eta: " << constituent->eta()
                      << ", phi: " << constituent->phi()
                      << ", mass: " << constituent->mass() 
                      << ", charge: " << constituent->charge() 
                      << ", pdgId: " << constituent->pdgId() << std::endl;
        }

        // Check that all vectors have the same size after processing each jet
        if (constituentPt.size() != constituentEta.size() ||
            constituentPt.size() != constituentPhi.size() ||
            constituentPt.size() != constituentMass.size() ||
            constituentPt.size() != constituentCharge.size() ||
            constituentPt.size() != constituentPdgId.size()) {
            throw cms::Exception("LogicError") << "Mismatched vector sizes after processing a jet";
        }

        outputJets->push_back(newJet);
    }

    // Add columns to the table
    constituentTable->addColumn<float>("pt", std::move(constituentPt), "pT of the constituent");
    constituentTable->addColumn<float>("eta", std::move(constituentEta), "eta of the constituent");
    constituentTable->addColumn<float>("phi", std::move(constituentPhi), "phi of the constituent");
    constituentTable->addColumn<float>("mass", std::move(constituentMass), "mass of the constituent");
    constituentTable->addColumn<float>("charge", std::move(constituentCharge), "charge of the constituent");
    constituentTable->addColumn<int>("pdgId", std::move(constituentPdgId), "PDG ID of the constituent");

    iEvent.put(jetTokenOut_, std::move(outputJets));
    iEvent.put(std::move(constituentTable));
}

// Correctly define the module
DEFINE_FWK_MODULE(NanoAODCustomProducer);




