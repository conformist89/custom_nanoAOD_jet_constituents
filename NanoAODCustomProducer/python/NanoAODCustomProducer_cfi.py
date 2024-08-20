import FWCore.ParameterSet.Config as cms

NanoAODCustomProducer = cms.EDProducer('NanoAODCustomProducer',
    jets = cms.InputTag('slimmedJetsAK8')
)
