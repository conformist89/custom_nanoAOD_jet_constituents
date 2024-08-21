#include "JetCust/NanoAODCustomProducer/plugins/NanoAODCustomProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include <iostream>  // For debugging output

NanoAODCustomProducer::NanoAODCustomProducer(const edm::ParameterSet& iConfig) :
    jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"))),
    jetTokenOut_(produces<pat::JetCollection>()),
    tableTokenOut_(produces<nanoaod::FlatTable>())  // Register the FlatTable product
{}

void NanoAODCustomProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
    edm::Handle<pat::JetCollection> jets;
    iEvent.getByToken(jetToken_, jets);

    auto outputJets = std::make_unique<pat::JetCollection>();


    std::vector<float> constituentPt;
    std::vector<float> constituentEta;
    std::vector<float> constituentPhi;
    std::vector<float> constituentMass;
    std::vector<float> constituentCharge;
    std::vector<int> constituentPdgId;
    std::vector<int> jetIndex;  // To keep track of which jet the constituent belongs to

    int jetCounter = 0;

    for (const auto& jet : *jets) {
        pat::Jet newJet = jet;
        jetCounter++;  // Increment the counter for each jet

        if (jet.numberOfDaughters() == 0) {
            // If the jet has no constituents, add default [0] entries
            std::cerr << "Warning: Jet " << jetCounter << " has no constituents." << std::endl;

            constituentPt.push_back(0);
            constituentEta.push_back(0);
            constituentPhi.push_back(0);
            constituentMass.push_back(0);
            constituentCharge.push_back(0);
            constituentPdgId.push_back(0);
            jetIndex.push_back(jetCounter);
        } else {
            for (unsigned i = 0; i < jet.numberOfDaughters(); ++i) {
                const reco::Candidate* constituent = jet.daughter(i);
                constituentPt.push_back(constituent->pt());
                constituentEta.push_back(constituent->eta());
                constituentPhi.push_back(constituent->phi());
                constituentMass.push_back(constituent->mass());
                constituentCharge.push_back(constituent->charge());
                constituentPdgId.push_back(constituent->pdgId());
                jetIndex.push_back(jetCounter);

                // Debugging output
                std::cout << "Jet " << jetCounter 
                          << ": Added constituent with pt: " << constituent->pt() 
                          << ", eta: " << constituent->eta()
                          << ", phi: " << constituent->phi()
                          << ", mass: " << constituent->mass() 
                          << ", charge: " << constituent->charge() 
                          << ", pdgId: " << constituent->pdgId() << std::endl;
            }
        }

        outputJets->push_back(newJet);
    }

    // Print the number of jets processed
    std::cout << "Total number of jets in collection: " << jetCounter << std::endl;

    size_t nConstituents = constituentPt.size();

    // Initialize the FlatTable with the correct size
    auto constituentTable = std::make_unique<nanoaod::FlatTable>(nConstituents, "constituents", false);


    // Debugging outputs to check vector sizes before adding to FlatTable
    std::cout << "Final vector sizes: " << std::endl;
    std::cout << "pt size: " << constituentPt.size()
              << ", eta size: " << constituentEta.size()
              << ", phi size: " << constituentPhi.size()
              << ", mass size: " << constituentMass.size()
              << ", charge size: " << constituentCharge.size()
              << ", pdgId size: " << constituentPdgId.size()
              << ", jetIndex size: " << jetIndex.size() << std::endl;

    if (constituentPt.size() != constituentEta.size() ||
        constituentPt.size() != constituentPhi.size() ||
        constituentPt.size() != constituentMass.size() ||
        constituentPt.size() != constituentCharge.size() ||
        constituentPt.size() != constituentPdgId.size() ||
        constituentPt.size() != jetIndex.size()) {
        throw cms::Exception("LogicError") << "Mismatched vector sizes before adding to FlatTable";
    }

    // // Add columns to the table
    constituentTable->addColumn<float>("constituent_pt", std::move(constituentPt), "pT of the constituent");
    constituentTable->addColumn<float>("constituent_eta", std::move(constituentEta), "eta of the constituent");
    constituentTable->addColumn<float>("constituent_phi", std::move(constituentPhi), "phi of the constituent");
    constituentTable->addColumn<float>("constituent_mass", std::move(constituentMass), "mass of the constituent");
    constituentTable->addColumn<float>("constituent_charge", std::move(constituentCharge), "charge of the constituent");
    constituentTable->addColumn<int>("constituent_pdgId", std::move(constituentPdgId), "PDG ID of the constituent");
    constituentTable->addColumn<int>("constituent_jetIndex", std::move(jetIndex), "Index of the associated jet");

    iEvent.put(jetTokenOut_, std::move(outputJets));
    iEvent.put(std::move(constituentTable));
}

DEFINE_FWK_MODULE(NanoAODCustomProducer);





