void BHltNtuples::fillB    (const edm::Handle<reco::MuonCollection>       & muons ,
                            const edm::Handle<reco::TrackCollection >     & tracks,
                            const edm::Handle<reco::VertexCollection >    & vtxColl,
                            const edm::Event                              & event ,
                            const edm::EventSetup                         & eventSetup)
{

  const double trackMass2  = trackMass_ *trackMass_ ; 

  //get offline beamspot position
  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  event.getByToken(beamspotToken_,recoBeamSpotHandle);
  const reco::BeamSpot& vertexBeamSpot = *recoBeamSpotHandle;

  //get the transient track builder:
  edm::ESHandle<TransientTrackBuilder> theB;
  eventSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

  //get the b field
  std::string mfName_ = "";
  edm::ESHandle<MagneticField> bFieldHandle;
  eventSetup.get<IdealMagneticFieldRecord>().get(mfName_, bFieldHandle);  
  const MagneticField* magField = bFieldHandle.product();
  TSCBLBuilderNoMaterial blsBuilder;

  const reco::VertexCollection pvColl  = *(vtxColl.product())  ;    

  // Loop muon collection
  for(std::vector<reco::Muon>::const_iterator mu1=muons->begin(); mu1!=muons->end(); ++mu1) 
  { 
    if( muon::isSoftMuon( (*mu1), pvColl[0] ) && (*mu1).pt() > minPtMu_ && fabs( (*mu1).eta() ) < maxEtaMu_) 
    {
      // Look for the second muon
      for(std::vector<reco::Muon>::const_iterator mu2=mu1; mu2!=muons->end(); ++mu2) {
        if( mu2 == mu1 ) continue; 
        if( muon::isSoftMuon( (*mu2), pvColl[0]) && (*mu2).pt() > minPtMu_ && fabs( (*mu2).eta() ) < maxEtaMu_) 
        {
          if(!( mu1->charge() * mu2->charge() < 0 ))         continue; 
          hists_["mumuMass_all"]->Fill( (mu1->p4() + mu2->p4()).mass() );

          // do jpsi vertex fit
          std::vector<reco::TransientTrack> j_tks;
          j_tks.push_back((*theB).build(mu1->track().get()));
          j_tks.push_back((*theB).build(mu2->track().get()));
          if (j_tks.size()!=2) continue;
       
          KalmanVertexFitter jkvf;
          TransientVertex jtv = jkvf.vertex(j_tks);
          if (!jtv.isValid()) continue;
        
          reco::Vertex jpsivertex = jtv;
          float dimuonCL = 0;
          if( (jpsivertex.chi2()>=0.0) && (jpsivertex.ndof()>0) ) 
            dimuonCL = TMath::Prob(jpsivertex.chi2(), jpsivertex.ndof() );
            
          // calculate three-track transverse momentum
          math::XYZVector jpperp(mu1->px() + mu2->px() ,
                                 mu1->py() + mu2->py() ,
                                 0.);
         
          GlobalPoint jVertex = jtv.position();
          GlobalError jerr    = jtv.positionError();
          
          //calculate decay length  significance w.r.t. the beamspot
          GlobalPoint displacementFromBeamspotJpsi( -1*((vertexBeamSpot.x0() - jVertex.x()) + (jVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()), 
                                                    -1*((vertexBeamSpot.y0() - jVertex.y()) + (jVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()),
                                                     0);
          reco::Vertex::Point vperpj(displacementFromBeamspotJpsi.x(), displacementFromBeamspotJpsi.y(), 0.);

          float jpsi_ls  = displacementFromBeamspotJpsi.perp() /sqrt(jerr.rerr(displacementFromBeamspotJpsi));
          float jpsi_cos = vperpj.Dot(jpperp)/(vperpj.R()*jpperp.R());

          hists_["JpsiPt"  ]->Fill( jpperp.R() );
          hists_["JpsiLS"  ]->Fill( jpsi_ls    );
          hists_["JpsiCos" ]->Fill( jpsi_cos   );
          hists_["JpsiCL"  ]->Fill( dimuonCL   );
         
          // Loop on track collection - trk 1
          for (unsigned tracksIt =0 ;  tracksIt < tracks->size(); tracksIt++)
          {
            reco::Track itrk1       = tracks->at(tracksIt) ;                                                
            
            if (overlap(*mu1,itrk1))    continue;
            if (overlap(*mu2,itrk1))    continue;
            
            FreeTrajectoryState InitialFTS = initialFreeState(itrk1, magField);
            TrajectoryStateClosestToBeamLine tscb( blsBuilder(InitialFTS, *recoBeamSpotHandle) );
            float trk_d0sig = tscb.transverseImpactParameter().significance();

            hists_["trkPt"] -> Fill( itrk1.pt() );
            hists_["D0sig"] -> Fill( trk_d0sig );
 
            // eta and pt cut
            if (fabs(itrk1.eta()) > maxEtaTrk_ )                   continue;
            if (itrk1.pt()        < minPtTrk_  )                   continue;
            if (trk_d0sig         < mind0Sign_ )                   continue;
            if (! itrk1.quality(reco::TrackBase::highPurity))     continue;

            reco::Particle::LorentzVector pB, p1, p2, p3;
            // Combined system
            double e1   = sqrt(mu1->momentum().Mag2()  + MuMass2          );
            double e2   = sqrt(mu2->momentum().Mag2()  + MuMass2          );
            double e3   = sqrt(itrk1.momentum().Mag2() + trackMass2  );
            
            p1   = reco::Particle::LorentzVector(mu1->px() , mu1->py() , mu1->pz() , e1  );
            p2   = reco::Particle::LorentzVector(mu2->px() , mu2->py() , mu2->pz() , e2  );
            p3   = reco::Particle::LorentzVector(itrk1.px(), itrk1.py(), itrk1.pz(), e3  );
            pB   = p1 + p2 + p3;
            
            // do the vertex fit
            std::vector<reco::TransientTrack> t_tks;
            t_tks.push_back((*theB).build(mu1->track().get()));
            t_tks.push_back((*theB).build(mu2->track().get()));
            t_tks.push_back((*theB).build(&itrk1));
            if (t_tks.size()!=3) continue;
            
            KalmanVertexFitter kvf;
            TransientVertex tv  = kvf.vertex(t_tks);
            reco::Vertex vertex = tv;
            if (!tv.isValid()) continue;
            hists_["BInvMass"]->Fill( pB.mass() );

            float JpsiTkCL = 0;
            if ((vertex.chi2()>=0.0) && (vertex.ndof()>0) )   
              JpsiTkCL = TMath::Prob(vertex.chi2(), vertex.ndof() );
            
            // calculate four-track transverse momentum
            math::XYZVector pperp(mu1->px() + mu2->px() + itrk1.px(),
                                  mu1->py() + mu2->py() + itrk1.py(),
                                  0.);
            // get vertex position and error to calculate the decay length significance
            GlobalPoint secondaryVertex = tv.position();
            GlobalError err             = tv.positionError();
            GlobalPoint displacementFromBeamspot( -1*((vertexBeamSpot.x0() - secondaryVertex.x()) + 
                                                      (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()), 
                                                  -1*((vertexBeamSpot.y0() - secondaryVertex.y()) + 
                                                      (secondaryVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()), 
                                                  0);
            reco::Vertex::Point vperp(displacementFromBeamspot.x(),displacementFromBeamspot.y(),0.);
            

            BCand theB;
            
            theB.Mu1Pt    = mu1 -> pt( );
            theB.Mu1Eta   = mu1 -> eta();
            theB.Mu1Phi   = mu1 -> phi();
            theB.Mu1Ch    = mu1 -> charge();
            theB.Mu2Pt    = mu2 -> pt( );
            theB.Mu2Eta   = mu2 -> eta();
            theB.Mu2Phi   = mu2 -> phi();
            theB.Mu2Ch    = mu2 -> charge();

            theB.MuMuMass = (mu1->p4() + mu2->p4()).mass();
            theB.MuMuCL   = dimuonCL;

            theB.JpsiL          = displacementFromBeamspotJpsi.perp();
            theB.JpsiSigma      = sqrt(jerr.rerr(displacementFromBeamspotJpsi));
            theB.JpsiPt         = jpperp.R();
            theB.JpsiCosBS      = jpsi_cos;
         
            theB.TrkPt     = itrk1.pt( ) ;
            theB.TrkEta    = itrk1.eta() ;
            theB.TrkPhi    = itrk1.phi() ;
            theB.Trkd0Sign = trk_d0sig  ;
            theB.TrkCh     = itrk1.charge()  ;
            (itrk1.quality(reco::TrackBase::highPurity)) ? theB.TrkHQ = 1 : theB.TrkHQ = 0;

            theB.BMass         = pB.mass()        ;
            theB.BPt           = pB.pt()          ;
            theB.BEta          = pB.eta()         ;
            theB.BPhi          = pB.phi()         ;
            
            theB.JpsiTkCL         = JpsiTkCL;
            theB.JpsiTkL          = displacementFromBeamspot.perp();
            theB.JpsiTkSigma      = sqrt(err.rerr(displacementFromBeamspot));
            theB.JpsiTkCosBS      = vperp.Dot(pperp)/(vperp.R()*pperp.R());
            event_.bcands.push_back(theB);

          }
        }
      }
    }
  }
}





// ---------------------------------------------------------------------
void BHltNtuples::fillMuMu (const edm::Handle<reco::MuonCollection>       & muons ,
                            const edm::Handle<reco::VertexCollection >    & vtxColl,
                            const edm::Event                              & event ,
                            const edm::EventSetup                         & eventSetup)
{

  //get offline beamspot position
  edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
  event.getByToken(beamspotToken_,recoBeamSpotHandle);
  const reco::BeamSpot& vertexBeamSpot = *recoBeamSpotHandle;

  //get the transient track builder:
  edm::ESHandle<TransientTrackBuilder> theB;
  eventSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

  const reco::VertexCollection pvColl  = *(vtxColl.product())  ;    

  // Loop muon collection
  for(std::vector<reco::Muon>::const_iterator mu1=muons->begin(); mu1!=muons->end(); ++mu1) 
  { 
    if( muon::isSoftMuon( (*mu1), pvColl[0] ) && (*mu1).pt() > minPtMu_ && fabs( (*mu1).eta() ) < maxEtaMu_) 
    {
      // Look for the second muon
      for(std::vector<reco::Muon>::const_iterator mu2=mu1; mu2!=muons->end(); ++mu2) {
        if( mu2 == mu1 ) continue; 
        if( muon::isSoftMuon( (*mu2), pvColl[0]) && (*mu2).pt() > minPtMu_ && fabs( (*mu2).eta() ) < maxEtaMu_) 
        {
          if(!( mu1->charge() * mu2->charge() < 0 ))         continue; 

          // do jpsi vertex fit
          std::vector<reco::TransientTrack> j_tks;
          j_tks.push_back((*theB).build(mu1->track().get()));
          j_tks.push_back((*theB).build(mu2->track().get()));
          if (j_tks.size()!=2) continue;
       
          KalmanVertexFitter jkvf;
          TransientVertex jtv = jkvf.vertex(j_tks);
          if (!jtv.isValid()) continue;
        
          reco::Vertex jpsivertex = jtv;
          float dimuonCL = 0;
          if( (jpsivertex.chi2()>=0.0) && (jpsivertex.ndof()>0) ) 
            dimuonCL = TMath::Prob(jpsivertex.chi2(), jpsivertex.ndof() );
            
          // calculate three-track transverse momentum
          math::XYZVector jpperp(mu1->px() + mu2->px() ,
                                 mu1->py() + mu2->py() ,
                                 0.);
         
          GlobalPoint jVertex = jtv.position();
          GlobalError jerr    = jtv.positionError();
          
          //calculate decay length  significance w.r.t. the beamspot
          GlobalPoint displacementFromBeamspotJpsi( -1*((vertexBeamSpot.x0() - jVertex.x()) + (jVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dxdz()), 
                                                    -1*((vertexBeamSpot.y0() - jVertex.y()) + (jVertex.z() - vertexBeamSpot.z0()) * vertexBeamSpot.dydz()),
                                                     0);
          reco::Vertex::Point vperpj(displacementFromBeamspotJpsi.x(), displacementFromBeamspotJpsi.y(), 0.);

          float jpsi_cos = vperpj.Dot(jpperp)/(vperpj.R()*jpperp.R());
         
          MuMuCand theDiMu;
          
          theDiMu.Mu1Pt    = mu1 -> pt( );
          theDiMu.Mu1Eta   = mu1 -> eta();
          theDiMu.Mu1Phi   = mu1 -> phi();
          theDiMu.Mu1Ch    = mu1 -> charge();
          theDiMu.Mu2Pt    = mu2 -> pt( );
          theDiMu.Mu2Eta   = mu2 -> eta();
          theDiMu.Mu2Phi   = mu2 -> phi();
          theDiMu.Mu2Ch    = mu2 -> charge();

          theDiMu.MuMuMass = (mu1->p4() + mu2->p4()).mass();
          theDiMu.MuMuCL   = dimuonCL;

          theDiMu.JpsiL          = displacementFromBeamspotJpsi.perp();
          theDiMu.JpsiSigma      = sqrt(jerr.rerr(displacementFromBeamspotJpsi));
          theDiMu.JpsiPt         = jpperp.R();
          theDiMu.JpsiCosBS      = jpsi_cos;

          event_.mumucands.push_back(theDiMu);
        }
      }
    }
  }
}



