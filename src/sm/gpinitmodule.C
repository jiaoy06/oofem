// Initialization module reading data rekated to Gauss points from a specified file
/*

                   *****    *****   ******  ******  ***   ***                            
                 **   **  **   **  **      **      ** *** **                             
                **   **  **   **  ****    ****    **  *  **                              
               **   **  **   **  **      **      **     **                               
              **   **  **   **  **      **      **     **                                
              *****    *****   **      ******  **     **         
            
                                                                   
               OOFEM : Object Oriented Finite Element Code                 
                    
                 Copyright (C) 1993 - 2010   Borek Patzak                                       



         Czech Technical University, Faculty of Civil Engineering,
     Department of Structural Mechanics, 166 29 Prague, Czech Republic
                                                                               
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                                                                              
*/

#include "gpinitmodule.h"
#include "gausspnt.h"
#include "element.h"
#include "integrationrule.h"
#include "material.h"

#include "timestep.h"
#include "engngm.h"
#include "strreader.h"
#include "structuralms.h"
#ifndef __MAKEDEPEND
#include <vector>

#include <assert.h>
#include "flotarry.h"
#endif


namespace oofem {


GPInitModule :: GPInitModule (EngngModel* e) : InitModule(e)
{
}


GPInitModule::~GPInitModule ()
{
}


IRResultType
GPInitModule :: initializeFrom (InputRecord* ir)
{
 InitModule::initializeFrom (ir);
 return IRRT_OK;
}


void    
GPInitModule :: doInit ()
{
  int ielem, igp, nelem, ie, ig, nv, iv, nc, ic, varsize, vt;
  InternalStateType vartype;
  Element* elem;
  GaussPoint* gp;
  FloatArray value;

  double coords[3];
  
  Domain* d = emodel->giveDomain(1);
  nelem = d -> giveNumberOfElements();

  // loop over elements
  for (ielem = 1; ielem <= nelem; ielem++) {
    elem = d->giveElement(ielem);
    Material* mat = elem -> giveMaterial();
    IntegrationRule* iRule = elem->giveDefaultIntegrationRulePtr();
    // loop over Gauss points
    for (igp=0 ; igp < iRule->getNumberOfIntegrationPoints() ; igp++) {
      gp = iRule->getIntegrationPoint(igp) ;
      MaterialStatus* status = mat -> giveStatus(gp);
      fscanf (initStream,"%d %d",&ie,&ig);
      // check whether the element and GP number agree
      assert (ielem==ie);
      assert ((igp+1)==ig);
      // read coordinates
      fscanf (initStream,"%d",&nc);
      assert (nc>=0 && nc<=3);
      for (ic=0; ic<nc; ic++)
	fscanf (initStream,"%lg",&coords[ic]);
      fscanf (initStream,"%d",&nv);
      assert (nv>=0);
      for (iv=1; iv<=nv; iv++){
	fscanf (initStream,"%d %d",&vt,&varsize);
	vartype = (InternalStateType) vt;
	value.resize(varsize);
	for (ic=1; ic<=varsize; ic++)
	  fscanf (initStream,"%lg",&value.at(ic));
	mat -> setIPValue (value, gp, vartype);
      }
      
      // restore consistency (compute dependent internal variables)
      status -> restoreConsistency(); 
    }
  }
 fclose (initStream);
}

} // namespace oofem