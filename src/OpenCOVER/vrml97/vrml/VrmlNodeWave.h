/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
// %W% %G%
//

#ifndef _VRMLNODEWAVE_
#define _VRMLNODEWAVE_

#include "VrmlNode.h"
#include "VrmlSFFloat.h"
#include "VrmlSFRotation.h"
#include "VrmlSFVec3f.h"
#include "VrmlSFString.h"

#include "VrmlNodeChild.h"

namespace vrml
{

class VrmlNodeType;
class VrmlScene;

class VRMLEXPORT VrmlNodeWave : public VrmlNodeChild
{

public:
    // Define the built in VrmlNodeType:: "Wave"
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);
    virtual VrmlNodeType *nodeType() const;

    VrmlNodeWave(VrmlScene *);
    virtual ~VrmlNodeWave();

    // Copy the node.
    virtual VrmlNode *cloneMe() const;

    virtual VrmlNodeWave *toWave() const;

    virtual void addToScene(VrmlScene *s, const char *relativeUrl);

    virtual std::ostream &printFields(std::ostream &os, int indent);

    virtual void render(Viewer *);

    virtual void setField(const char *fieldName,
                          const VrmlField &fieldValue);
    const VrmlField *getField(const char *fieldName) const;

protected:
    VrmlSFFloat d_fraction;
    VrmlSFFloat d_speed1;
    VrmlSFFloat d_speed2;
    VrmlSFFloat d_freq1;
    VrmlSFFloat d_height1;
    VrmlSFFloat d_damping1;
    VrmlSFVec3f d_dir1;
    VrmlSFFloat d_freq2;
    VrmlSFFloat d_height2;
    VrmlSFFloat d_damping2;
    VrmlSFVec3f d_dir2;
    VrmlSFRotation d_coeffSin;
    VrmlSFRotation d_coeffCos;
    VrmlSFString d_fileName;
};
}
#endif //_VRMLNODEWave_
