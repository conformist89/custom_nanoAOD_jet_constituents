#ifndef NanoAODCustomProducer_h
#define NanoAODCustomProducer_h

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <vector>
#include <memory>

class NanoAODCustomProducer : public edm::global::EDProducer<> {
public:
    explicit NanoAODCustomProducer(const edm::ParameterSet& iConfig);
    ~NanoAODCustomProducer() override = default;

    void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const override;

private:
    edm::EDGetTokenT<pat::JetCollection> jetToken_;
    edm::EDPutTokenT<pat::JetCollection> jetTokenOut_;
};

#endif

