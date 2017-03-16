import FWCore.ParameterSet.Config as cms

process = cms.Process("NTUPLE")

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.source    = cms.Source("PoolSource")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))


process.source.fileNames          = cms.untracked.vstring([
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/AODSIM/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/0493F56F-EDF0-E611-8684-001517FAD640.root',
])

process.source.secondaryFileNames = cms.untracked.vstring([
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/B0C8B5E9-D6F0-E611-A443-001517FB141C.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/A4DF6AFC-D6F0-E611-B645-A4BF0101DE18.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/BAD0DEEF-D6F0-E611-AB50-A4BF010122D7.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/C22442E5-D6F0-E611-8C90-001E67A406E0.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/EC9E6DC9-CDF0-E611-930D-001E67DFF5D7.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/F8244F0B-D7F0-E611-B459-001E67DDD348.root',
    'root://cms-xrd-global.cern.ch//store/mc/PhaseIFall16DR/BuToJpsiK_BFilter_TuneCUEP8M1_13TeV-pythia8-evtgen/GEN-SIM-RAW/FlatPU28to62HcalNZSRAW_81X_upgrade2017_realistic_v26-v1/110000/F8512CE8-D6F0-E611-847B-A4BF01025C16.root',
])

# Other statements
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag = '81X_upgrade2017_realistic_v26'



process.theNtuples = cms.EDAnalyzer("BHltNtuples",

                       puInfoTag                = cms.untracked.InputTag("addPileupInfo"),
                       offlineVtx               = cms.InputTag("offlinePrimaryVertices"),
                       beamspot                 = cms.InputTag("offlineBeamSpot"),
                       lumiScalerTag            = cms.untracked.InputTag("scalersRawToDigi"),

                       L1Candidates             = cms.untracked.InputTag("gmtStage2Digis", "Muon"), # from AOD
                       # L1Candidates             = cms.untracked.InputTag("gmtStage2Digis", "Muon", "RAW2DIGI"),  # if rerun L1 emulator only
                       # L1Candidates             = cms.untracked.InputTag("hltGtStage2Digis", "Muon"), # if rerun HLT

                       OfflineMuonsTag          = cms.untracked.InputTag("muons", "", "RECO"),
                       OfflineTkTag             = cms.untracked.InputTag("generalTracks", "", "RECO"),

                       trkMass                  = cms.untracked.double(0.493677),# kaon
                       displacedJpsi            = cms.untracked.bool(False),

                       maxEtaMu                 = cms.untracked.double(2.2),
                       minPtMu                  = cms.untracked.double(0.5),
                       maxEtaTrk                = cms.untracked.double(2.2),
                       minPtTrk                 = cms.untracked.double(0.5),
                       mind0Sign                = cms.untracked.double(0.),
                       doOffline                = cms.untracked.bool(True),
                       )   
                       

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("BHltNtuple.root"),
                                   closeFileFast = cms.untracked.bool(False)
                                   )

process.runAna = cms.EndPath(process.theNtuples)

