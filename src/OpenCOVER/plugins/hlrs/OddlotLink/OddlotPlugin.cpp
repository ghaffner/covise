/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/****************************************************************************\ 
**                                                            (C)2009 HLRS  **
**                                                                          **
** Description: Oddlot Plugin (connection to the OpenDrive Road Editor)     **
**                                                                          **
**                                                                          **
** Author: U.Woessner		                                                **
**                                                                          **
** History:  								                                **
**                                                                          **
** Apr-16  v1	    				       		                            **
**                                                                          **
\****************************************************************************/

#include "OddlotPlugin.h"
#include <cover/coVRPluginSupport.h>
#include <cover/RenderObject.h>
#include <cover/coVRMSController.h>
#include <cover/coVRConfig.h>
#include <cover/coVRSelectionManager.h>
#include "cover/coVRTui.h"
#include <OpenVRUI/coCheckboxMenuItem.h>
#include <OpenVRUI/coButtonMenuItem.h>
#include <OpenVRUI/coSubMenuItem.h>
#include <OpenVRUI/coRowMenu.h>
#include <OpenVRUI/coCheckboxGroup.h>
#include <OpenVRUI/coButtonMenuItem.h>
#include <OpenVRUI/osg/OSGVruiUserDataCollection.h>
#include <OpenVRUI/osg/mathUtils.h>

#include <PluginUtil/PluginMessageTypes.h>


#include <osg/Geode>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/Array>
#include <osg/CullFace>
#include <osg/MatrixTransform>
#include <osg/lineSegment>
#include <osgUtil/IntersectVisitor>


#include <net/covise_host.h>
#include <net/covise_socket.h>
#include <net/tokenbuffer.h>
#include <config/CoviseConfig.h>

using covise::TokenBuffer;
using covise::coCoviseConfig;
void OddlotPlugin::createMenu()
{

   /* cbg = new coCheckboxGroup();
    viewpointMenu = new coRowMenu("Oddlot Viewpoints");

    REVITButton = new coSubMenuItem("Oddlot");
    REVITButton->setMenu(viewpointMenu);
    
    roomInfoMenu = new coRowMenu("Room Information");

    roomInfoButton = new coSubMenuItem("Room Info");
    roomInfoButton->setMenu(roomInfoMenu);
    viewpointMenu->add(roomInfoButton);
    label1 = new coLabelMenuItem("No Room");
    roomInfoMenu->add(label1);
    addCameraButton = new coButtonMenuItem("Add Camera");
    addCameraButton->setMenuListener(this);
    viewpointMenu->add(addCameraButton);
    updateCameraButton = new coButtonMenuItem("UpdateCamera");
    updateCameraButton->setMenuListener(this);
    viewpointMenu->add(updateCameraButton);

    cover->getMenu()->add(REVITButton);*/

    revitTab = new coTUITab("Oddlot", coVRTui::instance()->mainFolder->getID());
    revitTab->setPos(0, 0);

   /* updateCameraTUIButton = new coTUIButton("Update Camera", revitTab->getID());
    updateCameraTUIButton->setEventListener(this);
    updateCameraTUIButton->setPos(0, 0);

    addCameraTUIButton = new coTUIButton("Add Camera", revitTab->getID());
    addCameraTUIButton->setEventListener(this);
    addCameraTUIButton->setPos(0, 1);*/
}

void OddlotPlugin::destroyMenu()
{
  /*  delete roomInfoButton;
    delete roomInfoMenu;
    delete label1;
    delete viewpointMenu;
    delete REVITButton;
    delete cbg;

    delete addCameraTUIButton;
    delete updateCameraTUIButton;*/
    delete revitTab;
}

OddlotPlugin::OddlotPlugin()
{
    fprintf(stderr, "OddlotPlugin::OddlotPlugin\n");
    plugin = this;
    int port = coCoviseConfig::getInt("port", "COVER.Plugin.Oddlot.Server", 31822);
    toOddlot = NULL;
    serverConn = new ServerConnection(port, 1234, Message::UNDEFINED);
    if (!serverConn->getSocket())
    {
        cout << "tried to open server Port " << port << endl;
        cout << "Creation of server failed!" << endl;
        cout << "Port-Binding failed! Port already bound?" << endl;
        delete serverConn;
        serverConn = NULL;
    }

    struct linger linger;
    linger.l_onoff = 0;
    linger.l_linger = 0;
    cout << "Set socket options..." << endl;
    if (serverConn)
    {
        setsockopt(serverConn->get_id(NULL), SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));

        cout << "Set server to listen mode..." << endl;
        serverConn->listen();
        if (!serverConn->is_connected()) // could not open server port
        {
            fprintf(stderr, "Could not open server port %d\n", port);
            delete serverConn;
            serverConn = NULL;
        }
    }
    msg = new Message;

}

bool OddlotPlugin::init()
{
    //cover->addPlugin("Annotation"); // we would like to have the Annotation plugin
    createMenu();
    return true;
}
// this is called if the plugin is removed at runtime
OddlotPlugin::~OddlotPlugin()
{
    destroyMenu();
    delete serverConn;
    serverConn = NULL;
    delete toOddlot;
    delete msg;
    toOddlot = NULL;
}

void OddlotPlugin::menuEvent(coMenuItem *aButton)
{
    
}
void OddlotPlugin::tabletPressEvent(coTUIElement *tUIItem)
{
}

void OddlotPlugin::tabletEvent(coTUIElement *tUIItem)
{
}


void OddlotPlugin::sendMessage(Message &m)
{
    if(toOddlot) // false on slaves
    {
        toOddlot->send_msg(&m);
    }
}


void OddlotPlugin::message(int type, int len, const void *buf)
{
    if (type == PluginMessageTypes::MoveAddMoveNode)
    {
    }
    else if(type >= PluginMessageTypes::HLRS_Oddlot_Message && type <= (PluginMessageTypes::HLRS_Oddlot_Message+100))
    {
        Message m;
        m.type = type-PluginMessageTypes::HLRS_Oddlot_Message + MSG_GetHeight;
        m.length = len;
        m.data = (char *)buf;
        handleMessage(&m);
    }

}

OddlotPlugin *OddlotPlugin::plugin = NULL;
void
OddlotPlugin::handleMessage(Message *m)
{
    //cerr << "got Message" << endl;
    //m->print();
    enum MessageTypes type = (enum MessageTypes)m->type;
    
    switch (type)
    {
        
        case opencover::PluginMessageTypes::HLRS_Oddlot_Message:
        {
            TokenBuffer tb(m);
            int t;
            tb >> t;
            float _scale = cover->getScale();
            switch(t)
            {
            case MSG_GetHeight:
                {
                    osg::Matrix oldXformMat=cover->getXformMat();
                    cover->setXformMat(osg::Matrix());
                    int numPoints;
                    tb >> numPoints;
                    TokenBuffer rtb;
                    rtb << MSG_GetHeight;
                    rtb << numPoints;
                    for(int i=0;i<numPoints;i++)
                    {
                        float x; float y;
                        tb >> x;
                        tb >> y;
                        x*=(1000.0);
                        y*=(1000.0);

                        double minHeightValue = 100000000.0;
                        double maxHeightValue = -100000000.0;

                        osg::LineSegment *ray = new osg::LineSegment();

                        osg::Vec3 rayP = osg::Vec3(x, y, 9999999);
                        osg::Vec3 rayQ = osg::Vec3(x, y, -9999999);

                        ray->set(rayP, rayQ);

                        osgUtil::IntersectVisitor intersectVisitor;
                        intersectVisitor.addLineSegment(ray);

                        cover->getObjectsXform()->accept(intersectVisitor);

                        osgUtil::IntersectVisitor::HitList hits;
                        hits = intersectVisitor.getHitList(ray);

                        if (hits.empty())
                        {
                            rtb << 0.0f;
                        }
                        else
                        {
                            osgUtil::Hit results;
                            results = hits.front();
                            osg::Vec3d terrainHeight = results.getWorldIntersectPoint();

                            double height = terrainHeight.z() / 1000.0;
                            if (height < minHeightValue)
                            {
                                minHeightValue = height;
                            }
                            if (height > maxHeightValue)
                            {
                                maxHeightValue = height;
                            }

                            rtb << (float)height;
                        }
                    }
                    Message m(rtb);
                    m.type = PluginMessageTypes::HLRS_Oddlot_Message;
                    sendMessage(m);
                    cover->setXformMat(oldXformMat);
                }
                break;
            case MSG_GetMap:
                {
                    float x,y,width,height;
                    tb >> x;
                    tb >> y;
                    tb >> width;
                    tb >> height;
                    fprintf(stderr," x: %f  y: %f width: %f height: %f\n",x,y,width,height);
                }
                break;

            default:
                cerr << "Unknown Oddlot to COVER message " << t << endl;
                break;
            }
        }
        break;
        
    
        
    default:
        switch (m->type)
        {
        case Message::SOCKET_CLOSED:
        case Message::CLOSE_SOCKET:
            delete toOddlot;
            toOddlot = NULL;

            cerr << "connection to Oddlot closed" << endl;
            break;
        default:
            cerr << "Unknown Oddlot message " << m->type << endl;
            break;
        }
    }
}

void
OddlotPlugin::preFrame()
{
    if (serverConn && serverConn->is_connected() && serverConn->check_for_input()) // we have a server and received a connect
    {
        //   std::cout << "Trying serverConn..." << std::endl;
        toOddlot = serverConn->spawn_connection();
        if (toOddlot && toOddlot->is_connected())
        {
            fprintf(stderr, "Connected to Oddlot\n");
        }
    }
    char gotMsg = '\0';
    if (coVRMSController::instance()->isMaster())
    {
        if(toOddlot)
        {
            static double lastTime = 0;
            if(cover->frameTime() > lastTime+4)
            {
                lastTime = cover->frameTime();
                
            }
        }
        while (toOddlot && toOddlot->check_for_input())
        {
            toOddlot->recv_msg(msg);
            if (msg)
            {
                gotMsg = '\1';
                coVRMSController::instance()->sendSlaves(&gotMsg, sizeof(char));
                coVRMSController::instance()->sendSlaves(msg);
                
                cover->sendMessage(this, coVRPluginSupport::TO_SAME_OTHERS,PluginMessageTypes::HLRS_Oddlot_Message+msg->type-MSG_GetHeight,msg->length, msg->data);
                handleMessage(msg);
            }
            else
            {
                gotMsg = '\0';
                cerr << "could not read message" << endl;
                break;
            }
        }
        gotMsg = '\0';
        coVRMSController::instance()->sendSlaves(&gotMsg, sizeof(char));
    }
    else
    {
        do
        {
            coVRMSController::instance()->readMaster(&gotMsg, sizeof(char));
            if (gotMsg != '\0')
            {
                coVRMSController::instance()->readMaster(msg);
                handleMessage(msg);
            }
        } while (gotMsg != '\0');
    }
}

COVERPLUGIN(OddlotPlugin)