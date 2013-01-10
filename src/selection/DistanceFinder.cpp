/*
 * Copyright (c) 2005, 2010 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
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
 *
 * SUPPORT OPEN SCIENCE!  If you use OpenMD or its source code in your
 * research, please cite the appropriate papers when you publish your
 * work.  Good starting points are:
 *                                                                      
 * [1]  Meineke, et al., J. Comp. Chem. 26, 252-271 (2005).             
 * [2]  Fennell & Gezelter, J. Chem. Phys. 124, 234104 (2006).          
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 24107 (2008).          
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

#include "selection/DistanceFinder.hpp"
#include "primitives/Molecule.hpp"
#ifdef IS_MPI
#include <mpi.h>
#endif

namespace OpenMD {
  
  DistanceFinder::DistanceFinder(SimInfo* info) : info_(info) {
    
    nStuntDoubles_ = info_->getNGlobalAtoms() + info_->getNGlobalRigidBodies();
    stuntdoubles_.resize(nStuntDoubles_);
    
    SimInfo::MoleculeIterator mi;
    Molecule* mol;
    Molecule::AtomIterator ai;
    Atom* atom;
    Molecule::RigidBodyIterator rbIter;
    RigidBody* rb;
    
    
    for (mol = info_->beginMolecule(mi); mol != NULL; 
         mol = info_->nextMolecule(mi)) {
        
      for(atom = mol->beginAtom(ai); atom != NULL; 
          atom = mol->nextAtom(ai)) {
	stuntdoubles_[atom->getGlobalIndex()] = atom;
      }

      for (rb = mol->beginRigidBody(rbIter); rb != NULL; 
           rb = mol->nextRigidBody(rbIter)) {
	stuntdoubles_[rb->getGlobalIndex()] = rb;
      }
    }
  }

  OpenMDBitSet DistanceFinder::find(const OpenMDBitSet& bs, RealType distance) {
    StuntDouble * center;
    Vector3d centerPos;
    Snapshot* currSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    OpenMDBitSet bsResult(nStuntDoubles_);   
    assert(bsResult.size() == bs.size());

#ifdef IS_MPI
    int mol;
    int proc;
    RealType data[3];
    int worldRank = MPI::COMM_WORLD.Get_rank();
#endif
 
    for (unsigned int j = 0; j < stuntdoubles_.size(); ++j) {
      if (stuntdoubles_[j]->isRigidBody()) {
        RigidBody* rb = static_cast<RigidBody*>(stuntdoubles_[j]);
        rb->updateAtoms();
      }
    }
       
    OpenMDBitSet bsTemp(nStuntDoubles_);
    bsTemp = bs;
    bsTemp.parallelReduce();

    for (int i = bsTemp.firstOnBit(); i != -1; i = bsTemp.nextOnBit(i)) {

      // Now, if we own stuntdouble i, we can use the position, but in
      // parallel, we'll need to let everyone else know what that
      // position is!

#ifdef IS_MPI
      mol = info_->getGlobalMolMembership(i);
      proc = info_->getMolToProc(mol);
     
      if (proc == worldRank) {
        center = stuntdoubles_[i];
        centerPos = center->getPos();
        data[0] = centerPos.x();
        data[1] = centerPos.y();
        data[2] = centerPos.z();          
        MPI::COMM_WORLD.Bcast(data, 3, MPI::REALTYPE, proc);
      } else {
        MPI::COMM_WORLD.Bcast(data, 3, MPI::REALTYPE, proc);
        centerPos = Vector3d(data);
      }
#else
      center = stuntdoubles_[i];
      centerPos = center->getPos();
#endif
      
      for (unsigned int j = 0; j < stuntdoubles_.size(); ++j) {
	Vector3d r =centerPos - stuntdoubles_[j]->getPos();
	currSnapshot->wrapVector(r);
	if (r.length() <= distance) {
	  bsResult.setBitOn(j);
	}
      }
    }
    return bsResult;
  }


OpenMDBitSet DistanceFinder::find(const OpenMDBitSet& bs, RealType distance, int frame ) {
    StuntDouble * center;
    Vector3d centerPos;
    Snapshot* currSnapshot = info_->getSnapshotManager()->getSnapshot(frame);
    OpenMDBitSet bsResult(nStuntDoubles_);   
    assert(bsResult.size() == bs.size());

#ifdef IS_MPI
    int mol;
    int proc;
    RealType data[3];
    int worldRank = MPI::COMM_WORLD.Get_rank();
#endif
 
    for (unsigned int j = 0; j < stuntdoubles_.size(); ++j) {
      if (stuntdoubles_[j]->isRigidBody()) {
        RigidBody* rb = static_cast<RigidBody*>(stuntdoubles_[j]);
        rb->updateAtoms(frame);
      }
    }
       
    OpenMDBitSet bsTemp(nStuntDoubles_);
    bsTemp = bs;
    bsTemp.parallelReduce();

    for (int i = bsTemp.firstOnBit(); i != -1; i = bsTemp.nextOnBit(i)) {

      // Now, if we own stuntdouble i, we can use the position, but in
      // parallel, we'll need to let everyone else know what that
      // position is!

#ifdef IS_MPI
      mol = info_->getGlobalMolMembership(i);
      proc = info_->getMolToProc(mol);
     
      if (proc == worldRank) {
        center = stuntdoubles_[i];
        centerPos = center->getPos(frame);
        data[0] = centerPos.x();
        data[1] = centerPos.y();
        data[2] = centerPos.z();          
        MPI::COMM_WORLD.Bcast(data, 3, MPI::REALTYPE, proc);
      } else {
        MPI::COMM_WORLD.Bcast(data, 3, MPI::REALTYPE, proc);
        centerPos = Vector3d(data);
      }
#else
      center = stuntdoubles_[i];
      centerPos = center->getPos(frame);
#endif
      
      for (unsigned int j = 0; j < stuntdoubles_.size(); ++j) {
	Vector3d r =centerPos - stuntdoubles_[j]->getPos(frame);
	currSnapshot->wrapVector(r);
	if (r.length() <= distance) {
	  bsResult.setBitOn(j);
	}
      }
    }
    return bsResult;
  }
}
