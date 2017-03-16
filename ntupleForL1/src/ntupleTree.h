#ifndef  ntupleTree_h
#define  ntupleTree_h

#include "TROOT.h"
#include "TMath.h"
#include <vector>
#include <string>



class BCand {
public:

  Float_t  Mu1Pt  ;  
  Float_t  Mu2Pt  ;  
  Float_t  Mu1Eta ;  
  Float_t  Mu1Phi ;  
  Float_t  Mu2Eta ;  
  Float_t  Mu2Phi ;  
  Int_t    Mu1Ch  ;  
  Int_t    Mu2Ch  ;  

  Float_t MuMuMass       ;  
  Float_t MuMuCL         ;  
  Float_t JpsiPt         ;  
  Float_t JpsiL          ;  
  Float_t JpsiSigma      ;  
  Float_t JpsiCosBS      ;  

  Float_t TrkPt          ;  
  Float_t TrkEta         ;  
  Float_t TrkPhi         ;  
  Float_t TrkHQ          ;  
  Float_t Trkd0Sign      ;  
  Int_t   TrkCh          ;  

  Float_t BMass          ;  
  Float_t BPt            ;  
  Float_t BEta           ;  
  Float_t BPhi           ;  

  Float_t JpsiTkCL                ;  
  Float_t JpsiTkL                 ;  
  Float_t JpsiTkSigma             ;  
  Float_t JpsiTkCosBS             ;  

  BCand(){};
  virtual ~BCand(){};

  ClassDef(BCand,1)
};



class MuMuCand {
public:

  Float_t  Mu1Pt  ;  
  Float_t  Mu2Pt  ;  
  Float_t  Mu1Eta ;  
  Float_t  Mu1Phi ;  
  Float_t  Mu2Eta ;  
  Float_t  Mu2Phi ;  
  Int_t    Mu1Ch  ;  
  Int_t    Mu2Ch  ;  

  Float_t MuMuMass       ;  
  Float_t MuMuCL         ;  
  Float_t JpsiPt         ;  
  Float_t JpsiL          ;  
  Float_t JpsiSigma      ;  
  Float_t JpsiCosBS      ;  

  MuMuCand(){};
  virtual ~MuMuCand(){};

  ClassDef(MuMuCand,1)
};



class L1MuonCand {
public:

  Float_t pt;           
  Float_t eta;          
  Float_t phi;          
  Float_t etaAtVtx;          
  Float_t phiAtVtx;          
  Int_t   charge;      
  Int_t   quality;      
  
  L1MuonCand(){};
  virtual ~L1MuonCand(){};

  ClassDef(L1MuonCand,1)

};



class ntupleEvent {
public:

  Int_t   runNumber;             
  Int_t   luminosityBlockNumber; 
  Int_t   eventNumber;           

  Int_t   nVtx;                    
  Float_t trueNI;   

  Float_t bxId;
  Float_t instLumi; 
  
  Float_t primaryVertex[3];        
  Float_t cov_primaryVertex[3][3]; 

  std::vector <BCand>               bcands;         
  std::vector <MuMuCand>            mumucands;         

  std::vector <L1MuonCand>          L1muons;      

  ntupleEvent(){};
  virtual ~ntupleEvent(){};

  ClassDef(ntupleEvent,1)
};


#endif

