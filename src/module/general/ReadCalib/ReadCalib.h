/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef _READ_DASIM_H
#define _READ_DASIM_H
/**************************************************************************\
**                                                   	      (C)2002 RUS **
**                                                                        **
** Description: READ SoundVol result files             	                  **
**                                                                        **
**                                                                        **
**                                                                        **
**                                                                        **
**                                                                        **
** Author: Uwe Woessner                                                   **                             **
**                                                                        **
\**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <api/coModule.h>
using namespace covise;

class ReadCalib : public coModule
{

private:
    //  member functions
    virtual int compute(const char *port);

    //  member data
    coOutputPort *p_meshSoll;
    coOutputPort *p_meshIst;
    coFileBrowserParam *p_GridPath;

public:
    ReadCalib(int argc, char *argv[]);
    virtual ~ReadCalib();
};

#endif
