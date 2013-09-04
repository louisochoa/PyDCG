#include <Python.h>
#include "gbCPP.h"

using std::vector;

static const long long max_val = (1 << 30);

static const char* count_convex_rholes_doc =
"count_convex_rholes(points, r, mono = True)\n\
    \n\
    Counts the r-holes in a point set.\n\
    \n\
    This function counts how many convex `r`-holes are in `points` (the \n\
    point set may be colored). It is an implementation of the algorithm \n\
    presented in \"Searching for Empty Convex Polygons\" [1]_\n\
    \n\
    Parameters\n\
    ----------\n\
    points : list\n\
        This list represents the point set, each point is represented as a\n\
        list of 3 integers: the first two are the coordinates of the point,\n\
        the third one is the color. The color is optional.\n\
    r : int\n\
        The number of sides of the holes we want to fint in the point set.\n\
    mono : boolean\n\
        Determines wheter to look for monochromatic `r`-holes or not.\n\
    \n\
    Returns\n\
    -------\n\
    h : int\n\
        The number of `r`-holes in `points`.\n\
    \n\
    Notes\n\
    -----\n\
    The coordinates of the points should be less than or equal to :math:`2^{30}` to\n\
    prevent overflow on the C++ side.\n\
    \n\
    Examples\n\
    --------\n\
    The first call counts the empty quadrilaterals, disregarding the color\n\
    of the points. The second call counts only monochromatic quadrilaterals.\n\
    \n\
    >>> import holesCpp as holes\n\
    >>> points=[[0,2,0], [1,0,1], [2,4,0], [4,1,0], [4,3,0]]\n\
    >>> holes.count_convex_rholes(points, 4)\n\
    5\n\
    >>> holes.count_convex_rholes(points, 4, True)\n\
    1\n";

extern "C" PyObject* count_convex_rholes(PyObject* self, PyObject* args)
{
    //The C++ function prototype is: int count_convex_rholes(const std::vector<punto>&, int, bool=false);
    PyObject* py_pts;
    PyObject* py_mono = NULL;

    int r;
    int mono = 0;

    //The arguments must be: a list with the points (each point is a list of two integers),
    //an integer (r) and a boolean (mono). The boolean is optional.
    if (!PyArg_ParseTuple(args, "O!i|O!:count_convex_rholes", &PyList_Type, &py_pts, &r, &PyBool_Type, &py_mono))
        return NULL;


    if(py_mono != NULL && py_mono == Py_True)
        mono = 1;

    Py_ssize_t points_size = PyList_Size(py_pts);

    vector<punto> pts;
    //Py_ssize_t can be bigger than an 2^32, but we don't expect
    //to work with that many points.
    pts.reserve(int(points_size));

    for(Py_ssize_t i=0; i < points_size; i++)
    {
        PyObject *punto = PyList_GetItem(py_pts, i); //Borrowed Reference
        Py_ssize_t n_coords = PyList_Size(punto);
        long long x, y;
        int color = 0;

        if(n_coords == 3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2)); //Borrowed Reference
        else if(n_coords > 3 || n_coords < 2)
        {
            PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
            return NULL;
        }

        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //Borrowed References
        y = PyInt_AsLong(PyList_GetItem(punto, 1));

        if(x > max_val || y > max_val || x < -max_val || y < -max_val)
        {
            PyErr_SetString(PyExc_ValueError, "The coordinates of each point must less than or equal to 2^30 in absolute value.");
            return NULL;
        }

        pts.emplace_back(x, y, color);
    }
    return Py_BuildValue("i", count_convex_rholes(pts, r, mono));
}

static const char* count_convex_rholes_p_doc =
"count_convex_rholes_p(p, points, r, mono = True)\n\
    \n\
    Counts the r-holes in a point set certain point as vertex and the.\n\
    r-gons with only that point inside them.\n\
    \n\
    This function counts how many convex `r`-holes in `points` have `p`\n\
    as a vertex and how many convex `r`-gons in `points` have only `p` \n\
    inside them. The point set may be colored. The point `p` must not\n\
    be contained inside `points`.\n\
    \n\
    Parameters\n\
    ----------\n\
    p : list\n\
        A list with 3 integers, representing a point in the plane. The\n\
        first two are the coordinates of the point, the third one is the\n\
        color. The color is optional\n\
    points : list\n\
        This list represents the point set, each point is represented in the\n\
        same way as p.\n\
    r : int\n\
        The number of sides of the holes we want to fint in the point set.\n\
    mono : boolean\n\
        Determines wheter to look for monochromatic `r`-holes or not\n\
    \n\
    Returns\n\
    -------\n\
    (a, b) : tuple\n\
        `a` is the number of convex `r`-holes in `points` that have `p` as\n\
        a vertex and `b` the number of convex `r`-gons in `points` have only\n\
        `p` inside them.\n\
    \n\
    Notes\n\
    -----\n\
    The coordinates of the points should be less than or equal to 2^30 to\n\
    prevent overflow on the C++ side. The point `p` must not be included\n\
    in points.";

extern "C" PyObject* count_convex_rholes_p(PyObject* self, PyObject* args)
{
    //The C++ function prototype is:
    //void count_convex_rholes_p(punto, const std::vector<punto>&, int, int&, int&, bool=false);
    PyObject* py_pts;
    PyObject* py_p;
    PyObject* py_mono = NULL;

    int r;
    int mono = 0;
    punto p;

    //The arguments must be: a point (each point is a list of two or three integers),
    //a list with the points, an integer (r) and a boolean (mono). The boolean is optional.
    if (!PyArg_ParseTuple(args, "O!O!i|O!:count_convex_rholes", &PyList_Type, &py_p, &PyList_Type, &py_pts, &r, &PyBool_Type, &py_mono))
        return NULL;

    if(py_mono != NULL && py_mono == Py_True)
        mono = 1;

    Py_ssize_t points_size = PyList_Size(py_pts);

    vector<punto> pts;

    //Py_ssize_t can be bigger than an 2^32, but we don't expect
    //to work with that many points.
    pts.reserve(int(points_size));

    for(Py_ssize_t i=0; i < points_size; i++)
    {
        PyObject *punto = PyList_GetItem(py_pts, i); //Borrowed Reference
        Py_ssize_t n_coords = PyList_Size(punto);
        long long x, y;
        int color = 0;

        if(n_coords == 3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2)); //Borrowed Reference
        else if(n_coords > 3 || n_coords < 2)
        {
            PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
            return NULL;
        }

        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //Borrowed References
        y = PyInt_AsLong(PyList_GetItem(punto, 1)); //Borrowed References

        if(x > max_val || y > max_val || x < -max_val || y < -max_val)
        {
            PyErr_SetString(PyExc_ValueError, "The coordinates of each point must less than or equal to 2^30 in absolute value.");
            return NULL;
        }

        pts.emplace_back(x, y, color);
    }

    //Constructing the point p

    Py_ssize_t size_pt = PyList_Size(py_p);
    if(size_pt > 3 || size_pt < 2)
    {
        PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
        return NULL;
    }
    if(size_pt == 3)
        p.color = (int)PyInt_AsLong(PyList_GetItem(py_p, 2)); //Borrowed References

    p.x = PyInt_AsLong(PyList_GetItem(py_p, 0)); //Borrowed References
    p.y = PyInt_AsLong(PyList_GetItem(py_p, 1)); //Borrowed References

    int A, B;
    count_convex_rholes_p(p, pts, r, A, B, mono);

    return Py_BuildValue("ii", A, B);
}

static const char* report_convex_rholes_doc =
"report_convex_rholes(points, r, mono = True)\n\
    \n\
    Reports the r-holes in a point set.\n\
    \n\
    This function reports how many convex `r`-holes are in `points` (the \n\
    point set may be colored). It is an implementation of the algorithm \n\
    presented in \"Searching for Empty Convex Polygons\"\n\
    \n\
    Parameters\n\
    ----------\n\
    points : list\n\
        This list represents the point set, each point is represented as a\n\
        list of 3 integers: the first two are the coordinates of the point,\n\
        the third one is the color. The color is optional.\n\
    r : int\n\
        The number of sides of the holes we want to fint in the point set.\n\
    mono : boolean\n\
        Determines wheter to look for monochromatic `r`-holes or not\n\
    \n\
    Returns\n\
    -------\n\
    H : int\n\
        A list containing the `r`-holes in `points`. Each `r`-hole is repre-\n\
        sented as a list of points, in counterclockwise order. Each point is\n\
        stored in the same way explained for the parameter `points`.\n\
    \n\
    Notes\n\
    -----\n\
    The coordinates of the points should be less than or equal to 2^30 to\n\
    prevent overflow on the C++ side.";

extern "C" PyObject* report_convex_rholes(PyObject* self, PyObject* args)
{
    //The C++ function prototype is:
    //std::deque<std::vector<punto> > report_convex_rholes(const std::vector<punto>&, int, bool=false);
    PyObject* py_pts;
    PyObject* py_mono = NULL;

    int r;
    int mono = 0;

    //The arguments must be: a list with the points (each point is a list of two integers),
    //an integer (r) and a boolean (mono). The boolean is optional.
    if (!PyArg_ParseTuple(args, "O!i|O!:count_convex_rholes", &PyList_Type, &py_pts, &r, &PyBool_Type, &py_mono))
        return NULL;


    if(py_mono != NULL && py_mono == Py_True)
        mono = 1;

    Py_ssize_t points_size = PyList_Size(py_pts);

    vector<punto> pts;
    //Py_ssize_t can be bigger than an 2^32, but we don't expect
    //to work with that many points.
    pts.reserve(int(points_size));

    for(Py_ssize_t i=0; i < points_size; i++)
    {
        PyObject *punto = PyList_GetItem(py_pts, i); //Borrowed Reference
        Py_ssize_t n_coords = PyList_Size(punto);
        long long x, y;
        int color = 0;

        if(n_coords == 3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2)); //Borrowed Reference
        else if(n_coords > 3 || n_coords < 2)
        {
            PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
            return NULL;
        }

        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //Borrowed References
        y = PyInt_AsLong(PyList_GetItem(punto, 1));

        if(x > max_val || y > max_val || x < -max_val || y < -max_val)
        {
            PyErr_SetString(PyExc_ValueError, "The coordinates of each point must less than or equal to 2^30 in absolute value.");
            return NULL;
        }

        pts.emplace_back(x, y, color);
    }
    auto res = report_convex_rholes(pts, r, mono);

    PyObject* py_res = PyList_New(0);

    for (auto poli : res)
    {
        PyObject* py_poli = PyList_New(0);
        for(auto point : poli)
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

            coord = PyInt_FromLong(point.color);
            if(PyList_Append(py_point, coord) == -1) //Append increases reference count
                return NULL;
            Py_DECREF(coord);

            if(PyList_Append(py_poli, py_point) == -1)
                return NULL;
            Py_DECREF(py_point);
        }
        if(PyList_Append(py_res, py_poli) == -1)
            return NULL;
        Py_DECREF(py_poli);
    }

    return py_res;
}

extern "C" PyObject* countEmptyTriangs(PyObject* self, PyObject* args)
{
    PyObject* py_pts;

    if (!PyArg_ParseTuple(args, "O!:countEmptyTriangs", &PyList_Type, &py_pts))
        return NULL;

    Py_ssize_t N_points = PyList_Size(py_pts);

    vector<punto> pts;

    for(int i=0; i<N_points; i++)
    {
        PyObject *punto = PyList_GetItem(py_pts, i); //BORROWED REFERENCE!
        Py_ssize_t n_coords = PyList_Size(punto);
        long x, y;
        int color=0;

        if(n_coords==3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2));
        else if(n_coords>3)
        {
            printf("Demasiados valores en la lista.");
            return NULL;
        }
        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //BORROWED REFERENCES!
        y = PyInt_AsLong(PyList_GetItem(punto, 1));

        //Se debería checar si no hubo exepción
        pts.emplace_back(x, y, color);
    }
    return Py_BuildValue("i", countEmptyTriangs(pts));
}

extern "C" PyObject* count_empty_triangles_p(PyObject* self, PyObject* args)
{
    PyObject* py_pts;
    PyObject* py_p;

    vector<punto> pts;
    punto p;

    if (!PyArg_ParseTuple(args, "O!O!:count_convex_rholes", &PyList_Type, &py_p, &PyList_Type, &py_pts))
        return NULL;

    int N_points = (int)PyList_Size(py_pts);

    //Pasar la lista de puntos a vector<punto>
    for(int i=0; i<N_points; i++)
    {
        PyObject *punto = PyList_GetItem(py_pts, i); //BORROWED REFERENCE!
        Py_ssize_t n_coords = PyList_Size(punto);
        long x, y;
        int color=0;

        if(n_coords==3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2));
        else if(n_coords>3)
        {
            printf("Demasiados valores en la lista.");
            return NULL;
        }
        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //BORROWED REFERENCES!
        y = PyInt_AsLong(PyList_GetItem(punto, 1));

        //Se debería checar si no hubo exepción
        pts.emplace_back(x, y, color);
    }

    //Pasar py_p a punto
    Py_ssize_t N_pt = PyList_Size(py_p);
    if(N_pt==3)
        p.color = (int)PyInt_AsLong(PyList_GetItem(py_p, 2));
    else if(N_pt>3)
    {
        printf("Demasiados valores en p.");
        return NULL;
    }
    p.x = PyInt_AsLong(PyList_GetItem(py_p, 0)); //BORROWED REFERENCES!
    p.y = PyInt_AsLong(PyList_GetItem(py_p, 1));

    int A, B;
    count_empty_triangles_p(p, pts, A, B);

    return Py_BuildValue("ii", A, B);
}

extern "C" PyObject* general_position(PyObject* self, PyObject* args)
{
    //The C++ function prototype is: general_position(vector<punto>& points)
    PyObject* py_points;

    vector<punto> points;

    //The arguments must be: a list with the point (a list of two integers),
    //and a list of 3 points representing the triangle
    if (!PyArg_ParseTuple(args, "O!:general_position", &PyList_Type, &py_points))
        return NULL;

    Py_ssize_t points_size = PyList_Size(py_points);

    //Py_ssize_t can be bigger than an 2^32, but we don't expect
    //to work with that many points.
    points.reserve(int(points_size));

    for(Py_ssize_t i=0; i < points_size; i++)
    {
        PyObject *punto = PyList_GetItem(py_points, i); //Borrowed Reference
        Py_ssize_t n_coords = PyList_Size(punto);
        long long x, y;
        int color = 0;

        if(n_coords == 3)
            color = (int)PyInt_AsLong(PyList_GetItem(punto, 2)); //Borrowed Reference
        else if(n_coords > 3 || n_coords < 2)
        {
            PyErr_SetString(PyExc_ValueError, "Wrong number of values representing a point, must be 2 or 3.");
            return NULL;
        }

        x = PyInt_AsLong(PyList_GetItem(punto, 0)); //Borrowed References
        y = PyInt_AsLong(PyList_GetItem(punto, 1));

        if(x > max_val || y > max_val || x < -max_val || y < -max_val)
        {
            PyErr_SetString(PyExc_ValueError, "The coordinates of each point must less than or equal to 2^30 in absolute value.");
            return NULL;
        }

        points.emplace_back(x, y, color);
    }
    return Py_BuildValue("i", general_position(points));
}

extern "C" PyMethodDef holesCppMethods[] =
{
    {"count_convex_rholes", count_convex_rholes, METH_VARARGS, count_convex_rholes_doc},
    {"count_convex_rholes_p", count_convex_rholes_p, METH_VARARGS, count_convex_rholes_p_doc},
    {"report_convex_rholes", report_convex_rholes, METH_VARARGS, report_convex_rholes_doc},
    {"countEmptyTriangs", countEmptyTriangs, METH_VARARGS,
        "Counts the number of empty triangles in points."},
    {"count_empty_triangles_p", count_empty_triangles_p, METH_VARARGS,
        "Returns (A_p, B_p), the number of empty triangles with p as a vertex and the number of triangles with only p inside. points must not contain p."},
    {"general_position", general_position, METH_VARARGS, "Verify if a point set is in general position"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initholesCpp(void)
{
    (void) Py_InitModule3("holesCpp", holesCppMethods,
                          "Extension written in C++ intended to help finding sets with few r-holes.");
}
