/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  %W% %G%
//  VrmlNodeProximitySensor.cpp

#include "config.h"
#include "VrmlNodeProximitySensor.h"
#include "VrmlNodeType.h"

#include "MathUtils.h"
#include "System.h"
#include "Viewer.h"
#include "VrmlScene.h"
#include "coEventQueue.h"

using namespace vrml;

// ProximitySensor factory.

static VrmlNode *creator(VrmlScene *scene)
{
    return new VrmlNodeProximitySensor(scene);
}

VrmlNodeProximitySensor *VrmlNodeProximitySensor::toProximitySensor() const { return (VrmlNodeProximitySensor *)this; }

// Define the built in VrmlNodeType:: "ProximitySensor" fields

VrmlNodeType *VrmlNodeProximitySensor::defineType(VrmlNodeType *t)
{
    static VrmlNodeType *st = 0;

    if (!t)
    {
        if (st)
            return st; // Only define the type once.
        t = st = new VrmlNodeType("ProximitySensor", creator);
    }

    VrmlNodeChild::defineType(t); // Parent class
    t->addExposedField("center", VrmlField::SFVEC3F);
    t->addExposedField("enabled", VrmlField::SFBOOL);
    t->addExposedField("size", VrmlField::SFVEC3F);
    t->addEventOut("enterTime", VrmlField::SFTIME);
    t->addEventOut("exitTime", VrmlField::SFTIME);
    t->addEventOut("isActive", VrmlField::SFBOOL);
    t->addEventOut("orientation_changed", VrmlField::SFROTATION);
    t->addEventOut("position_changed", VrmlField::SFVEC3F);

    return t;
}

VrmlNodeType *VrmlNodeProximitySensor::nodeType() const { return defineType(0); }

VrmlNodeProximitySensor::VrmlNodeProximitySensor(VrmlScene *scene)
    : VrmlNodeChild(scene)
    , d_center(0.0, 0.0, 0.0)
    , d_enabled(true)
    , d_size(0.0, 0.0, 0.0)
    , d_isActive(false)
    , d_position(0.0, 0.0, 0.0)
    , d_enterTime(0.0)
    , d_exitTime(0.0)
{
    forceTraversal(false);
    setModified();
}

VrmlNodeProximitySensor::~VrmlNodeProximitySensor()
{
}

VrmlNode *VrmlNodeProximitySensor::cloneMe() const
{
    return new VrmlNodeProximitySensor(*this);
}

const VrmlField *VrmlNodeProximitySensor::getField(const char *fieldName) const
{
    if (strcmp(fieldName, "center") == 0)
        return &d_center;
    else if (strcmp(fieldName, "size") == 0)
        return &d_size;
    else if (strcmp(fieldName, "enabled") == 0)
        return &d_enabled;
    else if (strcmp(fieldName, "isActive") == 0)
        return &d_isActive;
    else if (strcmp(fieldName, "position") == 0)
        return &d_position;
    else if (strcmp(fieldName, "orientation") == 0)
        return &d_orientation;
    else if (strcmp(fieldName, "enterTime") == 0)
        return &d_enterTime;
    else if (strcmp(fieldName, "exitTime") == 0)
        return &d_exitTime;

    return VrmlNodeChild::getField(fieldName);
}

std::ostream &VrmlNodeProximitySensor::printFields(std::ostream &os, int indent)
{
    if (!FPZERO(d_center.x()) || !FPZERO(d_center.y()) || !FPZERO(d_center.z()))
        PRINT_FIELD(center);
    if (!d_enabled.get())
        PRINT_FIELD(enabled);
    if (!FPZERO(d_size.x()) || !FPZERO(d_size.y()) || !FPZERO(d_size.z()))
        PRINT_FIELD(size);

    return os;
}

//
// Generate proximity events. If necessary, events prior to the current
// time are generated due to interpolation of enterTimes and exitTimes.
// The timestamp should never be increased.
//
// This is in a render() method since the it needs the viewer position
// with respect to the local coordinate system.
// Could do this with VrmlNode::inverseTransform(double [4][4]) now...
//
// The positions and times are not interpolated to report the exact
// place and time of entries and exits from the sensor regions as
// required by the spec, since that would require the last viewer position
// to be stored in the node, which is problematic in the presence of
// DEF/USEd nodes...
// I suppose the scene could keep the last viewer position in the global
// coordinate system and it could be transformed all the way down the
// scenegraph, but that sounds painful.

void VrmlNodeProximitySensor::render(Viewer *viewer)
{
    if (d_enabled.get() && d_size.x() > 0.0 && d_size.y() > 0.0 && d_size.z() > 0.0 && viewer->getRenderMode() == Viewer::RENDER_MODE_DRAW)
    {
        VrmlSFTime timeNow(System::the->time());
        float x, y, z;

        // Is viewer inside the box?
        viewer->getPosition(&x, &y, &z);
        bool inside = (fabs(x - d_center.x()) <= 0.5 * d_size.x() && fabs(y - d_center.y()) <= 0.5 * d_size.y() && fabs(z - d_center.z()) <= 0.5 * d_size.z());
        bool wasIn = d_isActive.get();

        // Check if viewer has entered the box
        if (inside && !wasIn)
        {
            System::the->debug("PS.%s::render ENTER %g %g %g\n", name(), x, y, z);

            d_isActive.set(true);
            eventOut(timeNow.get(), "isActive", d_isActive);

            d_enterTime = timeNow;
            eventOut(timeNow.get(), "enterTime", d_enterTime);
            double dpoint[3];
            dpoint[0] = (double)x;
            dpoint[1] = (double)y;
            dpoint[2] = (double)z;
            scene()->getSensorEventQueue()->addEvent(this, timeNow.get(), false, true, dpoint);
        }

        // Check if viewer has left the box
        else if (wasIn && !inside)
        {
            System::the->debug("PS.%s::render EXIT %g %g %g\n", name(), x, y, z);

            d_isActive.set(false);
            eventOut(timeNow.get(), "isActive", d_isActive);

            d_exitTime = timeNow;
            eventOut(timeNow.get(), "exitTime", d_exitTime);
            double dpoint[3];
            dpoint[0] = (double)x;
            dpoint[1] = (double)y;
            dpoint[2] = (double)z;
            scene()->getSensorEventQueue()->addEvent(this, timeNow.get(), false, false, dpoint);
        }

        // Check for movement within the box
        if (wasIn || inside)
        {
            if (!FPEQUAL(d_position.x(), x) || !FPEQUAL(d_position.y(), y) || !FPEQUAL(d_position.z(), z))
            {
                d_position.set(x, y, z);
                eventOut(timeNow.get(), "position_changed", d_position);
                double dpoint[3];
                dpoint[0] = (double)x;
                dpoint[1] = (double)y;
                dpoint[2] = (double)z;
                scene()->getSensorEventQueue()->addEvent(this, timeNow.get(), true, false, dpoint);
            }

            float xyzr[4];
            viewer->getOrientation(xyzr);
            if (!FPEQUAL(d_orientation.x(), xyzr[0]) || !FPEQUAL(d_orientation.y(), xyzr[1]) || !FPEQUAL(d_orientation.z(), xyzr[2]) || !FPEQUAL(d_orientation.r(), xyzr[3]))
            {
                d_orientation.set(xyzr[0], xyzr[1], xyzr[2], xyzr[3]);
                eventOut(timeNow.get(), "orientation_changed", d_orientation);
                double dpoint[4];
                dpoint[0] = (double)xyzr[0];
                dpoint[1] = (double)xyzr[1];
                dpoint[2] = (double)xyzr[2];
                dpoint[3] = (double)xyzr[3];
                scene()->getSensorEventQueue()->addEvent(this, timeNow.get(), true, true, dpoint);
            }
        }
    }

    else
        clearModified();
}

// Set the value of one of the node fields.

void VrmlNodeProximitySensor::setField(const char *fieldName,
                                       const VrmlField &fieldValue)
{
    if
        TRY_FIELD(center, SFVec3f)
    else if
        TRY_FIELD(enabled, SFBool)
    else if
        TRY_FIELD(size, SFVec3f)
    else
        VrmlNodeChild::setField(fieldName, fieldValue);
}

void VrmlNodeProximitySensor::remoteEvent(double timeStamp,
                                          bool isOver, bool isActive, float *point)
{
    if (isOver) // position changed
    {
        if (isActive) //orientation
        {
            d_orientation.set(point[0], point[1], point[2], point[3]);
            eventOut(timeStamp, "orientation_changed", d_orientation);
        }
        else // position
        {
            d_position.set(point[0], point[1], point[2]);
            eventOut(timeStamp, "position_changed", d_position);
        }
    }
    else if (isActive) // Entered
    {
        d_enterTime.set(timeStamp);
        eventOut(timeStamp, "enterTime", d_enterTime);
    }
    else // Left
    {
        d_exitTime.set(timeStamp);
        eventOut(timeStamp, "exitTime", d_exitTime);
    }
}
