/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include "coTetin__apprxCurve.h"
#include <string.h>

/// read from file
coTetin__apprxCurve::coTetin__apprxCurve(istream &str, int binary, ostream &ostr)
    : coTetinCommand(coTetin::APPROXIMATE_CURVE)
{
    return;
}

/// read from memory
coTetin__apprxCurve::coTetin__apprxCurve(int *&intDat, float *&floatDat, char *&charDat)
    : coTetinCommand(coTetin::APPROXIMATE_CURVE)
{
    apprx_family = *intDat++;
    n_names = *intDat++;
    names = 0;
    if (n_names > 0)
    {
        names = new char *[n_names];
        int i;
        for (i = 0; i < n_names; i++)
        {
            names[i] = getString(charDat);
        }
    }
}

coTetin__apprxCurve::coTetin__apprxCurve(int apprx_family_in,
                                         int n_names_in,
                                         const char **names_in)
    : coTetinCommand(coTetin::APPROXIMATE_CURVE)
{
    apprx_family = apprx_family_in;
    n_names = n_names_in;
    names = 0;
    if (n_names > 0)
    {
        names = new char *[n_names];
        int i;
        for (i = 0; i < n_names; i++)
        {
            int len = (names_in && names_in[i]) ? (strlen(names_in[i]) + 1) : 1;
            names[i] = new char[len];
            if (names_in && names_in[i])
            {
                strcpy(names[i], names_in[i]);
            }
            else
            {
                names[i][0] = '\0';
            }
        }
    }
}

/// Destructor
coTetin__apprxCurve::~coTetin__apprxCurve()
{
    int i;
    if (names)
    {
        for (i = 0; i < n_names; i++)
        {
            if (names[i])
                delete[] names[i];
        }
        delete[] names;
    }
    apprx_family = 0;
    names = 0;
    n_names = 0;
}

/// check whether Object is valid
int coTetin__apprxCurve::isValid() const
{
    if (d_comm)
        return 1;
    else
        return 0;
}

/// count size required in fields
void coTetin__apprxCurve::addSizes(int &numInt, int &numFloat, int &numChar) const
{
    // command name + n_names + apprx_family
    numInt += 3;
    int i;
    for (i = 0; i < n_names; i++)
    {
        numChar += (names && names[i]) ? (strlen(names[i]) + 1) : 1;
    }
}

/// put my data to a given set of pointers
void coTetin__apprxCurve::getBinary(int *&intDat, float *&floatDat, char *&charDat) const
{
    // copy the command's name
    *intDat++ = d_comm;

    // copy the data
    *intDat++ = apprx_family;
    *intDat++ = n_names;
    int i;
    for (i = 0; i < n_names; i++)
    {
        if (names && names[i])
        {
            strcpy(charDat, names[i]);
            charDat += strlen(names[i]) + 1;
        }
        else
        {
            *charDat++ = '\0';
        }
    }
}

/// print to a stream in Tetin format
void coTetin__apprxCurve::print(ostream &str) const
{
    return;
}

// ===================== command-specific functions =====================
