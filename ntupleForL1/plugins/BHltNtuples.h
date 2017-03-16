/** \class BHltNtuples
 */      
// #include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/Common/interface/Handle.h"
// #include "DataFormats/Common/interface/TriggerResults.h"
// #include "DataFormats/HLTReco/interface/TriggerEvent.h"
// #include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/Scalers/interface/LumiScalers.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"


#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
// #include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"



#include <map>
#include <string>
#include <iomanip>
#include "TTree.h"

#include "NtupleTools/ntupleForL1/src/ntupleTree.h"


// class BHltNtuples : public edm::EDAnalyzer {
class BHltNtuples : public edm::one::EDAnalyzer<> {

 public:
  BHltNtuples(const edm::ParameterSet& cfg);
  virtual ~BHltNtuples() {};

  virtual void beginJob();
  /// everything that needs to be done after the event loop
  virtual void endJob();
  /// everything that needs to be done before each run
  virtual void beginRun(const edm::Run & run, const edm::EventSetup & eventSetup);
  /// everything that needs to be done after each run
  virtual void endRun(const edm::Run & run, const edm::EventSetup & eventSetup);
  /// everything that needs to be done during the event loop
  virtual void analyze(const edm::Event& event, const edm::EventSetup& eventSetup);

  virtual void beginEvent();

 private:


  void fillMuMu     (const edm::Handle<reco::MuonCollection> &,
                     const edm::Handle<reco::VertexCollection >& ,
                     const edm::Event      &, 
                     const edm::EventSetup & 
                    );
  void fillB        (const edm::Handle<reco::MuonCollection> &,
                     const edm::Handle<reco::TrackCollection> &,
                     const edm::Handle<reco::VertexCollection >& ,
                     const edm::Event      &, 
                     const edm::EventSetup & 
                    );

  void fillL1Muons  (const edm::Handle<l1t::MuonBxCollection> &,
                     const edm::Event   &
                    );


  int                       overlap            ( const reco::Candidate&, const reco::Track    ) ;
  FreeTrajectoryState       initialFreeState   ( const reco::Track&,     const MagneticField* ) ;
  std::pair<double,double>  pionIPBeamSpot     ( reco::TransientTrack,   GlobalPoint          ) ;
  std::pair<double,double>  pionImpactParameter(reco::TransientTrack,    TransientVertex      ) ;
  
  /// file service
  edm::Service<TFileService> outfile_;

  edm::InputTag puTag_;
  edm::EDGetTokenT<std::vector< PileupSummaryInfo>> puToken_;
  edm::InputTag offlinePVTag_;
  edm::EDGetTokenT<reco::VertexCollection> offlinePVToken_;
  edm::InputTag beamspotTag_;
  edm::EDGetTokenT<reco::BeamSpot> beamspotToken_;
  edm::InputTag lumiScalerTag_;
  edm::EDGetTokenT<LumiScalersCollection> lumiScalerToken_; 
 
  edm::InputTag l1candTag_;
  edm::EDGetTokenT<l1t::MuonBxCollection> l1candToken_; 
  
  edm::InputTag offlineMuonsTag_;
  edm::EDGetTokenT<reco::MuonCollection> offlineMuonsToken_;
  edm::InputTag offlineTksTag_;
  edm::EDGetTokenT<reco::TrackCollection> offlineTksToken_;

  ntupleEvent event_;
  std::map<std::string,TTree*> outTree_;

  /// histograms
  std::map<std::string, TH1*> hists_;
  
  unsigned int nGoodVtx; 

  const double MuMass = 0.106;
  const double MuMass2 = MuMass*MuMass;

  // Services
  edm::ESHandle<MagneticField>         magneticField_;
  edm::ESHandle<Propagator>            propagator_;
  
  bool debug_;

  double trackMass_;
  bool   doOffline_;

  double maxEtaMu_ ;
  double minPtMu_  ;
  
  double maxEtaTrk_;
  double minPtTrk_ ;
  double mind0Sign_;
  
};


void BHltNtuples::endJob() {}

void BHltNtuples::beginRun(const edm::Run & run, const edm::EventSetup & eventSetup) {}

void BHltNtuples::endRun  (const edm::Run & run, const edm::EventSetup & eventSetup) {}

//---------------------------------------------
void BHltNtuples::beginEvent()
{

  event_.bcands.clear();
  event_.mumucands.clear();
  event_.L1muons.clear();

  event_.nVtx       = -1;
  event_.trueNI     = -1;
  
  nGoodVtx = 0; 
}

