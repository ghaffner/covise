/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  %W% %G%
//  VrmlNodeTouchSensor.h

#ifndef _VRMLNODETOUCHSENSOR_
#define _VRMLNODETOUCHSENSOR_

#include "VrmlNode.h"
#include "VrmlSFBool.h"
#include "VrmlSFTime.h"
#include "VrmlSFVec2f.h"
#include "VrmlSFVec3f.h"

#include "VrmlNodeChild.h"

namespace vrml
{

class VRMLEXPORT VrmlScene;

class VRMLEXPORT VrmlNodeTouchSensor : public VrmlNodeChild
{

public:
    // Define the fields of TouchSensor nodes
    static VrmlNodeType *defineType(VrmlNodeType *t = 0);
    virtual VrmlNodeType *nodeType() const;

    VrmlNodeTouchSensor(VrmlScene *scene = 0);
    virtual ~VrmlNodeTouchSensor();

    virtual VrmlNode *cloneMe() const;

    virtual VrmlNodeTouchSensor *toTouchSensor() const;

    virtual std::ostream &printFields(std::ostream &os, int indent);

    void activate(double timeStamp, bool isOver, bool isActive, double *p);

    virtual void setField(const char *fieldName, const VrmlField &fieldValue);
    virtual const VrmlField *getField(const char *fieldName) const;

    bool isEnabled()
    {
        return d_enabled.get();
    }

private:
    // Fields
    VrmlSFBool d_enabled;

    // Internal state
    VrmlSFVec3f d_hitNormal_changed;
    VrmlSFVec3f d_hitPoint_changed;
    VrmlSFVec2f d_hitTexCoord_changed;
    VrmlSFBool d_isActive;
    VrmlSFBool d_isOver;
    VrmlSFTime d_touchTime;
};
}
#endif //_VRMLNODETOUCHSENSOR_
