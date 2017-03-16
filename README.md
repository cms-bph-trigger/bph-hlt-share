```
cmsrel CMSSW_9_0_0_pre6
cd CMSSW_9_0_0_pre6/src/
cmsenv
git cms-addpkg  L1Trigger/L1TMuon
git cms-addpkg  EventFilter/L1TRawToDigi
git cms-addpkg  DataFormats/L1Trigger/
git cms-merge-topic 17853

git remote add l1repo git@github.com:cms-l1t-offline/cmssw.git
git fetch l1repo

git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2  L1Trigger/L1TMuon/src/MuonRawDigiTranslator.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/src/MicroGMTConfiguration.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/plugins/L1TMuonProducer.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/interface/MuonRawDigiTranslator.h
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/interface/MicroGMTConfiguration.h
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/python/gtStage2Raw_cfi.py
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/python/gmtStage2Raw_cfi.py
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/plugins/implementations_stage2/MuonUnpacker.h
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/plugins/implementations_stage2/MuonUnpacker.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/plugins/implementations_stage2/IntermediateMuonUnpacker.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/plugins/implementations_stage2/GTSetup.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 EventFilter/L1TRawToDigi/plugins/implementations_stage2/GMTSetup.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 DataFormats/L1Trigger/src/classes_def.xml
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2  DataFormats/L1Trigger/src/Muon.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2  DataFormats/L1Trigger/interface/Muon.h

git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/data
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/interface/MicroGMTIsolationUnit.h
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/interface/MicroGMTExtrapolationLUT.h
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/python/fakeGmtParams_cff.py
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/src/MicroGMTExtrapolationLUT.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/src/MicroGMTExtrapolationLUTFactory.cc
git checkout l1repo/l1t-integration-CMSSW_9_0_0_pre2 L1Trigger/L1TMuon/src/MicroGMTIsolationUnit.cc
git clone https://github.com/cms-l1t-offline/L1Trigger-L1TMuon.git L1Trigger/L1TMuon/data

mkdir NtupleTools
cd NtupleTools
git clone git@github.com:sarafiorendi/bph-hlt-share.git .

cd ..
scramv1 b
```

