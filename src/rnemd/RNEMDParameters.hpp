/*
 * Copyright (c) 2012 The University of Notre Dame. All Rights Reserved.
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
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 234107 (2008).          
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */
 
#ifndef RNEMD_RNEMDPARAMTERS_HPP
#define RNEMD_RNEMDPARAMTERS_HPP

#include "types/DataHolder.hpp"

namespace OpenMD {
  class RNEMDParameters : public DataHolder {
    DeclareParameter(UseRNEMD, bool);
    DeclareParameter(ObjectSelection, std::string);
    DeclareParameter(Method, std::string);
    DeclareParameter(FluxType, std::string);
    DeclareParameter(ExchangeTime, RealType);
    DeclareParameter(KineticFlux, RealType);
    DeclareParameter(MomentumFlux, RealType);
    DeclareParameter(MomentumFluxVector, Vector3d);
    DeclareParameter(AngularMomentumFlux, RealType);
    DeclareParameter(AngularMomentumFluxVector, Vector3d);
    DeclareParameter(SlabWidth, RealType);
    DeclareParameter(SlabACenter, RealType);
    DeclareParameter(SlabBCenter, RealType);
    DeclareParameter(SphereARadius, RealType);
    DeclareParameter(SphereBRadius, RealType);
    DeclareParameter(SelectionA, std::string);
    DeclareParameter(SelectionB, std::string);
    DeclareParameter(CoordinateOrigin, Vector3d);
    DeclareParameter(OutputFileName, std::string);
    DeclareParameter(OutputBins, int);
    DeclareParameter(OutputBinWidth, RealType);
    DeclareParameter(OutputFields, std::string);    
  public:
    RNEMDParameters();
    virtual ~RNEMDParameters();
    virtual void validate();
    
  };
}
#endif
