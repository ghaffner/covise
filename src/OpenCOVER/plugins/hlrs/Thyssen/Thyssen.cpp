/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

//
//

#include "Thyssen.h"

#include <net/covise_host.h>
#include <net/covise_socket.h>

using namespace covise;

ThyssenPlugin *ThyssenPlugin::plugin = NULL;

static VrmlNode *creator(VrmlScene *scene)
{
    return new VrmlNodeThyssen(scene);
}

// Define the built in VrmlNodeType:: "Thyssen" fields

VrmlNodeType *VrmlNodeThyssen::defineType(VrmlNodeType *t)
{
    static VrmlNodeType *st = 0;

    if (!t)
    {
        if (st)
            return st; // Only define the type once.
        t = st = new VrmlNodeType("Thyssen", creator);
    }

    VrmlNodeChild::defineType(t); // Parent class
    t->addExposedField("enabled", VrmlField::SFBOOL);
    t->addEventOut("car0Pos", VrmlField::SFVEC3F);
    t->addEventOut("ints_changed", VrmlField::MFINT32);
    t->addEventOut("floats_changed", VrmlField::MFFLOAT);

    return t;
}

VrmlNodeType *VrmlNodeThyssen::nodeType() const
{
    return defineType(0);
}

VrmlNodeThyssen::VrmlNodeThyssen(VrmlScene *scene)
    : VrmlNodeChild(scene)
    , d_enabled(true)
{
    setModified();
}

VrmlNodeThyssen::VrmlNodeThyssen(const VrmlNodeThyssen &n)
    : VrmlNodeChild(n.d_scene)
    , d_enabled(n.d_enabled)
{
    setModified();
}

VrmlNodeThyssen::~VrmlNodeThyssen()
{
}

VrmlNode *VrmlNodeThyssen::cloneMe() const
{
    return new VrmlNodeThyssen(*this);
}

VrmlNodeThyssen *VrmlNodeThyssen::toThyssen() const
{
    return (VrmlNodeThyssen *)this;
}

ostream &VrmlNodeThyssen::printFields(ostream &os, int indent)
{
    if (!d_enabled.get())
        PRINT_FIELD(enabled);

    return os;
}

// Set the value of one of the node fields.

void VrmlNodeThyssen::setField(const char *fieldName,
                               const VrmlField &fieldValue)
{
    if
        TRY_FIELD(enabled, SFBool)
    else
        VrmlNodeChild::setField(fieldName, fieldValue);
}

const VrmlField *VrmlNodeThyssen::getField(const char *fieldName)
{
    if (strcmp(fieldName, "enabled") == 0)
        return &d_enabled;
    else if (strcmp(fieldName, "floats_changed") == 0)
        return &d_floats;
    else if (strcmp(fieldName, "ints_changed") == 0)
        return &d_ints;
    else
        cerr << "Node does not have this eventOut or exposed field " << nodeType()->getName() << "::" << name() << "." << fieldName << endl;
    return 0;
}

void VrmlNodeThyssen::eventIn(double timeStamp,
                              const char *eventName,
                              const VrmlField *fieldValue)
{

    // Check exposedFields
    //else
    {
        VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }

    setModified();
}

void VrmlNodeThyssen::render(Viewer *)
{
    if (!d_enabled.get())
        return;

    double timeStamp = System::the->time();
    if (ThyssenPlugin::plugin->numFloats)
    {
        d_floats.set(ThyssenPlugin::plugin->numFloats, ThyssenPlugin::plugin->floatValues);
        eventOut(timeStamp, "floats_changed", d_floats);
    }
    if (ThyssenPlugin::plugin->numInts)
    {
        d_ints.set(ThyssenPlugin::plugin->numInts, ThyssenPlugin::plugin->intValues);
        eventOut(timeStamp, "ints_changed", d_ints);
    }

    d_car0Pos.set(ThyssenPlugin::plugin->ypos / 1000.0, ThyssenPlugin::plugin->zpos / 1000.0, 0);
    eventOut(timeStamp, "car0Pos", d_car0Pos);
}

ThyssenPlugin::ThyssenPlugin()
{
    fprintf(stderr, "ThyssenPlugin::ThyssenPlugin\n");

    plugin = this;
    conn = NULL;

    port = coCoviseConfig::getInt("COVER.Plugin.ThyssenPlugin.TCPPort", 12345);

    sConn = new ServerConnection(port, 1234, 0);

    if (!sConn->getSocket())
    {
        cout << "tried to open server Port " << port << endl;
        cout << "Creation of server failed!" << endl;
        cout << "Port-Binding failed! Port already bound?" << endl;
        delete sConn;
        sConn = NULL;
    }
    struct linger linger;
    linger.l_onoff = 0;
    linger.l_linger = 0;
    cout << "Set socket options..." << endl;
    if (sConn)
    {
        setsockopt(sConn->get_id(NULL), SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));

        cout << "Set server to listen mode..." << endl;
        sConn->listen();
        if (!sConn->is_connected()) // could not open server port
        {
            fprintf(stderr, "Could not open server port %d\n", port);
            delete sConn;
            sConn = NULL;
        }
    }

    numFloats = 62;
    numInts = 61;

    floatValues = new float[numFloats];
    intValues = new int[numInts];
}

// this is called if the plugin is removed at runtime
ThyssenPlugin::~ThyssenPlugin()
{
    fprintf(stderr, "ThyssenPlugin::~ThyssenPlugin\n");

    delete[] floatValues;
    delete[] intValues;
    delete conn;
}

bool ThyssenPlugin::init()
{
    VrmlNamespace::addBuiltIn(VrmlNodeThyssen::defineType());

    return true;
}

bool ThyssenPlugin::readVal(void *buf, unsigned int numBytes)
{
    unsigned int toRead = numBytes;
    unsigned int numRead = 0;
    int readBytes = 0;
    while (numRead < numBytes)
    {
        readBytes = conn->getSocket()->Read(((unsigned char *)buf) + readBytes, toRead);
        if (readBytes < 0)
        {
            cerr << "error reading data from socket" << endl;
            return false;
        }
        numRead += readBytes;
        toRead = numBytes - numRead;
    }
    return true;
}

void
ThyssenPlugin::preFrame()
{

    if (sConn && sConn->is_connected() && sConn->check_for_input()) // we have a server and received a connect
    {
        //   std::cout << "Trying serverConn..." << std::endl;
        conn = sConn->spawnSimpleConnection();
        if (conn && conn->is_connected())
        {
            std::cerr << "Client connected" << endl;
        }
    }

    if (conn)
    {
        while (conn->check_for_input())
        {
            const char *line = conn->readLine();
            if (line != NULL)
            {
                if (strncmp(line, "CARID0", 6) == 0)
                {
                    sscanf(line + 12, "%f, PosY %f", &zpos, &ypos);

                    //cerr << line <<  "z" << zpos << endl;
                }

                /*
				ExCHID4 PosZ 153850.000000 ,  PosY 3000.000000
CARID0 PosZ 113020.000000 ,  PosY 3000.000000
CARID1 PosZ 153850.000000 ,  PosY 3000.000000
CARID2 PosZ 5000.000000 ,  PosY 3000.000000
CARID3 PosZ 34420.000000 ,  PosY 3000.000000
ExCHID0 PosZ 5000.000000 ,  PosY 3000.000000
ExCHID1 PosZ 34420.000000 ,  PosY 3000.000000
ExCHID2 PosZ 44200.000000 ,  PosY 5700.000000
ExCHID3 PosZ 94200.000000 ,  PosY 3000.000000
ExCHID4 PosZ 153850.000000 ,  PosY 3000.000000
CARID0 PosZ 113050.000000 ,  PosY 3000.000000
CARID1 PosZ 153850.000000 ,  PosY 3000.000000
CARID2 PosZ 5000.000000 ,  PosY 3000.000000
CARID3 PosZ 34420.000000 ,  PosY 3000.000000
ExCHID0 PosZ 5000.000000 ,  PosY 3000.000000
ExCHID1 PosZ 34420.000000 ,  PosY 3000.000000
ExCHID2 PosZ 44200.000000 ,  PosY 5700.000000
ExCHID3 PosZ 94200.000000 ,  PosY 3000.000000
ExCHID4 PosZ 153850.000000 ,  PosY 3000.000000
CARID0 PosZ 113230.000000 ,  PosY 3000.000000*/
            }
            else
            {
                /*delete conn;
				conn = NULL;
			    std::cerr << "Client disconnected" << endl;*/
            }
        }
    }

    if (coVRMSController::instance()->isMaster())
    {
        //cerr << numFloats << endl;
        //cerr << numInts << endl;
        /*   coVRMSController::instance()->sendSlaves((char *)&numFloats,sizeof(int));
      coVRMSController::instance()->sendSlaves((char *)&numInts,sizeof(int));
      if(numFloats)
         coVRMSController::instance()->sendSlaves((char *)floatValues,numFloats*sizeof(float));
      if(numInts)
         coVRMSController::instance()->sendSlaves((char *)intValues,numInts*sizeof(int));*/
    }
    else
    {
        /*  int newNumFloats=0;
      int newNumInts=0;
      coVRMSController::instance()->readMaster((char *)&newNumFloats,sizeof(int));
      coVRMSController::instance()->readMaster((char *)&newNumInts,sizeof(int));
      //cerr << newNumFloats << endl;
      //cerr << newNumInts << endl;
      if(newNumFloats>0 && newNumFloats != numFloats)
      {
      cerr << "resize" << endl;
         numFloats=newNumFloats;
         delete[] floatValues;
         floatValues = new float[numFloats];
      }
      if(newNumInts > 0 && newNumInts != numInts)
      {
      cerr << "resize" << endl;
         numInts=newNumInts;
         delete[] intValues;
         intValues = new int[numInts];
      }
      if(newNumFloats>0 && numFloats)
      {
      //cerr << "rf" << endl;
         coVRMSController::instance()->readMaster((char *)floatValues,numFloats*sizeof(float));
      }
      if(newNumFloats>0 && numInts)
      {
      //cerr << "ri" << endl;
         coVRMSController::instance()->readMaster((char *)intValues,numInts*sizeof(int));
      }*/
    }
}

COVERPLUGIN(ThyssenPlugin)
