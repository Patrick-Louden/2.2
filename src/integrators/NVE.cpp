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
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */
 
/**
 * @file NVE.cpp
 * @author tlin
 * @date 11/08/2004
 * @time 15:13am
 * @version 1.0
 */

#include "integrators/NVE.hpp"
#include "primitives/Molecule.hpp"
#include "utils/PhysicalConstants.hpp"

namespace OpenMD {


  NVE::NVE(SimInfo* info) : VelocityVerletIntegrator(info){

  }

  void NVE::moveA(){
    SimInfo::MoleculeIterator i;
    Molecule::IntegrableObjectIterator  j;
    Molecule* mol;
    StuntDouble* integrableObject;
    Vector3d vel;
    Vector3d pos;
    Vector3d frc;
    Vector3d Tb;
    Vector3d ji;
    RealType mass;
    
    for (mol = info_->beginMolecule(i); mol != NULL; mol = info_->nextMolecule(i)) {
      for (integrableObject = mol->beginIntegrableObject(j); integrableObject != NULL;
	   integrableObject = mol->nextIntegrableObject(j)) {

	vel =integrableObject->getVel();
	pos = integrableObject->getPos();
	frc = integrableObject->getFrc();
	mass = integrableObject->getMass();
                
	// velocity half step
	vel += (dt2 /mass * PhysicalConstants::energyConvert) * frc;

	// position whole step
	pos += dt * vel;

	integrableObject->setVel(vel);
	integrableObject->setPos(pos);

	if (integrableObject->isDirectional()){

	  // get and convert the torque to body frame

	  Tb = integrableObject->lab2Body(integrableObject->getTrq());

	  // get the angular momentum, and propagate a half step

	  ji = integrableObject->getJ();

	  ji += (dt2  * PhysicalConstants::energyConvert) * Tb;

	  rotAlgo->rotate(integrableObject, ji, dt);

	  integrableObject->setJ(ji);
	}

            
      }
    } //end for(mol = info_->beginMolecule(i))
    
    rattle->constraintA();
    
  }    

  void NVE::moveB(){
    SimInfo::MoleculeIterator i;
    Molecule::IntegrableObjectIterator  j;
    Molecule* mol;
    StuntDouble* integrableObject;
    Vector3d vel;
    Vector3d frc;
    Vector3d Tb;
    Vector3d ji;
    RealType mass;
    
    for (mol = info_->beginMolecule(i); mol != NULL; mol = info_->nextMolecule(i)) {
      for (integrableObject = mol->beginIntegrableObject(j); integrableObject != NULL;
	   integrableObject = mol->nextIntegrableObject(j)) {

	vel =integrableObject->getVel();
	frc = integrableObject->getFrc();
	mass = integrableObject->getMass();
                
	// velocity half step
	vel += (dt2 /mass * PhysicalConstants::energyConvert) * frc;
                
	integrableObject->setVel(vel);

	if (integrableObject->isDirectional()){

	  // get and convert the torque to body frame

	  Tb = integrableObject->lab2Body(integrableObject->getTrq());

	  // get the angular momentum, and propagate a half step

	  ji = integrableObject->getJ();

	  ji += (dt2  * PhysicalConstants::energyConvert) * Tb;

	  integrableObject->setJ(ji);
	}

            
      }
    } //end for(mol = info_->beginMolecule(i))
  

    rattle->constraintB();

  }


  RealType NVE::calcConservedQuantity() {
    return thermo.getTotalE() ;
  }

} //end namespace OpenMD
