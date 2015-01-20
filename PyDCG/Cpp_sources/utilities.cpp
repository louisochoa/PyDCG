#include <Python.h>
#include <vector>
#include "geometricbasicsCpp.h"

#ifdef INT32
static const long long max_val = (1L << 30);
const char max_val_error[] = "The coordinates of each point must less than or equal to 2^30 in absolute value.";
#else
static const long long max_val = (1L << 62);
const char max_val_error[] = "The coordinates of each point must less than or equal to 2^62 in absolute value.";
#endif

using std::vector;

const int FAIL = 0;
const int SUCCESS = 1;

//TODO: I should write another function that transforms C++ vectors of polygons, there's too much repeated code in the wrappers

/**Converts py_p, python object representing a point (a list of two numbers) to p, a C++ point object.
   The size of the python list must be 2 or 3.*/
int pyPoint_CPoint(PyObject* py_p, Punto& p)
{
    Py_ssize_t size_pt = PyList_Size(py_p);
    if(size_pt > 3 || size_pt < 2)
    {
        PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
        return FAIL;
    }

    p.x = PyInt_AsLong(PyList_GetItem(py_p, 0)); //Borrowed References
    p.y = PyInt_AsLong(PyList_GetItem(py_p, 1)); //Borrowed References
    if(size_pt == 3)
    {
        p.color = (int)PyInt_AsLong(PyList_GetItem(py_p, 2)); //Borrowed References
        p._has_color = true;
    }

    if(PyErr_Occurred() != NULL)
        return FAIL;

    if(abs(p.x) > max_val || abs(p.y) > max_val)
    {
        PyErr_SetString(PyExc_OverflowError, max_val_error);
        return FAIL;
    }
    return SUCCESS;
}

/**Recieves a C++ point object and returns a python object representing a point (a list of two numbers).*/
PyObject* CPoint_PyPoint(Punto point)
{
    PyObject* py_point = PyList_New(0);

    PyObject* coord = PyInt_FromLong(point.x);
    if(PyList_Append(py_point, coord) == -1) //Append increases reference count
        return NULL;
    Py_DECREF(coord);

    coord = PyInt_FromLong(point.y);
    if(PyList_Append(py_point, coord) == -1) //Append increases reference count
        return NULL;
    Py_DECREF(coord);

    if(point._has_color)
    {
        coord = PyInt_FromLong(point.color);
        if(PyList_Append(py_point, coord) == -1) //Append increases reference count
            return NULL;
        Py_DECREF(coord);
    }

    return py_point;
}

/**Converts py_pts, python object representing a point set (a list of lists of two numbers) to pts, a C++ vector of points.
   The size of the python list must be 2 or 3.*/
int pyPointset_CPointset(PyObject* py_pts, vector<Punto>& pts)
{
    Py_ssize_t points_size = PyList_Size(py_pts);
    pts.reserve(int(points_size));

    for(Py_ssize_t i=0; i < points_size; i++)
    {
        PyObject *point = PyList_GetItem(py_pts, i); //Borrowed Reference
        Punto p;

        if(pyPoint_CPoint(point, p) != SUCCESS) //TODO: Check if itś worth it to unpack x, y nd color to use emplace_back instead of push_back
            return FAIL;

        pts.push_back(p);
    }
    return SUCCESS;
}


/**Recieves a C++ vector of points and returns a python object representing a point set (a list of lists of two numbers).*/
PyObject* CPointset_PyPointset(vector<Punto>& pts)
{
    PyObject* py_pts = PyList_New(0);
    for(auto point : pts)
    {
        PyObject* py_point = CPoint_PyPoint(point);

        if(py_point == NULL)
            return NULL;

        if(PyList_Append(py_pts, py_point) == -1) //Append increases reference count
            return NULL;
        Py_DECREF(py_point);
    }
    return py_pts;
}
