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
  
 /**
  * @file MoleculeCreator.cpp
  * @author tlin
  * @date 11/04/2004
  * @time 13:44am
  * @version 1.0
  */

#include <cassert>
#include <set>

#include "brains/MoleculeCreator.hpp"
#include "primitives/GhostBend.hpp"
#include "types/DirectionalAtomType.hpp"
#include "types/FixedBondType.hpp"
#include "utils/simError.h"
#include "utils/StringUtils.hpp"

namespace oopse {

Molecule* MoleculeCreator::createMolecule(ForceField* ff, MoleculeStamp *molStamp,
    int stampId, int globalIndex, LocalIndexManager* localIndexMan) {

    Molecule* mol = new Molecule(stampId, globalIndex, molStamp->getID());
    
    //create atoms
    Atom* atom;
    AtomStamp* currentAtomStamp;
    int nAtom = molStamp->getNAtoms();
    for (int i = 0; i < nAtom; ++i) {
        currentAtomStamp = molStamp->getAtom(i);
        atom = createAtom(ff, mol, currentAtomStamp, localIndexMan);
        mol->addAtom(atom);
    }

    //create rigidbodies
    RigidBody* rb;
    RigidBodyStamp * currentRigidBodyStamp;
    int nRigidbodies = molStamp->getNRigidBodies();

    for (int i = 0; i < nRigidbodies; ++i) {
        currentRigidBodyStamp = molStamp->getRigidBody(i);
        rb = createRigidBody(molStamp, mol, currentRigidBodyStamp, localIndexMan);
        mol->addRigidBody(rb);
    }

    //create bonds
    Bond* bond;
    BondStamp* currentBondStamp;
    int nBonds = molStamp->getNBonds();

    for (int i = 0; i < nBonds; ++i) {
        currentBondStamp = molStamp->getBond(i);
        bond = createBond(ff, mol, currentBondStamp);
        mol->addBond(bond);
    }

    //create bends
    Bend* bend;
    BendStamp* currentBendStamp;
    int nBends = molStamp->getNBends();
    for (int i = 0; i < nBends; ++i) {
        currentBendStamp = molStamp->getBend(i);
        bend = createBend(ff, mol, currentBendStamp);
        mol->addBend(bend);
    }

    //create torsions
    Torsion* torsion;
    TorsionStamp* currentTorsionStamp;
    int nTorsions = molStamp->getNTorsions();
    for (int i = 0; i < nTorsions; ++i) {
        currentTorsionStamp = molStamp->getTorsion(i);
        torsion = createTorsion(ff, mol, currentTorsionStamp);
        mol->addTorsion(torsion);
    }

    //create cutoffGroups
    CutoffGroup* cutoffGroup;
    CutoffGroupStamp* currentCutoffGroupStamp;
    int nCutoffGroups = molStamp->getNCutoffGroups();
    for (int i = 0; i < nCutoffGroups; ++i) {
        currentCutoffGroupStamp = molStamp->getCutoffGroup(i);
        cutoffGroup = createCutoffGroup(mol, currentCutoffGroupStamp);
        mol->addCutoffGroup(cutoffGroup);
    }

    //every free atom is a cutoff group    
    std::set<Atom*> allAtoms;
     Molecule::AtomIterator ai;

    //add all atoms into allAtoms set
    for(atom = mol->beginAtom(ai); atom != NULL; atom = mol->nextAtom(ai)) {
        allAtoms.insert(atom);
    }

    Molecule::CutoffGroupIterator ci;
    CutoffGroup* cg;
    std::set<Atom*> cutoffAtoms;    
    
    //add all of the atoms belong to cutoff groups into cutoffAtoms set
    for (cg = mol->beginCutoffGroup(ci); cg != NULL; cg = mol->nextCutoffGroup(ci)) {

        for(atom = cg->beginAtom(ai); atom != NULL; atom = cg->nextAtom(ai)) {
            cutoffAtoms.insert(atom);
        }

    }       
    
    //find all free atoms (which do not belong to cutoff groups)  
    //performs the "difference" operation from set theory,  the output range contains a copy of every
    //element that is contained in [allAtoms.begin(), allAtoms.end()) and not contained in 
    //[cutoffAtoms.begin(), cutoffAtoms.end()).
    std::vector<Atom*> freeAtoms;    
    std::set_difference(allAtoms.begin(), allAtoms.end(), cutoffAtoms.begin(), cutoffAtoms.end(),
                            std::back_inserter(freeAtoms));

    if (freeAtoms.size() != allAtoms.size() - cutoffAtoms.size()) {
        //Some atoms in rigidAtoms are not in allAtoms, something must be wrong
        sprintf(painCave.errMsg, "Atoms in cutoff groups are not in the atom list of the same molecule");

        painCave.isFatal = 1;
        simError();        
    }

    //loop over the free atoms and then create one cutoff group for every single free atom
    std::vector<Atom*>::iterator fai;

    for (fai = freeAtoms.begin(); fai != freeAtoms.end(); ++fai) {
        cutoffGroup = createCutoffGroup(mol, *fai);
        mol->addCutoffGroup(cutoffGroup);
    }
    //create constraints
    createConstraintPair(mol);
    createConstraintElem(mol);
    
    //the construction of this molecule is finished
    mol->complete();

    return mol;
}    


Atom* MoleculeCreator::createAtom(ForceField* ff, Molecule* mol, AtomStamp* stamp, 
                                                                  LocalIndexManager* localIndexMan) {
    AtomType * atomType;
    Atom* atom;

    atomType =  ff->getAtomType(stamp->getType());

    if (atomType == NULL) {
        sprintf(painCave.errMsg, "Can not find Matching Atom Type for[%s]",
                   stamp->getType());

        painCave.isFatal = 1;
        simError();
    }
    
    //below code still have some kind of hard-coding smell
    if (atomType->isDirectional()){
     
        DirectionalAtomType* dAtomType = dynamic_cast<DirectionalAtomType*>(atomType);
        
        if (dAtomType == NULL) {
            sprintf(painCave.errMsg, "Can not cast AtomType to DirectionalAtomType");

            painCave.isFatal = 1;
            simError();
        }

        DirectionalAtom* dAtom;
        dAtom = new DirectionalAtom(dAtomType);
        atom = dAtom;    
    }
    else{
        atom = new Atom(atomType);
    }

    atom->setLocalIndex(localIndexMan->getNextAtomIndex());

    return atom;
}

RigidBody* MoleculeCreator::createRigidBody(MoleculeStamp *molStamp, Molecule* mol,
                                                                                    RigidBodyStamp* rbStamp, 
                                                                                    LocalIndexManager* localIndexMan) {
    Atom* atom;
    int nAtoms;
    Vector3d refCoor;
    AtomStamp* atomStamp;
    
    RigidBody* rb = new RigidBody();
    nAtoms = rbStamp->getNMembers();    
    for (int i = 0; i < nAtoms; ++i) {
        //rbStamp->getMember(i) return the local index of current atom inside the molecule.
        //It is not the same as local index of atom which is the index of atom at DataStorage class
        atom = mol->getAtomAt(rbStamp->getMember(i));
        atomStamp= molStamp->getAtom(rbStamp->getMember(i));    
        rb->addAtom(atom, atomStamp);
    }

    //after all of the atoms are added, we need to calculate the reference coordinates
    rb->calcRefCoords();

    //set the local index of this rigid body, global index will be set later
    rb->setLocalIndex(localIndexMan->getNextRigidBodyIndex());

    //the rule for naming rigidbody MoleculeName_RB_Integer
    //The first part is the name of the molecule
    //The second part is alway fixed as "RB"
    //The third part is the index of the rigidbody defined in meta-data file
    //For example, Butane_RB_0 is a valid rigid body name of butane molecule
    /**@todo replace itoa by lexi_cast */
    rb->setType(mol->getType() + "_RB_" + toString(mol->getNRigidBodies()));
    
    return rb;
}    

Bond* MoleculeCreator::createBond(ForceField* ff, Molecule* mol, BondStamp* stamp) {
    BondType* bondType;
    Atom* atomA;
    Atom* atomB;

    atomA = mol->getAtomAt(stamp->getA());
    atomB = mol->getAtomAt(stamp->getB());

    assert( atomA && atomB);
    
    bondType = ff->getBondType(atomA->getType(), atomB->getType());

    if (bondType == NULL) {
        sprintf(painCave.errMsg, "Can not find Matching Bond Type for[%s, %s]",
                   atomA->getType().c_str(),
                   atomB->getType().c_str());

        painCave.isFatal = 1;
        simError();
    }
    return new Bond(atomA, atomB, bondType);    
}    

Bend* MoleculeCreator::createBend(ForceField* ff, Molecule* mol, BendStamp* stamp) {
    bool isGhostBend = false;
    int ghostIndex;

    
    //
    if (stamp->haveExtras()){
        LinkedAssign* extras = stamp->getExtras();
        LinkedAssign* currentExtra = extras;

        while (currentExtra != NULL){
            if (!strcmp(currentExtra->getlhs(), "ghostVectorSource")){
                switch (currentExtra->getType()){
                case 0:
                    ghostIndex = currentExtra->getInt();
                    isGhostBend = true;
                    break;

                default:
                sprintf(painCave.errMsg,
                "SimSetup Error: ghostVectorSource must be an int.\n");
                painCave.isFatal = 1;
                simError();
                }
            } else{
                sprintf(painCave.errMsg,
                "SimSetup Error: unhandled bend assignment:\n");
                painCave.isFatal = 1;
                simError();
            }
            currentExtra = currentExtra->getNext();
        }
        
    }

    if (isGhostBend) {

        int indexA = stamp->getA();
        int indexB= stamp->getB();

        assert(indexA != indexB);

        int normalIndex;
        if (indexA == ghostIndex) {
            normalIndex = indexB;
        } else if (indexB == ghostIndex) {
            normalIndex = indexA;
        }
        
        Atom* normalAtom = mol->getAtomAt(normalIndex) ;        
        DirectionalAtom* ghostAtom = dynamic_cast<DirectionalAtom*>(mol->getAtomAt(ghostIndex));
        if (ghostAtom == NULL) {
            sprintf(painCave.errMsg, "Can not cast Atom to DirectionalAtom");
            painCave.isFatal = 1;
            simError();
        }
                
        BendType* bendType = ff->getBendType(normalAtom->getType(), ghostAtom->getType(), "GHOST");

        if (bendType == NULL) {
            sprintf(painCave.errMsg, "Can not find Matching Bend Type for[%s, %s, %s]",
                       normalAtom->getType().c_str(),
                       ghostAtom->getType().c_str(),
                       "GHOST");

            painCave.isFatal = 1;
            simError();
        }
        
        return new GhostBend(normalAtom, ghostAtom, bendType);       

    } else {
            
        Atom* atomA = mol->getAtomAt(stamp->getA());
        Atom* atomB = mol->getAtomAt(stamp->getB());
        Atom* atomC = mol->getAtomAt(stamp->getC());

        assert( atomA && atomB && atomC);
        
        BendType* bendType = ff->getBendType(atomA->getType(), atomB->getType(), atomC->getType());

        if (bendType == NULL) {
            sprintf(painCave.errMsg, "Can not find Matching Bend Type for[%s, %s, %s]",
                       atomA->getType().c_str(),
                       atomB->getType().c_str(),
                       atomC->getType().c_str());

            painCave.isFatal = 1;
            simError();
        }

        return new Bend(atomA, atomB, atomC, bendType);       
    }
}    

Torsion* MoleculeCreator::createTorsion(ForceField* ff, Molecule* mol, TorsionStamp* stamp) {
    TorsionType* torsionType;
    Atom* atomA;
    Atom* atomB;
    Atom* atomC;
    Atom* atomD;

    atomA = mol->getAtomAt(stamp->getA());
    atomB = mol->getAtomAt(stamp->getB());
    atomC = mol->getAtomAt(stamp->getC());
    atomD = mol->getAtomAt(stamp->getD());

    assert(atomA && atomB && atomC && atomD);
    
    torsionType = ff->getTorsionType(atomA->getType(), atomB->getType(), 
                                                       atomC->getType(), atomD->getType());

    if (torsionType == NULL) {
        sprintf(painCave.errMsg, "Can not find Matching Torsion Type for[%s, %s, %s, %s]",
                   atomA->getType().c_str(),
                   atomB->getType().c_str(),
                   atomC->getType().c_str(),
                   atomD->getType().c_str());

        painCave.isFatal = 1;
        simError();
    }
    
    return new Torsion(atomA, atomB, atomC, atomD, torsionType);       
}    

CutoffGroup* MoleculeCreator::createCutoffGroup(Molecule* mol, CutoffGroupStamp* stamp) {
    int nAtoms;
    CutoffGroup* cg;
    Atom* atom;
    cg = new CutoffGroup();
    
    nAtoms = stamp->getNMembers();
    for (int i =0; i < nAtoms; ++i) {
        atom = mol->getAtomAt(stamp->getMember(i));
        assert(atom);
        cg->addAtom(atom);
    }

    return cg;
}    

CutoffGroup* MoleculeCreator::createCutoffGroup(Molecule * mol, Atom* atom) {
    CutoffGroup* cg;
    cg  = new CutoffGroup();
    cg->addAtom(atom);
    return cg;
}

void MoleculeCreator::createConstraintPair(Molecule* mol) {

    //add bond constraints
    Molecule::BondIterator bi;
    Bond* bond;
    for (bond = mol->beginBond(bi); bond != NULL; bond = mol->nextBond(bi)) {
        
        BondType* bt = bond->getBondType();

        //class Parent1 {};
        //class Child1 : public Parent {};
        //class Child2 : public Parent {};
        //Child1* ch1 = new Child1();
        //Child2* ch2 = dynamic_cast<Child2*>(ch1); 
        //the dynamic_cast is succeed in above line. A compiler bug?        

        if (typeid(FixedBondType) == typeid(*bt)) {
            FixedBondType* fbt = dynamic_cast<FixedBondType*>(bt);

            ConstraintElem* consElemA = new ConstraintElem(bond->getAtomA());
            ConstraintElem* consElemB = new ConstraintElem(bond->getAtomB());            
            ConstraintPair* consPair = new ConstraintPair(consElemA, consElemB, fbt->getEquilibriumBondLength());
            mol->addConstraintPair(consPair);
        }
    }

    //rigidbody -- rigidbody constraint is not support yet
}

void MoleculeCreator::createConstraintElem(Molecule* mol) {

    ConstraintPair* consPair;
    Molecule::ConstraintPairIterator cpi;
    std::set<StuntDouble*> sdSet;
    for (consPair = mol->beginConstraintPair(cpi); consPair != NULL; consPair = mol->nextConstraintPair(cpi)) {

        StuntDouble* sdA = consPair->getConsElem1()->getStuntDouble();            
        if (sdSet.find(sdA) == sdSet.end()){
            sdSet.insert(sdA);
            mol->addConstraintElem(new ConstraintElem(sdA));
        }

        StuntDouble* sdB = consPair->getConsElem2()->getStuntDouble();            
        if (sdSet.find(sdB) == sdSet.end()){
            sdSet.insert(sdB);
            mol->addConstraintElem(new ConstraintElem(sdB));
        }
        
    }

}
    
}