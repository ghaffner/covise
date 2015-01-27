/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef _CO_INT_SCALAR_PARAM_H_
#define _CO_INT_SCALAR_PARAM_H_

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +                                                                         +
// +  coIntScalarParam Parameter handling class                                +
// +                                                                         +
// +                           Andreas Werner                                +
// +         (C)  Computing Center University of Stuttgart                   +
// +                          Allmandring 30a                                +
// +                          70550 Stuttgart                                +
// + Date:  19.07.99                                                         +
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <covise/covise.h>
#include "coUifPara.h"

namespace covise
{

/// single integer parameter
class APIEXPORT coIntScalarParam : public coUifPara
{

private:
    /// Copy-Constructor: NOT  IMPLEMENTED
    coIntScalarParam(const coIntScalarParam &);

    /// Assignment operator: NOT  IMPLEMENTED
    coIntScalarParam &operator=(const coIntScalarParam &);

    /// Default constructor: NOT  IMPLEMENTED
    coIntScalarParam();

    /// my type info
    static coUifPara::Typeinfo s_paraType;

    /// Port data fields
    long d_value;

    // Parameter type name
    static const char *s_type;

public:
    /// Constructor
    coIntScalarParam(const char *name, const char *desc);

    /// Destructor : virtual in case we derive objects
    virtual ~coIntScalarParam();

    /// Check the type
    virtual int isOfType(coUifPara::Typeinfo type);

    /// get my type
    static coUifPara::Typeinfo getType();

    /// handle parameter changes: called by paramCB
    virtual int paramChange();

    /// give dafault values to Covise -> automatically called !
    virtual void initialize();

    /// print this to a stream
    virtual void print(ostream &str) const;

    /// set/update the value: return 0 on error
    int setValue(long val);

    /// get the value
    long getValue() const;

    /// get the type string of this parameter
    virtual const char *getTypeString() const;

    /// get the value of this parameter as a string
    virtual const char *getValString() const;

    /// set the value of this parameter from a string
    virtual void setValString(const char *str);
};
}
#endif
