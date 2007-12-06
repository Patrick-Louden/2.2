/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 */

#include "applications/dynamicProps/DirectionalRCorrFunc.hpp"

namespace oopse {
  DirectionalRCorrFunc::DirectionalRCorrFunc(SimInfo* info, const std::string& filename, const std::string& sele1, const std::string& sele2)
    : ParticleTimeCorrFunc(info, filename, sele1, sele2, DataStorage::dslPosition){

      setCorrFuncType("DirectionalRCorrFunc");
      setOutputName(getPrefix(dumpFilename_) + ".drcorr");
      histogram_.resize(nTimeBins_); 
      count_.resize(nTimeBins_);
      nSelected_ =   seleMan1_.getSelectionCount();  
      assert(  nSelected_ == seleMan2_.getSelectionCount());
    }

  void DirectionalRCorrFunc::correlateFrames(int frame1, int frame2) {
    Snapshot* snapshot1 = bsMan_->getSnapshot(frame1);
    Snapshot* snapshot2 = bsMan_->getSnapshot(frame2);
    assert(snapshot1 && snapshot2);

    RealType time1 = snapshot1->getTime();
    RealType time2 = snapshot2->getTime();

    int timeBin = int ((time2 - time1) /deltaTime_ + 0.5);
    count_[timeBin] += nSelected_;    

    int i;
    int j;
    StuntDouble* sd1;
    StuntDouble* sd2;

    for (sd1 = seleMan1_.beginSelected(i), sd2 = seleMan2_.beginSelected(j);
	 sd1 != NULL && sd2 != NULL;
	 sd1 = seleMan1_.nextSelected(i), sd2 = seleMan2_.nextSelected(j)) {

      Vector3d corrVals = calcCorrVals(frame1, frame2, sd1, sd2);
      histogram_[timeBin] += corrVals; 
    }
    
  }

  void DirectionalRCorrFunc::postCorrelate() {
    for (int i =0 ; i < nTimeBins_; ++i) {
      if (count_[i] > 0) {
        histogram_[i] /= count_[i];
      }
    }
  }

  void DirectionalRCorrFunc::preCorrelate() {
    // Fill the histogram with empty Vector3d:
    std::fill(histogram_.begin(), histogram_.end(), Vector3d(0.0));
    // count array set to zero
    std::fill(count_.begin(), count_.end(), 0);
  }


  Vector3d DirectionalRCorrFunc::calcCorrVals(int frame1, int frame2, StuntDouble* sd1, StuntDouble* sd2) {
    Vector3d r1 = sd1->getPos(frame1);
    Vector3d r2 = sd2->getPos(frame2);
    Vector3d u1 = sd1->getA(frame1).getColumn(2);

    RealType rsq = (r2-r1).lengthSquare();
    RealType rpar = dot( r2-r1, u1);
    RealType rpar2 = rpar*rpar;
    RealType rperp2 = rsq - rpar2;

    return Vector3d(rsq, rpar2, rperp2);
  }

  void DirectionalRCorrFunc::writeCorrelate() {
    std::ofstream ofs(getOutputFileName().c_str());

    if (ofs.is_open()) {

      ofs << "#" << getCorrFuncType() << "\n";
      ofs << "#time\tr2\trparallel\trperpendicular\n";

      for (int i = 0; i < nTimeBins_; ++i) {
        ofs << time_[i] << "\t" << 
          histogram_[i](0) << "\t" <<
          histogram_[i](1) << "\t" <<
          histogram_[i](2) << "\t" << "\n";
      }
            
    } else {
      sprintf(painCave.errMsg,
              "DirectionalRCorrFunc::writeCorrelate Error: fail to open %s\n", getOutputFileName().c_str());
      painCave.isFatal = 1;
      simError();        
    }
    
    ofs.close();    
  }


}