/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/****************************************************************************\ 
 **                                                           (C)2008 HLRS **
 **                                                                        **
 ** Description: Cutting-surface generation using CUDA on nVidia GPUs      **
 **                                                                        **
 **                                                                        **
 ** Name:        cuCuttingSurfaceUSG                                       **
 ** Category:    Tools                                                     **
 **                                                                        **
 **                                                                        **
\****************************************************************************/
#include <GL/glew.h>
#include <GL/glut.h>

#include <cutil.h>
#include <util/coVector.h>

#include <do/coDoData.h>
#include <do/coDoUnstructuredGrid.h>
#include <do/coDoSet.h>

#include "../cuIsoSurfaceUSG/cudaEngine.h"
#include "cuCuttingSurfaceUSG.h"

CUDAEngine cudaEngine;

using namespace covise;

/*! \brief constructor
 *
 * create In/Output Ports and module parameters
 */
cuCuttingSurfaceUSG::cuCuttingSurfaceUSG(int argc, char **argv)
    : coModule(argc, argv, "Generate a CuttingSurface from an USG")
{
    p_mesh = addInputPort("mesh", "UnstructuredGrid", "mesh");
    p_polygons = addOutputPort("polygons", "Polygons", "polygons");
    p_dataIn = addInputPort("dataIn", "Float", "Data for cuttingsurface generation");
    //p_dataIn->setRequired(0);

    p_data = addOutputPort("data", "Float", "Interpolated data");

    p_vertex = addFloatVectorParam("vertex", "Normal of cuttingplane");
    p_vertex->setValue(1.0, 0.0, 0.0);

    p_scalar = addFloatParam("scalar", "Distance from the origin to the cuttingplane");
    p_scalar->setValue(0.0);

    state = NULL;
    cudaEngine.Init();
}

cuCuttingSurfaceUSG::~cuCuttingSurfaceUSG()
{
}

/*! \brief param callback
 *
 * called when a parameter in a module is changed.
 */
void cuCuttingSurfaceUSG::param(const char * /* name */, bool /* inMapLoading */)
{
}

void computeMesh(State *state, float nx, float ny, float nz, float dist,
                 int *numVertices, float **vertex, float **normal)
{
    cudaEngine.computeCuttingMesh(state, nx, ny, nz, dist, numVertices,
                                  vertex, normal);
}

struct polyData cuCuttingSurfaceUSG::createPolygons(coOutputPort *polyPort,
                                                    coOutputPort *dataPort,
                                                    coDistributedObject *obj,
                                                    coDistributedObject *data_obj,
                                                    float nx, float ny, float nz,
                                                    float dist, int level)
{
    coDoUnstructuredGrid *grid = NULL;
    coDoSet *set = NULL;
    coDistributedObject *poly = NULL;
    coDistributedObject *data = NULL;

    if ((set = dynamic_cast<coDoSet *>(obj)))
    {

        int numChildren = set->getNumElements();
        coDistributedObject **polyList = new coDistributedObject *[numChildren + 1];
        coDistributedObject **dataList = new coDistributedObject *[numChildren + 1];

        coDoSet *dataSet = dynamic_cast<coDoSet *>(data_obj);

        for (int index = 0; index < numChildren; index++)
        {
            coDistributedObject *d = data_obj;
            if (dataSet)
                d = dataSet->getElement(index);

            struct polyData polyData = createPolygons(polyPort, dataPort, set->getElement(index), d, nx, ny, nz, dist, level++);
            polyList[index] = polyData.polygons;
            ;
            dataList[index] = polyData.data;
        }

        polyList[numChildren] = NULL;
        dataList[numChildren] = NULL;

        char polyName[128];
        char dataName[128];
        snprintf(polyName, 128, "%s_%d", polyPort->getObjName(), level);
        snprintf(dataName, 128, "%s_%d", dataPort->getObjName(), level);

        poly = new coDoSet(polyName, set->getNumElements(), polyList);
        data = new coDoSet(dataName, set->getNumElements(), dataList);
    }
    else if ((grid = dynamic_cast<coDoUnstructuredGrid *>(obj)))
    {

        int numElem, numConn, numCoord;
        int *elemList = NULL, *connList = NULL, *typeList = NULL;
        float *x = NULL, *y = NULL, *z = NULL;

        grid->getGridSize(&numElem, &numConn, &numCoord);
        grid->getAddresses(&elemList, &connList, &x, &y, &z);
        grid->getTypeList(&typeList);

        coDoFloat *data_in;
        if ((data_in = (dynamic_cast<coDoFloat *>(data_obj))))
        {
            float *vertex = NULL, *normal = NULL;
            int numVertices;
            float *values = data_in->getAddress();

            std::string name(grid->getName());
            if (name != objectName)
            {
                delete state;
                state = InitState(typeList, elemList, connList, x, y, z,
                                  numElem, numConn, numCoord, values, NULL, NULL, NULL);
                objectName = name;
                printf("numElem: %d\n", numElem);
            }

            computeMesh(state, nx, ny, nz, dist,
                        &numVertices, &vertex, &normal);

            int *cl = new int[numVertices];
            int *pl = new int[numVertices / 3];
            for (int index = 0; index < numVertices; index++)
            {
                if (!(index % 3))
                    pl[index / 3] = index;
                cl[index] = index;
            }

            poly = new coDoPolygons(polyPort->getObjName(), numVertices,
                                    vertex, vertex + numVertices, vertex + numVertices * 2,
                                    numVertices, cl, numVertices / 3, pl);

            delete[] pl;
            delete[] cl;
            free(vertex);
            free(normal);
        }
    }

    struct polyData polyData;
    polyData.polygons = poly;
    polyData.data = data;

    return polyData;
}

int cuCuttingSurfaceUSG::compute(const char * /* port */)
{
    coDistributedObject *grid = p_mesh->getCurrentObject();
    coDistributedObject *data_obj = p_dataIn->getCurrentObject();

    if (grid != NULL && data_obj != NULL && data_obj->objectOk())
    {
        float vertex[3];
        float length;
        p_vertex->getValue(vertex[0], vertex[1], vertex[2]);
        length = sqrt(vertex[0] * vertex[0] + vertex[1] * vertex[1] + vertex[2] * vertex[2]);
        for (int index = 0; index < 3; index++)
            vertex[index] /= length;

        struct polyData polyData = createPolygons(p_polygons, p_data, grid, data_obj, vertex[0], vertex[1], vertex[2], p_scalar->getValue());

        p_polygons->setCurrentObject(polyData.polygons);
        p_data->setCurrentObject(polyData.data);
    }
    else
    {
        sendError("ERROR: Data object 'dataIn' or 'grid' can't be accessed in shared memory");
        return STOP_PIPELINE;
    }

    return CONTINUE_PIPELINE;
}

MODULE_MAIN(IO, cuCuttingSurfaceUSG)
