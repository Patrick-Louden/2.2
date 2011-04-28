/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
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
 * [4]  Vardeman & Gezelter, in progress (2009).                        
 */
 
/**
 * @file ForceManager.cpp
 * @author tlin
 * @date 11/09/2004
 * @time 10:39am
 * @version 1.0
 */

#include "brains/ForceManager.hpp"
#include "primitives/Molecule.hpp"
#define __OPENMD_C
#include "utils/simError.h"
#include "primitives/Bond.hpp"
#include "primitives/Bend.hpp"
#include "primitives/Torsion.hpp"
#include "primitives/Inversion.hpp"
#include "nonbonded/NonBondedInteraction.hpp"
#include "parallel/ForceMatrixDecomposition.hpp"

using namespace std;
namespace OpenMD {
  
  ForceManager::ForceManager(SimInfo * info) : info_(info) {

#ifdef IS_MPI
    fDecomp_ = new ForceMatrixDecomposition(info_);
#else
    // fDecomp_ = new ForceSerialDecomposition(info);
#endif
  }
  
  void ForceManager::calcForces() {
    
    if (!info_->isFortranInitialized()) {
      info_->update();
      interactionMan_->setSimInfo(info_);
      interactionMan_->initialize();
      swfun_ = interactionMan_->getSwitchingFunction();
      fDecomp_->distributeInitialData();
      info_->setupFortran();
    }
    
    preCalculation();   
    shortRangeInteractions();
    longRangeInteractions();
    postCalculation();
    
  }
  
  void ForceManager::preCalculation() {
    SimInfo::MoleculeIterator mi;
    Molecule* mol;
    Molecule::AtomIterator ai;
    Atom* atom;
    Molecule::RigidBodyIterator rbIter;
    RigidBody* rb;
    Molecule::CutoffGroupIterator ci;
    CutoffGroup* cg;
    
    // forces are zeroed here, before any are accumulated.
    
    for (mol = info_->beginMolecule(mi); mol != NULL; 
         mol = info_->nextMolecule(mi)) {
      for(atom = mol->beginAtom(ai); atom != NULL; atom = mol->nextAtom(ai)) {
	atom->zeroForcesAndTorques();
      }
          
      //change the positions of atoms which belong to the rigidbodies
      for (rb = mol->beginRigidBody(rbIter); rb != NULL; 
           rb = mol->nextRigidBody(rbIter)) {
	rb->zeroForcesAndTorques();
      }        

      if(info_->getNGlobalCutoffGroups() != info_->getNGlobalAtoms()){
        for(cg = mol->beginCutoffGroup(ci); cg != NULL; 
            cg = mol->nextCutoffGroup(ci)) {
          //calculate the center of mass of cutoff group
          cg->updateCOM();
        }
      }      
    }
   
    // Zero out the stress tensor
    tau *= 0.0;
    
  }
  
  void ForceManager::shortRangeInteractions() {
    Molecule* mol;
    RigidBody* rb;
    Bond* bond;
    Bend* bend;
    Torsion* torsion;
    Inversion* inversion;
    SimInfo::MoleculeIterator mi;
    Molecule::RigidBodyIterator rbIter;
    Molecule::BondIterator bondIter;;
    Molecule::BendIterator  bendIter;
    Molecule::TorsionIterator  torsionIter;
    Molecule::InversionIterator  inversionIter;
    RealType bondPotential = 0.0;
    RealType bendPotential = 0.0;
    RealType torsionPotential = 0.0;
    RealType inversionPotential = 0.0;

    //calculate short range interactions    
    for (mol = info_->beginMolecule(mi); mol != NULL; 
         mol = info_->nextMolecule(mi)) {

      //change the positions of atoms which belong to the rigidbodies
      for (rb = mol->beginRigidBody(rbIter); rb != NULL; 
           rb = mol->nextRigidBody(rbIter)) {
        rb->updateAtoms();
      }

      for (bond = mol->beginBond(bondIter); bond != NULL; 
           bond = mol->nextBond(bondIter)) {
        bond->calcForce();
        bondPotential += bond->getPotential();
      }

      for (bend = mol->beginBend(bendIter); bend != NULL; 
           bend = mol->nextBend(bendIter)) {
        
        RealType angle;
        bend->calcForce(angle);
        RealType currBendPot = bend->getPotential();          
         
        bendPotential += bend->getPotential();
        map<Bend*, BendDataSet>::iterator i = bendDataSets.find(bend);
        if (i == bendDataSets.end()) {
          BendDataSet dataSet;
          dataSet.prev.angle = dataSet.curr.angle = angle;
          dataSet.prev.potential = dataSet.curr.potential = currBendPot;
          dataSet.deltaV = 0.0;
          bendDataSets.insert(map<Bend*, BendDataSet>::value_type(bend, dataSet));
        }else {
          i->second.prev.angle = i->second.curr.angle;
          i->second.prev.potential = i->second.curr.potential;
          i->second.curr.angle = angle;
          i->second.curr.potential = currBendPot;
          i->second.deltaV =  fabs(i->second.curr.potential -  
                                   i->second.prev.potential);
        }
      }
      
      for (torsion = mol->beginTorsion(torsionIter); torsion != NULL; 
           torsion = mol->nextTorsion(torsionIter)) {
        RealType angle;
        torsion->calcForce(angle);
        RealType currTorsionPot = torsion->getPotential();
        torsionPotential += torsion->getPotential();
        map<Torsion*, TorsionDataSet>::iterator i = torsionDataSets.find(torsion);
        if (i == torsionDataSets.end()) {
          TorsionDataSet dataSet;
          dataSet.prev.angle = dataSet.curr.angle = angle;
          dataSet.prev.potential = dataSet.curr.potential = currTorsionPot;
          dataSet.deltaV = 0.0;
          torsionDataSets.insert(map<Torsion*, TorsionDataSet>::value_type(torsion, dataSet));
        }else {
          i->second.prev.angle = i->second.curr.angle;
          i->second.prev.potential = i->second.curr.potential;
          i->second.curr.angle = angle;
          i->second.curr.potential = currTorsionPot;
          i->second.deltaV =  fabs(i->second.curr.potential -  
                                   i->second.prev.potential);
        }      
      }      
      
      for (inversion = mol->beginInversion(inversionIter); 
	   inversion != NULL; 
           inversion = mol->nextInversion(inversionIter)) {
        RealType angle;
        inversion->calcForce(angle);
        RealType currInversionPot = inversion->getPotential();
        inversionPotential += inversion->getPotential();
        map<Inversion*, InversionDataSet>::iterator i = inversionDataSets.find(inversion);
        if (i == inversionDataSets.end()) {
          InversionDataSet dataSet;
          dataSet.prev.angle = dataSet.curr.angle = angle;
          dataSet.prev.potential = dataSet.curr.potential = currInversionPot;
          dataSet.deltaV = 0.0;
          inversionDataSets.insert(map<Inversion*, InversionDataSet>::value_type(inversion, dataSet));
        }else {
          i->second.prev.angle = i->second.curr.angle;
          i->second.prev.potential = i->second.curr.potential;
          i->second.curr.angle = angle;
          i->second.curr.potential = currInversionPot;
          i->second.deltaV =  fabs(i->second.curr.potential -  
                                   i->second.prev.potential);
        }      
      }      
    }
    
    RealType  shortRangePotential = bondPotential + bendPotential + 
      torsionPotential +  inversionPotential;    
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    curSnapshot->statData[Stats::SHORT_RANGE_POTENTIAL] = shortRangePotential;
    curSnapshot->statData[Stats::BOND_POTENTIAL] = bondPotential;
    curSnapshot->statData[Stats::BEND_POTENTIAL] = bendPotential;
    curSnapshot->statData[Stats::DIHEDRAL_POTENTIAL] = torsionPotential;
    curSnapshot->statData[Stats::INVERSION_POTENTIAL] = inversionPotential;    
  }
  
  void ForceManager::longRangeInteractions() {

    // some of this initial stuff will go away:
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    DataStorage* config = &(curSnapshot->atomData);
    DataStorage* cgConfig = &(curSnapshot->cgData);
    RealType* frc = config->getArrayPointer(DataStorage::dslForce);
    RealType* pos = config->getArrayPointer(DataStorage::dslPosition);
    RealType* trq = config->getArrayPointer(DataStorage::dslTorque);
    RealType* A = config->getArrayPointer(DataStorage::dslAmat);
    RealType* electroFrame = config->getArrayPointer(DataStorage::dslElectroFrame);
    RealType* particlePot = config->getArrayPointer(DataStorage::dslParticlePot);
    RealType* rc;    

    if(info_->getNGlobalCutoffGroups() != info_->getNGlobalAtoms()){
      rc = cgConfig->getArrayPointer(DataStorage::dslPosition);
    } else {
      // center of mass of the group is the same as position of the atom  
      // if cutoff group does not exist
      rc = pos;
    }
    
    //initialize data before passing to fortran
    RealType longRangePotential[N_INTERACTION_FAMILIES];
    RealType lrPot = 0.0;
    int isError = 0;

    // dangerous to iterate over enums, but we'll live on the edge:
    for (int i = NO_FAMILY; i != N_INTERACTION_FAMILIES; ++i){
      longRangePotential[i]=0.0; //Initialize array
    }

    // new stuff starts here:

    fDecomp_->distributeData();
 
    int cg1, cg2, atom1, atom2;
    Vector3d d_grp, dag;
    RealType rgrpsq, rgrp;
    RealType vij;
    Vector3d fij, fg;
    pair<int, int> gtypes;
    RealType rCutSq;
    bool in_switching_region;
    RealType sw, dswdr, swderiv;
    vector<int> atomListColumn, atomListRow, atomListLocal;
    InteractionData idat;
    SelfData sdat;
    RealType mf;

    int loopStart, loopEnd;

    loopEnd = PAIR_LOOP;
    if (info_->requiresPrepair() ) {
      loopStart = PREPAIR_LOOP;
    } else {
      loopStart = PAIR_LOOP;
    }

    for (int iLoop = loopStart; iLoop < loopEnd; iLoop++) {
      
      if (iLoop == loopStart) {
        bool update_nlist = fDecomp_->checkNeighborList();
        if (update_nlist) 
          neighborList = fDecomp_->buildNeighborList();
      }

      for (vector<pair<int, int> >::iterator it = neighborList.begin(); 
             it != neighborList.end(); ++it) {
        
        cg1 = (*it).first;
        cg2 = (*it).second;

        gtypes = fDecomp_->getGroupTypes(cg1, cg2);
        d_grp  = fDecomp_->getIntergroupVector(cg1, cg2);
        curSnapshot->wrapVector(d_grp);        
        rgrpsq = d_grp.lengthSquare();
        rCutSq = groupCutoffMap[gtypes].first;

        if (rgrpsq < rCutSq) {
          idat.rcut = groupCutoffMap[gtypes].second;
          if (iLoop == PAIR_LOOP) {
            vij *= 0.0;
            fij = V3Zero;
          }
          
          in_switching_region = swfun_->getSwitch(rgrpsq, idat.sw, dswdr, rgrp);               
          atomListRow = fDecomp_->getAtomsInGroupRow(cg1);
          atomListColumn = fDecomp_->getAtomsInGroupColumn(cg2);

          for (vector<int>::iterator ia = atomListRow.begin(); 
               ia != atomListRow.end(); ++ia) {            
            atom1 = (*ia);
            
            for (vector<int>::iterator jb = atomListColumn.begin(); 
                 jb != atomListColumn.end(); ++jb) {              
              atom2 = (*jb);
              
              if (!fDecomp_->skipAtomPair(atom1, atom2)) {
                
                idat = fDecomp_->fillInteractionData(atom1, atom2);

                if (atomListRow.size() == 1 && atomListColumn.size() == 1) {
                  idat.d = d_grp;
                  idat.r2 = rgrpsq;
                } else {
                  idat.d = fDecomp_->getInteratomicVector(atom1, atom2);
                  curSnapshot->wrapVector(idat.d);
                  idat.r2 = idat.d.lengthSquare();
                }
                
                idat.rij = sqrt(idat.r2);
               
                if (iLoop == PREPAIR_LOOP) {
                  interactionMan_->doPrePair(idat);
                } else {
                  interactionMan_->doPair(idat);
                  vij += idat.vpair;
                  fij += idat.f1;
                  tau -= outProduct(idat.d, idat.f1);
                }
              }
            }
          }

          if (iLoop == PAIR_LOOP) {
            if (in_switching_region) {
              swderiv = vij * dswdr / rgrp;
              fg = swderiv * d_grp;

              fij += fg;

              if (atomListRow.size() == 1 && atomListColumn.size() == 1) {
                tau -= outProduct(idat.d, fg);
              }
          
              for (vector<int>::iterator ia = atomListRow.begin(); 
                   ia != atomListRow.end(); ++ia) {            
                atom1 = (*ia);                
                mf = fDecomp_->getMfactRow(atom1);
                // fg is the force on atom ia due to cutoff group's
                // presence in switching region
                fg = swderiv * d_grp * mf;
                fDecomp_->addForceToAtomRow(atom1, fg);

                if (atomListRow.size() > 1) {
                  if (info_->usesAtomicVirial()) {
                    // find the distance between the atom
                    // and the center of the cutoff group:
                    dag = fDecomp_->getAtomToGroupVectorRow(atom1, cg1);
                    tau -= outProduct(dag, fg);
                  }
                }
              }
              for (vector<int>::iterator jb = atomListColumn.begin(); 
                   jb != atomListColumn.end(); ++jb) {              
                atom2 = (*jb);
                mf = fDecomp_->getMfactColumn(atom2);
                // fg is the force on atom jb due to cutoff group's
                // presence in switching region
                fg = -swderiv * d_grp * mf;
                fDecomp_->addForceToAtomColumn(atom2, fg);

                if (atomListColumn.size() > 1) {
                  if (info_->usesAtomicVirial()) {
                    // find the distance between the atom
                    // and the center of the cutoff group:
                    dag = fDecomp_->getAtomToGroupVectorColumn(atom2, cg2);
                    tau -= outProduct(dag, fg);
                  }
                }
              }
            }
            //if (!SIM_uses_AtomicVirial) {
            //  tau -= outProduct(d_grp, fij);
            //}
          }
        }
      }

      if (iLoop == PREPAIR_LOOP) {
        if (info_->requiresPrepair()) {            
          fDecomp_->collectIntermediateData();
          atomListLocal = fDecomp_->getAtomList();
          for (vector<int>::iterator ia = atomListLocal.begin(); 
               ia != atomListLocal.end(); ++ia) {              
            atom1 = (*ia);            
            sdat = fDecomp_->fillSelfData(atom1);
            interactionMan_->doPreForce(sdat);
          }
          fDecomp_->distributeIntermediateData();        
        }
      }

    }
    
    fDecomp_->collectData();
    
    if (info_->requiresSkipCorrection() || info_->requiresSelfCorrection()) {
      atomListLocal = fDecomp_->getAtomList();
      for (vector<int>::iterator ia = atomListLocal.begin(); 
           ia != atomListLocal.end(); ++ia) {              
        atom1 = (*ia);     

        if (info_->requiresSkipCorrection()) {
          vector<int> skipList = fDecomp_->getSkipsForAtom(atom1);
          for (vector<int>::iterator jb = skipList.begin(); 
               jb != skipList.end(); ++jb) {              
            atom2 = (*jb);
            idat = fDecomp_->fillSkipData(atom1, atom2);
            interactionMan_->doSkipCorrection(idat);
          }
        }
          
        if (info_->requiresSelfCorrection()) {
          sdat = fDecomp_->fillSelfData(atom1);
          interactionMan_->doSelfCorrection(sdat);
        }
      }
    }

    // dangerous to iterate over enums, but we'll live on the edge:
    for (int i = NO_FAMILY; i != N_INTERACTION_FAMILIES; ++i){
      lrPot += longRangePotential[i]; //Quick hack
    }
        
    //store the tau and long range potential    
    curSnapshot->statData[Stats::LONG_RANGE_POTENTIAL] = lrPot;
    curSnapshot->statData[Stats::VANDERWAALS_POTENTIAL] = longRangePotential[VANDERWAALS_FAMILY];
    curSnapshot->statData[Stats::ELECTROSTATIC_POTENTIAL] = longRangePotential[ELECTROSTATIC_FAMILY];
  }

  
  void ForceManager::postCalculation() {
    SimInfo::MoleculeIterator mi;
    Molecule* mol;
    Molecule::RigidBodyIterator rbIter;
    RigidBody* rb;
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    
    // collect the atomic forces onto rigid bodies
    
    for (mol = info_->beginMolecule(mi); mol != NULL; 
         mol = info_->nextMolecule(mi)) {
      for (rb = mol->beginRigidBody(rbIter); rb != NULL; 
           rb = mol->nextRigidBody(rbIter)) { 
        Mat3x3d rbTau = rb->calcForcesAndTorquesAndVirial();
        tau += rbTau;
      }
    }
    
#ifdef IS_MPI
    Mat3x3d tmpTau(tau);
    MPI_Allreduce(tmpTau.getArrayPointer(), tau.getArrayPointer(), 
                  9, MPI_REALTYPE, MPI_SUM, MPI_COMM_WORLD);
#endif
    curSnapshot->statData.setTau(tau);
  }

} //end namespace OpenMD
