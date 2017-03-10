/** \class BHltNtuples
 */
      
#include "BHltNtuples.h"
#include "BHltNtuples_utils.h"
#include "BHltNtuples_fillhltB.h"
#include "BHltNtuples_fillB.h"


/// default constructor
BHltNtuples::BHltNtuples(const edm::ParameterSet& cfg): 

  puTag_                  (cfg.getUntrackedParameter<edm::InputTag>("puInfoTag")),
    puToken_                (consumes<std::vector< PileupSummaryInfo>>(puTag_)), 
  offlinePVTag_           (cfg.getParameter<edm::InputTag>("offlineVtx")), 
    offlinePVToken_         (consumes<reco::VertexCollection>(offlinePVTag_)), 
  beamspotTag_            (cfg.getParameter<edm::InputTag>("beamspot")), 
    beamspotToken_         (consumes<reco::BeamSpot>(beamspotTag_)), 
  lumiScalerTag_          (cfg.getUntrackedParameter<edm::InputTag>("lumiScalerTag")),
    lumiScalerToken_        (consumes<LumiScalersCollection>(lumiScalerTag_)),   

  l1candTag_              (cfg.getUntrackedParameter<edm::InputTag>("L1Candidates")),
    l1candToken_            (consumes<l1t::MuonBxCollection>(l1candTag_)),

  offlineMuonsTag_        (cfg.getUntrackedParameter<edm::InputTag>("OfflineMuonsTag")),
    offlineMuonsToken_      (consumes<reco::MuonCollection>(offlineMuonsTag_)), 
  offlineTksTag_          (cfg.getUntrackedParameter<edm::InputTag>("OfflineTkTag")),
    offlineTksToken_        (consumes<reco::TrackCollection>(offlineTksTag_)), 

  trackMass_              (cfg.getUntrackedParameter<double>("trkMass")),  
  doOffline_              (cfg.getUntrackedParameter<bool>("doOffline")),

  maxEtaMu_               (cfg.getUntrackedParameter<double>("maxEtaMu")), 
  minPtMu_                (cfg.getUntrackedParameter<double>("minPtMu")),
  maxEtaTrk_              (cfg.getUntrackedParameter<double>("maxEtaTrk")), 
  minPtTrk_               (cfg.getUntrackedParameter<double>("minPtTrk")),
  mind0Sign_              (cfg.getUntrackedParameter<double>("mind0Sign"))

{
}


void BHltNtuples::beginJob() {

  TH1::SetDefaultSumw2() ;
  outTree_["ntupleTree"] = outfile_-> make<TTree>("ntupleTree","ntupleTree");
  outTree_["ntupleTree"] -> Branch("event" ,&event_, 64000,2);

  hists_["countEvents"  ] = outfile_->make<TH1F>("countEvents" , "countEvents"              ,    4,     0.,    4 );
  hists_["mumuMass_all" ] = outfile_->make<TH1F>("mumuMass_all", "mass"                     , 2000,     0.,   20 ); 

  hists_["JpsiPt"       ] = outfile_->make<TH1F>("JpsiPt"      , "mass"                     ,  400,     0.,   40 ); 
  hists_["JpsiLS"       ] = outfile_->make<TH1F>("JpsiLS"      , "mass"                     ,  400,     0.,   40 ); 
  hists_["JpsiCos"      ] = outfile_->make<TH1F>("JpsiCos"     , "mass"                     , 2000,    -1.,    1 ); 
  hists_["JpsiCL"       ] = outfile_->make<TH1F>("JpsiCL"      , "mass"                     , 1000,     0.,    1 ); 

  hists_["trkPt"        ] = outfile_->make<TH1F>("trkPt"       , "pt trk"                   ,  150,     0.,   15 );
  hists_["onlineTrkPt"  ] = outfile_->make<TH1F>("onlineTrkPt" , "pt onl trk"               ,  150,     0.,   15 );
  hists_["D0sig"        ] = outfile_->make<TH1F>("D0sig"       , ""                         ,  600,    -1.,    5 ); 
  hists_["BInvMass"     ] = outfile_->make<TH1F>("BInvMass"    , "BInvMass"                 , 2000,     0.,   20.);

}    



 
void BHltNtuples::analyze (const edm::Event &event, const edm::EventSetup &eventSetup) {

  beginEvent();
  
  // Fill general info
  event_.runNumber             = event.id().run();
  event_.luminosityBlockNumber = event.id().luminosityBlock();
  event_.eventNumber           = event.id().event();

  // Fill PU info
  if (!event.isRealData()) {
    edm::Handle<std::vector< PileupSummaryInfo > > puInfo;
    if ( event.getByToken(puToken_,puInfo)){
      std::vector<PileupSummaryInfo>::const_iterator PVI;
      for(PVI = puInfo->begin(); PVI != puInfo->end(); ++PVI) 
      {
        if(PVI->getBunchCrossing()==0){
          event_.trueNI   = PVI->getTrueNumInteractions();
          continue;
        }
      }
    } 
    else  
      edm::LogError("") << "PU collection not found !!!";
  }


  if (doOffline_){
    
    // Fill vertex info
    edm::Handle<reco::VertexCollection> vtxColl; 
    event.getByToken(offlinePVToken_, vtxColl);
    for(reco::VertexCollection::const_iterator it = vtxColl->begin(); it != vtxColl->end(); ++it) {
      if( !it->isValid())  continue;
      nGoodVtx++;
    }
    event_.nVtx = nGoodVtx;

	// Fill bx and inst lumi info
	if (event.isRealData()) {
	  event_.bxId  = event.bunchCrossing();

	  if (lumiScalerTag_.label() != "none")
	  {
	    edm::Handle<LumiScalersCollection> lumiScaler;
	    event.getByToken(lumiScalerToken_, lumiScaler);

  	    if (lumiScaler->begin() != lumiScaler->end())
		  event_.instLumi = lumiScaler->begin()->instantLumi();
	  }
	}
    // Handle the offline collections 
    edm::Handle<reco::MuonCollection> muons;
    event.getByToken(offlineMuonsToken_,  muons);
    edm::Handle<reco::TrackCollection >   tracks;
    event.getByToken(offlineTksToken_,    tracks);
    
    // fill the dimuon+trk collection
    fillB(muons, tracks, vtxColl, event, eventSetup );

    // fill the dimuon candidate collection
    fillMuMu(muons, vtxColl, event, eventSetup );

  }
  
  // Handle the L1 muon collection and fill L1 muons
  edm::Handle<l1t::MuonBxCollection> l1cands;
  if (event.getByToken(l1candToken_, l1cands))
    fillL1Muons(l1cands, event);
  else
    edm::LogWarning("") << "Online L1 muon collection not found !!!";

  outTree_["ntupleTree"] -> Fill();
}



// define this as a plug-in
DEFINE_FWK_MODULE(BHltNtuples);
