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
 
#include "minimizers/PRCG.hpp"
#ifdef IS_MPI
#include <mpi.h>
#endif

namespace OpenMD {
	
  void PRCGMinimizer::init(){

    calcG();
  
    for(int i = 0; i < direction.size(); i++){    
      direction[i] = -curG[i];
    }

  }

  int PRCGMinimizer::step(){
    int lsStatus;
  
    prevF = curF;
    prevG = curG;
    prevX = curX;

    //optimize along the search direction and reset minimum point value
    lsStatus = doLineSearch(direction, stepSize);

    if (lsStatus < 0)
      return -1;
    else
      return 1;
  }

  void PRCGMinimizer::prepareStep(){
    std::vector<RealType> deltaGrad;
    RealType beta;
    size_t i;

    deltaGrad.resize(ndim);
    
    //calculate the new direction using Polak-Ribiere Conjugate Gradient
  
    for(i = 0; i < curG.size(); i++)
      deltaGrad[i] = curG[i] - prevG[i];

#ifndef IS_MPI
    beta = dotProduct(deltaGrad, curG) / dotProduct(prevG, prevG);
#else
    RealType localDP1;
    RealType localDP2;
    RealType globalDP1;
    RealType globalDP2;

    localDP1 =  dotProduct(deltaGrad, curG);
    localDP2 = dotProduct(prevG, prevG);

    MPI_Allreduce(&localDP1, &globalDP1, 1, MPI_REALTYPE,MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&localDP2, &globalDP2, 1, MPI_REALTYPE,MPI_SUM, MPI_COMM_WORLD);
  
    beta = globalDP1 / globalDP2;
#endif

    for(i = 0; i < direction.size(); i++)  
      direction[i] = -curG[i] + beta * direction[i];

  }

}
