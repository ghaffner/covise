/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "SurfaceDemo.h"
#include "include/model.h"
#include <api/coFeedback.h>

#include <config/CoviseConfig.h>
#include <do/coDoUnstructuredGrid.h>
#include <do/coDoIntArr.h>
#include <do/coDoData.h>
#include <do/coDoSet.h>

#define RAD(x) ((x)*M_PI / 180.0)
#define GRAD(x) ((x)*180.0 / M_PI)

#define MAX_CUBES 12

SurfaceDemo::SurfaceDemo(int argc, char *argv[])
    : coModule(argc, argv, "SurfaceDemo")
{
    geo = NULL;

//   fprintf(stderr, "SurfaceDemo::SurfaceDemo()\n");

#ifdef USE_STARTFILE
    // start file param

    //   fprintf(stderr, "SurfaceDemo::SurfaceDemo() Init of StartFile\n");
    startFile = addFileBrowserParam("startFile", "Start file");
    startFile->setValue(coCoviseConfig::getEntry("value", "Module.SurfaceDemo.DataPath", getenv("HOME")), "*.txt");
#endif

    // We build the User-Menue ...
    SurfaceDemo::CreateUserMenu();

    // the output ports
    //   fprintf(stderr, "SurfaceDemo::SurfaceDemo() SetOutPort\n");
    grid = addOutputPort("grid", "UnstructuredGrid", "Computation Grid");
    surface = addOutputPort("surface", "Polygons", "Surface Polygons");
    bcin = addOutputPort("bcin", "Polygons", "Cells at entry");
    bcout = addOutputPort("bcout", "Polygons", "Cells at exit");
    bcwall = addOutputPort("bcwall", "Polygons", "Cells at walls");
    boco = addOutputPort("boco", "USR_FenflossBoco", "Boundary Conditions");
    intypes = addOutputPort("inletbctype", "Float", "0: cluster, 1-n: floor square type");
    bccheck = addOutputPort("bccheck", "Polygons", "can be used to check bc polygons");

    inpoints = addOutputPort("InbcNodes", "Points", "inbc nodes");
    feedback_info = addOutputPort("FeedbackInfo", "Points", "Feedback Info");

    model = NULL;
    rg = NULL;

    isInitialized = 0;
}

void SurfaceDemo::postInst()
{
#ifdef USE_STARTFILE
    startFile->show();
#endif
    p_makeGrid->show();
    p_lockmakeGrid->show();
    p_gridSpacing->show();
    p_nobjects->show();
    p_model_size->show();
    p_Q_total->show();
    p_BCFile->show();
    p_geofile->show();
    p_rbfile->show();
}

void SurfaceDemo::param(const char *portname, bool)
{
    if (!(strncmp(portname, "flowrate_rack", 12)))
    {
        if (model)
        {
            int num = atoi(portname + 12) - 1;
            model->cubes[num]->Q = p_flowrate[num]->getValue();
            fprintf(stderr, "changed Q[num] to %8.5f\n", p_flowrate[num]->getValue());
        }
    }

    if (!(strncmp(portname, "size_rack_", 10)))
    {
        if (model)
        {
            int num = atoi(portname + 10) - 1;
            model->cubes[num]->size[0] = p_cubes_size[num]->getValue(0);
            model->cubes[num]->size[1] = p_cubes_size[num]->getValue(1);
            model->cubes[num]->size[2] = p_cubes_size[num]->getValue(2);
        }
    }

    if (!(strncmp(portname, "pos_rack_", 9)))
    {
        if (model)
        {
            int num = atoi(portname + 9) - 1;
            model->cubes[num]->pos[0] = p_cubes_pos[num]->getValue(0);
            model->cubes[num]->pos[1] = p_cubes_pos[num]->getValue(1);
            model->cubes[num]->pos[2] = p_cubes_pos[num]->getValue(2);
        }
    }

    if (!(strncmp(portname, "p_Q_total", 9)))
    {
        if (model)
        {
            model->Q_total = p_Q_total->getValue();
        }
    }

#ifdef USE_STARTFILE
    if (strcmp(portname, "startFile") == 0)
    {
        if (isInitialized)
        {
            sendError("We Had an input file before...");
            return;
        }
        Covise::getname(buf, startFile->getValue());
        if (strlen(buf) == 0)
        {
            Covise::sendError("startFile parameter incorrect");
        }
        else
        {
            fprintf(stderr, "SurfaceDemo::param = ReadGeometry(%s) ...", buf);
            model = ReadStartfile(buf);
            if (model == NULL)
            {
                Covise::sendError("ReadStartfile error");
                return;
            }
        }
    }
#endif

    //selfExec();
}

void SurfaceDemo::quit()
{
    // :-)
}

int SurfaceDemo::compute(const char *)
{

    coDoPolygons *poly;
    int i;

    char name[256];

//   fprintf(stderr, "SurfaceDemo::compute(const char *) entering... \n");

#ifdef USE_STARTFILE
    char buf[256];
    Covise::getname(buf, startFile->getValue());
//   fprintf(stderr, "SurfaceDemo::param = ReadGeometry(%s) ...", buf);
#endif
    if (model != NULL)
        FreeModel(model);
    model = AllocModel();

#ifdef USE_STARTFILE
    ReadStartfile(buf, model);
#else
    GetParamsFromControlPanel(model);
#endif

    if (!model)
    {
        sendError("Please select a parameter file first!!");
        return FAIL;
    }

    //// Cover plugin information object is created here
    createFeedbackObjects();

    /////////////////////////////
    // create geometry for COVISE
    if ((ci = CreateGeometry4Covise(model)))
    {
        //      fprintf(stderr, "SurfaceDemo::compute(const char *): Geometry created\n");

        poly = new coDoPolygons(surface->getObjName(),
                                ci->p->nump,
                                ci->p->x, ci->p->y, ci->p->z,
                                ci->vx->num, ci->vx->list,
                                ci->pol->num, ci->pol->list);
        poly->addAttribute("MATERIAL", "metal metal.30");
        poly->addAttribute("vertexOrder", "1");
        surface->setCurrentObject(poly);
    }
    else
        fprintf(stderr, "Error in CreateGeometry4Covise (%s, %d)\n", __FILE__, __LINE__);

    //////////////// This creates the volume grid ////////////////////

    ////////////////////////
    // if button is pushed --> create computation grid
    if (p_makeGrid->getValue())
    {
        int size[2];

        if (p_lockmakeGrid->getValue() == 0)
            p_makeGrid->setValue(0); // push off button

        if (model == NULL)
        {
            sendError("Cannot create grid because model is NULL!");
            return (1);
        }
        model->spacing = p_gridSpacing->getValue();
        model->BCFile = p_BCFile->getValue();

        if ((rg = CreateRechGrid(model)) == NULL)
        {
            fprintf(stderr, "Error in CreateRechGrid!\n");
            return -1;
        }

        rg->bc_inval = 100;
        rg->bc_outval = 200;

        //      fprintf(stderr, "SurfaceDemo: Grid created\n");

        coDoUnstructuredGrid *unsGrd = new coDoUnstructuredGrid(grid->getObjName(), // name of USG object
                                                                rg->e->nume, // number of elements
                                                                8 * rg->e->nume, // number of connectivities
                                                                rg->p->nump, // number of coordinates
                                                                1); // does type list exist?

        int *elem, *conn, *type;
        float *xc, *yc, *zc;
        unsGrd->getAddresses(&elem, &conn, &xc, &yc, &zc);
        unsGrd->getTypeList(&type);

        //      printf("nelem  = %d\n", rg->e->nume);
        //      printf("nconn  = %d\n", 8*rg->e->nume);
        //      printf("nccord = %d\n", rg->p->nump);

        int **GgridConn = rg->e->e;
        for (i = 0; i < rg->e->nume; i++)
        {
            *elem = 8 * i;
            elem++;

            *conn = (*GgridConn)[0];
            conn++;
            *conn = (*GgridConn)[1];
            conn++;
            *conn = (*GgridConn)[2];
            conn++;
            *conn = (*GgridConn)[3];
            conn++;
            *conn = (*GgridConn)[4];
            conn++;
            *conn = (*GgridConn)[5];
            conn++;
            *conn = (*GgridConn)[6];
            conn++;
            *conn = (*GgridConn)[7];
            conn++;

            *type = TYPE_HEXAGON;
            type++;

            GgridConn++;
        }

        // copy geometry coordinates to unsgrd
        memcpy(xc, rg->p->x, rg->p->nump * sizeof(float));
        memcpy(yc, rg->p->y, rg->p->nump * sizeof(float));
        memcpy(zc, rg->p->z, rg->p->nump * sizeof(float));

        // no blades
        // no periodic mesh
        // no rotating mesh
        // ...
        unsGrd->addAttribute("number_of_blades", "0");
        unsGrd->addAttribute("periodic", "0");
        unsGrd->addAttribute("rotating", "0");
        unsGrd->addAttribute("revolutions", "0");
        unsGrd->addAttribute("walltext", "");
        unsGrd->addAttribute("periotext", "");

        // set out port
        grid->setCurrentObject(unsGrd);

        // boundary condition lists
        // 1. Cells at walls
        poly = new coDoPolygons(bcwall->getObjName(),
                                rg->p->nump,
                                rg->p->x, rg->p->y, rg->p->z,
                                rg->bcwall->num, rg->bcwall->list,
                                rg->bcwallpol->num, rg->bcwallpol->list);
        //poly->addAttribute("MATERIAL","metal metal.30");
        poly->addAttribute("vertexOrder", "1");
        bcwall->setCurrentObject(poly);

        // 2. Cells at outlet
        poly = new coDoPolygons(bcout->getObjName(),
                                rg->p->nump,
                                rg->p->x, rg->p->y, rg->p->z,
                                rg->bcout->num, rg->bcout->list,
                                rg->bcoutpol->num, rg->bcoutpol->list);
        //poly->addAttribute("MATERIAL","metal metal.30");
        poly->addAttribute("vertexOrder", "1");
        bcout->setCurrentObject(poly);

        // 3. Cells at inlet
        poly = new coDoPolygons(bcin->getObjName(),
                                rg->p->nump,
                                rg->p->x, rg->p->y, rg->p->z,
                                rg->bcin->num, rg->bcin->list,
                                rg->bcinpol->num, rg->bcinpol->list);
        //poly->addAttribute("MATERIAL","metal metal.30");
        poly->addAttribute("vertexOrder", "1");
        bcin->setCurrentObject(poly);

        /*
std::vector<int> checkbcs_vl;
std::vector<int> checkbcs_pl;  
for (i=0;i<rg->bcinpol->num;i++)
{
   if (rg->bcin_type[i]==107)
   {
      fprintf(stderr,"%d: %d %d %d %d %d\n",i,rg->bcin->list[4*i+0],rg->bcin->list[4*i+1],rg->bcin->list[4*i+2],rg->bcin->list[4*i+3],rg->bcin_type[i]);
      checkbcs_pl.push_back(checkbcs_vl.size());
      checkbcs_vl.push_back(rg->bcin->list[4*i+0]);
      checkbcs_vl.push_back(rg->bcin->list[4*i+1]);
      checkbcs_vl.push_back(rg->bcin->list[4*i+2]);
      checkbcs_vl.push_back(rg->bcin->list[4*i+3]);
   }
}
poly = new coDoPolygons(bccheck->getObjName(),
                        rg->p->nump,
                        rg->p->x, rg->p->y, rg->p->z,                       
                        checkbcs_vl.size(), &checkbcs_vl[0],
                        checkbcs_pl.size(), &checkbcs_pl[0]);
bccheck->setCurrentObject(poly);
*/

        // we had several additional info, we should send to the
        // Domaindecomposition:
        //   0. number of columns per info
        //   1. type of node
        //   2. type of element
        //   3. list of nodes with bc (a node may appear more than one time)
        //   4. corresponding type to 3.
        //   5. wall
        //   6. balance
        //   7. pressure
        //   8. NULL

        coDistributedObject *partObj[10];
        int *data;
        float *bPtr;
        const char *basename = boco->getObjName();

        //   0. number of columns per info
        sprintf(name, "%s_colinfo", basename);
        size[0] = 6;
        size[1] = 0;
        coDoIntArr *colInfo = new coDoIntArr(name, 1, size);
        data = colInfo->getAddress();
        data[0] = RG_COL_NODE; // (=2)
        data[1] = RG_COL_ELEM; // (=2)
        data[2] = RG_COL_DIRICLET; // (=2)
        data[3] = RG_COL_WALL; // (=6)
        data[4] = RG_COL_BALANCE; // (=6)
        data[5] = RG_COL_PRESS; // (=6)
        partObj[0] = colInfo;

        //   1. type of node
        sprintf(name, "%s_nodeinfo", basename);
        size[0] = RG_COL_NODE;
        size[1] = rg->p->nump;
        coDoIntArr *nodeInfo = new coDoIntArr(name, 2, size);
        data = nodeInfo->getAddress();
        for (i = 0; i < rg->p->nump; i++)
        {
            *data++ = i + 1; // may be, that we later do it really correct
            *data++ = 0; // same comment ;-)
        }
        partObj[1] = nodeInfo;

        //   2. type of element
        sprintf(name, "%s_eleminfo", basename);
        size[0] = 2;
        size[1] = rg->e->nume * RG_COL_ELEM; // uwe: hier wird 4*nume allociert aber nur 2*nume mit Werten gefuellt
        coDoIntArr *elemInfo = new coDoIntArr(name, 2, size);
        data = elemInfo->getAddress();
        for (i = 0; i < rg->e->nume; i++)
        {
            *data++ = i + 1; // may be, that we later do it really corect
            *data++ = 0; // same comment ;-)
        }
        partObj[2] = elemInfo;

        //   3. list of nodes with bc (a node may appear more than one time)
        //      and its types
        sprintf(name, "%s_diricletNodes", basename);
        int num_diriclet = (int)rg->bcin_nodes.size();

        size[0] = RG_COL_DIRICLET;
        size[1] = 5 * (num_diriclet);
        coDoIntArr *diricletNodes = new coDoIntArr(name, 2, size);
        data = diricletNodes->getAddress();

        //   4. corresponding value to 3.
        sprintf(name, "%s_diricletValue", basename);
        coDoFloat *diricletValues = new coDoFloat(name, 5 * num_diriclet);
        diricletValues->getAddress(&bPtr);

        for (i = 0; i < rg->bcin_nodes.size(); i++)
        {
            *data++ = rg->bcin_nodes[i] + 1; // node-number
            *data++ = 1; // type of node
            *bPtr++ = rg->bcin_velos[5 * i + 0]; // u

            *data++ = rg->bcin_nodes[i] + 1; // node-number
            *data++ = 2; // type of node
            *bPtr++ = rg->bcin_velos[5 * i + 1]; // v

            *data++ = rg->bcin_nodes[i] + 1; // node-number
            *data++ = 3; // type of node
            *bPtr++ = rg->bcin_velos[5 * i + 2]; // w

            *data++ = rg->bcin_nodes[i] + 1; // node-number
            *data++ = 4; // type of node
            *bPtr++ = rg->bcin_velos[5 * i + 4]; // epsilon

            *data++ = rg->bcin_nodes[i] + 1; // node-number
            *data++ = 5; // type of node
            *bPtr++ = rg->bcin_velos[5 * i + 3]; // k

            //*data++ = rg->bcin_nodes->list[i]+1;     // node-number
            //*data++ = 6;                             // type of node
            //*bPtr++ = 0.0;                           // temperature = 0.
        }

        partObj[3] = diricletNodes;
        partObj[4] = diricletValues;

        //   5. wall
        sprintf(name, "%s_wallValue", basename);
        coDoFloat *wallValues = new coDoFloat(name, rg->bcwallvol->num);
        wallValues->getAddress(&bPtr);
        size[0] = RG_COL_WALL;
        size[1] = rg->bcwallpol->num;
        sprintf(name, "%s_wall", basename);
        coDoIntArr *faces = new coDoIntArr(name, 2, size);
        data = faces->getAddress();
        for (i = 0; i < rg->bcwallpol->num; i++) // Achtung bcwall->pol->num != bcwall->vol->num
        {
            *data++ = rg->bcwall->list[4 * i + 0] + 1;
            *data++ = rg->bcwall->list[4 * i + 1] + 1;
            *data++ = rg->bcwall->list[4 * i + 2] + 1;
            *data++ = rg->bcwall->list[4 * i + 3] + 1;
            *data++ = rg->bcwallvol->list[i] + 1;
            *data++ = 55; // wall: moving | standing. here: always standing!
            *data++ = 0;
        }
        partObj[5] = faces;

        //   6. balance
        sprintf(name, "%s_balance", basename);
        size[0] = RG_COL_BALANCE;
        size[1] = (int)rg->bcinvol.size() + rg->bcoutvol->num;

        coDoIntArr *balance = new coDoIntArr(name, 2, size);
        data = balance->getAddress();
        for (i = 0; i < rg->bcinvol.size(); i++)
        {
            /*
   if (rg->bcin_type[i]==107)
   {
      fprintf(stderr,"balance107 %d: %d %d %d %d %d\n",i,rg->bcin->list[4*i+0],rg->bcin->list[4*i+1],rg->bcin->list[4*i+2],rg->bcin->list[4*i+3],rg->bcin_type[i]);
   }
*/
            *data++ = rg->bcin->list[4 * i + 0] + 1;
            *data++ = rg->bcin->list[4 * i + 1] + 1;
            *data++ = rg->bcin->list[4 * i + 2] + 1;
            *data++ = rg->bcin->list[4 * i + 3] + 1;
            *data++ = rg->bcinvol[i] + 1;
            //*data++ = rg->bc_inval;
            *data++ = rg->bcin_type[i];
            *data++ = 0;
        }

        for (i = 0; i < rg->bcoutvol->num; i++)
        {
            *data++ = rg->bcout->list[4 * i + 0] + 1;
            *data++ = rg->bcout->list[4 * i + 1] + 1;
            *data++ = rg->bcout->list[4 * i + 2] + 1;
            *data++ = rg->bcout->list[4 * i + 3] + 1;
            *data++ = rg->bcoutvol->list[i] + 1;
            //*data++ = rg->bc_outval;
            *data++ = rg->bcout_type[i];
            *data++ = 0;
        }
        partObj[6] = balance;

        //  7. pressure bc: outlet elements
        sprintf(name, "%s_pressElems", basename);
        size[0] = 6;
        size[1] = 0;
        coDoIntArr *pressElems = new coDoIntArr(name, 2, size);
        data = pressElems->getAddress();

        //  8. pressure bc: value for outlet elements
        sprintf(name, "%s_pressVal", basename);
        coDoFloat *pressValues
            = new coDoFloat(name, 0);
        pressValues->getAddress(&bPtr);

        partObj[7] = pressElems;
        partObj[8] = NULL;

        coDoSet *set = new coDoSet((char *)basename, (coDistributedObject **)partObj);

        boco->setCurrentObject(set);

        float *xinp = new float[rg->bcin_nodes.size()];
        float *yinp = new float[rg->bcin_nodes.size()];
        float *zinp = new float[rg->bcin_nodes.size()];
        for (i = 0; i < rg->bcin_nodes.size(); i++)
        {
            xinp[i] = rg->p->x[rg->bcin_nodes[i]];
            yinp[i] = rg->p->y[rg->bcin_nodes[i]];
            zinp[i] = rg->p->z[rg->bcin_nodes[i]];
        }
        coDoPoints *in_points;
        in_points = new coDoPoints(inpoints->getObjName(), (int)rg->bcin_nodes.size(), xinp, yinp, zinp);
        inpoints->setCurrentObject(in_points);

        coDoFloat *in_types;
        in_types = new coDoFloat(intypes->getObjName(), (int)rg->bcin_type2.size(), &rg->bcin_type2[0]);
        intypes->setCurrentObject(in_types);

        /*
      float *xairp = new float[rg->bcair_nodes->num];
      float *yairp = new float[rg->bcair_nodes->num];
      float *zairp = new float[rg->bcair_nodes->num];
      for (i=0;i<rg->bcair_nodes->num;i++)
      {
         xairp[i]=rg->p->x[rg->bcair_nodes->list[i]];
         yairp[i]=rg->p->y[rg->bcair_nodes->list[i]];
         zairp[i]=rg->p->z[rg->bcair_nodes->list[i]];
      }

      coDoPoints *air_points;
      air_points = new coDoPoints(airpoints->getObjName(), rg->bcair_nodes->num, xairp, yairp, zairp);
      airpoints->setCurrentObject(air_points);

      float *xvenp = new float[rg->bcven_nodes->num];
      float *yvenp = new float[rg->bcven_nodes->num];
      float *zvenp = new float[rg->bcven_nodes->num];
      for (i=0;i<rg->bcven_nodes->num;i++)
      {
         xvenp[i]=rg->p->x[rg->bcven_nodes->list[i]];
         yvenp[i]=rg->p->y[rg->bcven_nodes->list[i]];
         zvenp[i]=rg->p->z[rg->bcven_nodes->list[i]];
      }
      coDoPoints *ven_points;
      ven_points = new coDoPoints(venpoints->getObjName(), rg->bcven_nodes->num, xvenp, yvenp, zvenp);
      venpoints->setCurrentObject(ven_points);
*/
        const char *geofile = p_geofile->getValue();
        const char *rbfile = p_rbfile->getValue();

        if (p_createGeoRbFile->getValue())
        {
            CreateGeoRbFile(rg, geofile, rbfile);
        }
    }

    ///////////////////////// Free everything ////////////////////////////////

    return SUCCESS;
}

void SurfaceDemo::CreateUserMenu(void)
{
    //   fprintf(stderr, "Entering CreateUserMenu()\n");

    char *tmp;
    int i;
    char path[512];

    p_makeGrid = addBooleanParam("make_grid", "make grid?");
    p_makeGrid->setValue(0);

    p_lockmakeGrid = addBooleanParam("lock_make_grid_button", "lock make grid button?");
    p_lockmakeGrid->setValue(0);

    p_createGeoRbFile = addBooleanParam("create_geo_or_rb_file", "create geo/rb file?");
    p_createGeoRbFile->setValue(0);

    p_gridSpacing = addFloatParam("spacing", "elements per floor square");
    p_gridSpacing->setValue(2.);

    p_model_size = addFloatVectorParam("model_size", "model size");
    p_model_size->setValue(16, 12, 3.20f);

    p_nobjects = addInt32Param("n_objects", "make grid?");
    p_nobjects->setValue(MAX_CUBES);

    p_Q_total = addFloatParam("Q_inlet_m3_h", "total flow rate in m3/h");
    p_Q_total->setValue(50000.);

    sprintf(path, "%s/racks.txt", coCoviseConfig::getEntry("value", "Module.SurfaceDemo.GeorbPath", "/data/SurfaceDemo").c_str());
    p_BCFile = addStringParam("BCFile", "BCFile");
    p_BCFile->setValue(path);

    sprintf(path, "%s/geofile.geo", coCoviseConfig::getEntry("value", "Module.SurfaceDemo.GeorbPath", "/data/SurfaceDemo").c_str());
    p_geofile = addStringParam("GeofilePath", "geofile path");
    p_geofile->setValue(path);

    sprintf(path, "%s/rbfile.geo", coCoviseConfig::getEntry("value", "Module.SurfaceDemo.GeorbPath", "/data/SurfaceDemo").c_str());
    p_rbfile = addStringParam("RbfilePath", "rbfile path");
    p_rbfile->setValue(path);

    m_Geometry = paraSwitch("Geometry", "Select Rack");
    geo_labels = (char **)calloc(MAX_CUBES, sizeof(char *));

    for (i = 0; i < MAX_CUBES; i++)
    {
        // create description and name
        geo_labels[i] = IndexedParameterName(GEO_SEC, i);
        paraCase(geo_labels[i]); // Geometry section

        tmp = IndexedParameterName("pos_rack", i);
        p_cubes_pos[i] = addFloatVectorParam(tmp, tmp);
        p_cubes_pos[i]->setValue(0.0, 0.0, 0.0);

        tmp = IndexedParameterName("size_rack", i);
        p_cubes_size[i] = addFloatVectorParam(tmp, tmp);
        p_cubes_size[i]->setValue(0.0, 0.0, 0.0);
        free(tmp);

        paraEndCase(); // Geometry section
    }
    paraEndSwitch(); // "GeCross section", "Select GeCross section"

    m_FlowRate = paraSwitch("FlowRate", "Select Rack");
    flow_labels = (char **)calloc(MAX_CUBES - 1, sizeof(char *)); // racks without escalator and cold house

    //ici
    float flowrate_racks[MAX_CUBES] = {
        5., //  0 Infrastruktur
        5., //  1 Phoenix
        3., //  2 Strider
        5., //  3 SX8R
        5., //  4 Disk_Rack_1 (5 Stueck)
        5., //  5 Disk_Rack_2 (5 Stueck)
        5., //  6 Disk_Rack_3 (5 Stueck)
        5., //  7 Netz1
        5., //  8 Disk_Rack_6 (5 Stueck)
        5., //  9 IOX
        5., // 10 Netz2
        5., // 11 FC_Netz
    };

    // Infrastruktur
    // Phoenix
    // Strider
    // SX 8R
    // Disk_Rack_1
    // Disk_Rack_2
    // Disk_Rack_3
    // Netz1
    // Disk_Rack_6
    // IOX
    // Netz2
    // FC_Netz

    for (i = 0; i < MAX_CUBES; i++)
    {
        // create description and name
        flow_labels[i] = IndexedParameterName(FLOW_SEC, i);
        paraCase(flow_labels[i]);

        tmp = IndexedParameterName("flowrate_rack", i);
        p_flowrate[i] = addFloatParam(tmp, tmp);
        p_flowrate[i]->setValue(flowrate_racks[i]);

        paraEndCase(); // FlowRate section
    }

    paraEndSwitch(); // Flowrate section

#ifndef USE_STARTFILE
    setGeoParamsStandardForSurfaceDemo();
#endif
}

char *SurfaceDemo::IndexedParameterName(const char *name, int index)
{
    char buf[255];
    sprintf(buf, "%s_%d", name, index + 1);
    return strdup(buf);
}

void SurfaceDemo::createFeedbackObjects()
{

    cerr << "createFeedbackObjects()" << endl;

    coDoPoints *feedinfo;
    feedinfo = new coDoPoints(feedback_info->getObjName(), 0);
    ////////////////////////////////////////////////////////////////////
    // add the current parameter values as feedback parameters
    int i = 0;

    coFeedback feedback("TangiblePosition");
    feedback.addString("SurfaceDemo");

    // the others only if used
    for (i = 0; i < p_nobjects->getValue(); i++)
    {
        feedback.addPara(p_cubes_pos[i]);
        feedback.addPara(p_cubes_size[i]);
    }
    feedback.apply(feedinfo);
    feedback_info->setCurrentObject(feedinfo);
}

int SurfaceDemo::GetParamsFromControlPanel(struct rech_model *model)
{
    int i;

    //   fprintf (stderr, "entering GetParamsFromControlPanel\n");

    model->nobjects = p_nobjects->getValue();

    // alloc cubes
    if ((model->cubes = (struct cubus **)calloc(model->nobjects, sizeof(struct cubus *))) == NULL)
    {
        fprintf(stderr, "Not enough space!\n");
        return -1;
    }
    for (i = 0; i < model->nobjects; i++)
    {
        if ((model->cubes[i] = (struct cubus *)calloc(1, sizeof(struct cubus))) == NULL)
        {
            fprintf(stderr, "Not enough space!\n");
            return -1;
        }
    }

    // total size
    model->size[0] = p_model_size->getValue(0);
    model->size[1] = p_model_size->getValue(1);
    model->size[2] = p_model_size->getValue(2);

    for (i = 0; i < model->nobjects; i++)
    {
        model->cubes[i]->size[0] = p_cubes_size[i]->getValue(0);
        model->cubes[i]->size[1] = p_cubes_size[i]->getValue(1);
        model->cubes[i]->size[2] = p_cubes_size[i]->getValue(2);

        model->cubes[i]->pos[0] = p_cubes_pos[i]->getValue(0);
        model->cubes[i]->pos[1] = p_cubes_pos[i]->getValue(1);
        model->cubes[i]->pos[2] = p_cubes_pos[i]->getValue(2);
    }

    for (i = 0; i < model->nobjects - 4; i++) // not for escalator and cold house
    {
        model->cubes[i]->Q = p_flowrate[i]->getValue();
    }

    model->Q_total = p_Q_total->getValue();

    return (0);
}

int SurfaceDemo::setGeoParamsStandardForSurfaceDemo()
{
    int i;

    float rech_cubes_pos[MAX_CUBES][3];
    float rech_cubes_size[MAX_CUBES][3];

    //   fprintf(stderr,"entering setGeoParamsStandardForSurfaceDemo ...\n");

    i = 0;

    // Infrastruktur
    rech_cubes_pos[i][0] = 1.60f;
    rech_cubes_size[i][0] = 3.86f; //3.61;
    rech_cubes_pos[i][1] = 1.97f;
    rech_cubes_size[i][1] = 0.88f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.20f;
    i++;

    // Phoenix
    rech_cubes_pos[i][0] = 6.21f;
    rech_cubes_size[i][0] = 2.50f;
    rech_cubes_pos[i][1] = 1.97f;
    rech_cubes_size[i][1] = 0.88f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.20f;
    i++;

    /*
   // NAS
   rech_cubes_pos[ i][0]=8.11;  rech_cubes_size[ i][0]=0.89;
   rech_cubes_pos[ i][1]=1.97;  rech_cubes_size[ i][1]=0.88;
   rech_cubes_pos[ i][2]=0.00;  rech_cubes_size[ i][2]=2.20;

   // DDN
   rech_cubes_pos[ i][0]=9.77;  rech_cubes_size[ i][0]=0.67;
   rech_cubes_pos[ i][1]=1.97;  rech_cubes_size[ i][1]=0.88;
   rech_cubes_pos[ i][2]=0.00;  rech_cubes_size[ i][2]=2.00;
*/

    // Strider
    rech_cubes_pos[i][0] = 9.94f;
    rech_cubes_size[i][0] = 2.94f;
    rech_cubes_pos[i][1] = 1.97f;
    rech_cubes_size[i][1] = 0.88f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.00f;
    i++;

    // SX 8R
    rech_cubes_pos[i][0] = 13.38f;
    rech_cubes_size[i][0] = 1.08f;
    rech_cubes_pos[i][1] = 1.97f;
    rech_cubes_size[i][1] = 0.88f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.00f;
    i++;

    // Disk_Rack_1
    rech_cubes_pos[i][0] = 2.40f;
    rech_cubes_size[i][0] = 3.03f;
    rech_cubes_pos[i][1] = 5.39f;
    rech_cubes_size[i][1] = 0.78f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 1.60f;
    i++;

    // Disk_Rack_2
    rech_cubes_pos[i][0] = 6.00f;
    rech_cubes_size[i][0] = 3.03f;
    rech_cubes_pos[i][1] = 5.39f;
    rech_cubes_size[i][1] = 0.78f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 1.60f;
    i++;

    // Disk_Rack_3
    rech_cubes_pos[i][0] = 9.60f;
    rech_cubes_size[i][0] = 3.03f;
    rech_cubes_pos[i][1] = 5.39f;
    rech_cubes_size[i][1] = 0.78f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 1.60f;
    i++;

    // Netz1
    rech_cubes_pos[i][0] = 2.69f;
    rech_cubes_size[i][0] = 0.71f;
    rech_cubes_pos[i][1] = 7.02f;
    rech_cubes_size[i][1] = 2.15f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.30f;
    i++;

    // Disk_Rack_6
    rech_cubes_pos[i][0] = 4.22f;
    rech_cubes_size[i][0] = 3.03f;
    rech_cubes_pos[i][1] = 7.18f;
    rech_cubes_size[i][1] = 0.78f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 1.60f;
    i++;

    // IOX
    rech_cubes_pos[i][0] = 7.78f;
    rech_cubes_size[i][0] = 1.22f;
    rech_cubes_pos[i][1] = 7.45f;
    rech_cubes_size[i][1] = 0.93f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.00f;
    i++;

    // Netz2
    rech_cubes_pos[i][0] = 9.46f;
    rech_cubes_size[i][0] = 1.47f;
    rech_cubes_pos[i][1] = 7.55f;
    rech_cubes_size[i][1] = 0.85f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 2.30f;
    i++;

    // FC_Netz
    rech_cubes_pos[i][0] = 11.40f;
    rech_cubes_size[i][0] = 1.82f;
    rech_cubes_pos[i][1] = 7.51f;
    rech_cubes_size[i][1] = 0.90f;
    rech_cubes_pos[i][2] = 0.00f;
    rech_cubes_size[i][2] = 1.80f;
    i++;

    for (i = 0; i < MAX_CUBES; i++)
    {
        rech_cubes_pos[i][2] += 0.2f;
        p_cubes_pos[i]->setValue(rech_cubes_pos[i][0], rech_cubes_pos[i][1], rech_cubes_pos[i][2]);
        p_cubes_size[i]->setValue(rech_cubes_size[i][0], rech_cubes_size[i][1], rech_cubes_size[i][2]);
    }

    return 0;
}

MODULE_MAIN(UnderDev, SurfaceDemo)
