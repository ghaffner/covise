/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef CO_POTITOOLBOXITEM_H
#define CO_POTITOOLBOXITEM_H

#include <OpenVRUI/coAction.h>
#include <OpenVRUI/coToolboxMenuItem.h>
#include <OpenVRUI/coValuePoti.h>

#include <string>

namespace vrui
{

class coLabel;

/** This class provides a coMenuItem which consists of a coValuePoti and a coLabel.
 */
class OPENVRUIEXPORT coPotiToolboxItem
    : public coToolboxMenuItem,
      public coValuePotiActor,
      public coAction
{
protected:
    coValuePoti *poti; ///< actual poti interactor
    coLabel *label;

public:
    coPotiToolboxItem(const std::string &symbolicName,
                      float min, float max, float defaultValue,
                      vruiCOIM * = 0, const std::string & = "");

    virtual ~coPotiToolboxItem();

    void setMax(float m);
    void setMin(float m);

    void setInteger(bool i);
    void setIncrement(float incr);

    float getValue() const;

    int hit(vruiHit *);
    void miss();

    virtual void selected(bool);
    virtual void doActionPress();
    virtual void doSecondActionPress();

    virtual void potiValueChanged(float, float, coValuePoti *, int context = -1);
    virtual void potiPressed(coValuePoti *, int context = -1);
    virtual void potiReleased(coValuePoti *, int context = -1);

    virtual bool updateContentRange(float min, float max, float value, bool isInteger, float step);
    virtual bool updateContentFloat(float value);
    bool updateContentPressed();
    bool updateContentReleased();

    /// get the Element's classname
    virtual const char *getClassName() const;
    /// check if the Element or any ancestor is this classname
    virtual bool isOfClassName(const char *) const;
};
}
#endif
