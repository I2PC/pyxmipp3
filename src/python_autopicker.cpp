/***************************************************************************
 *
 * Authors:     J.M. De la Rosa Trevin (jmdelarosa@cnb.csic.es)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/

#include "python_autopicker.h"

#include <core/metadata_row_base.h>
#include <core/metadata_row_sql.h>
#include <core/metadata_db.h>

#include <vector>

/***************************************************************/
/*                            Autopicker                         */
/**************************************************************/

/* Autopicker methods */
PyMethodDef Autopicker_methods[] =
    {
        { "train", (PyCFunction) Autopicker_train, METH_VARARGS,
          "Train the model" },
        { "autopick", (PyCFunction) Autopicker_autopick, METH_VARARGS,
          "Autopick using the model" },
        { "correct", (PyCFunction) Autopicker_correct, METH_VARARGS,
          "Correct the model" },
        { "setSize", (PyCFunction) Autopicker_setSize, METH_VARARGS,
          "Set box size" },
        { "getParticlesThreshold", (PyCFunction) Autopicker_getParticlesThreshold, METH_NOARGS,
          "Get the threshold value" },

        { nullptr } /* Sentinel */
    };//Autopicker_methods


/*Autopicker Type */
PyTypeObject AutopickerType = {
                             PyObject_HEAD_INIT(0)
                             "xmipp.Autopicker", /*tp_name*/
                             sizeof(AutopickerObject), /*tp_basicsize*/
                             0, /*tp_itemsize*/
                             (destructor)Autopicker_dealloc, /*tp_dealloc*/
                             0, /*tp_print*/
                             0, /*tp_getattr*/
                             0, /*tp_setattr*/
                             0, /*tp_compare*/
                             nullptr, /*tp_repr*/
                             nullptr, /*tp_as_number*/
                             0, /*tp_as_sequence*/
                             0, /*tp_as_mapping*/
                             0, /*tp_hash */
                             0, /*tp_call*/
                             0, /*tp_str*/
                             0, /*tp_getattro*/
                             0, /*tp_setattro*/
                             0, /*tp_as_buffer*/
                             Py_TPFLAGS_DEFAULT, /*tp_flags*/
                             "Python wrapper to Xmipp Autopicker class",/* tp_doc */
                             0, /* tp_traverse */
                             0, /* tp_clear */
                             nullptr, /* tp_richcompare */
                             0, /* tp_weaklistoffset */
                             0, /* tp_iter */
                             0, /* tp_iternext */
                             nullptr, /* tp_methods */
                             0, /* tp_members */
                             0, /* tp_getset */
                             0, /* tp_base */
                             0, /* tp_dict */
                             0, /* tp_descr_get */
                             0, /* tp_descr_set */
                             0, /* tp_dictoffset */
                             0, /* tp_init */
                             0, /* tp_alloc */
                             Autopicker_new, /* tp_new */
                         };//AutopickerType

/* Destructor */
void Autopicker_dealloc(AutopickerObject* self)
{
    self->~AutopickerObject(); // Call the destructor
    Py_TYPE(self)->tp_free((PyObject*)self);
}//function Autopicker_dealloc

/* Constructor */
PyObject *
Autopicker_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    AutopickerObject *self = (AutopickerObject*)type->tp_alloc(type, 0);

    if (self != nullptr)
    {
        int particleSize;
        const char *modelName;
        PyObject *micrographs;
        
        if (!PyArg_ParseTuple(args, "isO!", &particleSize, &modelName, &PyList_Type, &micrographs))
        {
            return nullptr;
        }

        const auto nMicrographs = PyList_Size(micrographs);
        std::vector<MDRowSql> micrographRows;
	    for(Py_ssize_t i = 0; i < nMicrographs; i++)
	    {
            PyObject *item = PyList_GetItem(micrographs, i);
            if (!PyUnicode_Check(item)) {
                PyErr_Format(PyExc_TypeError, "Micrograph at index %zd must be a string.", i);
                return NULL; 
            }
            const char *micrographFn = PyUnicode_AsUTF8(item);

            MDRowSql row;
            row.setValueFromStr(MDL_MICROGRAPH, String(micrographFn));
            micrographRows.push_back(row);
	    }

        const int filterNum = 6;
        const int corrNum = 2;
        const int nPca = 4;
        self->autopicker = std::make_unique<AutoParticlePicking2>(
            particleSize,
            filterNum,
            corrNum,
            nPca,
            String(modelName),
            micrographRows
        );
    }
    return (PyObject *)self;
}//function Autopicker_new

PyObject *
Autopicker_train(PyObject *self, PyObject *args, PyObject *kwargs)
{
    int x;
    int y;
    int width;
    int height;
    PyObject *coordinates;
    if (!PyArg_ParseTuple(args, "O!iiii", &PyList_Type, &coordinates, &x, &y, &width, &height)) 
    {
        return NULL; 
    }

    MetaDataDb md;
    FileName micFile, posFile;
    std::vector<MDRowSql> rows;
    MDRowSql row;
    const auto nCoordinates = PyList_Size(coordinates);
    for (Py_ssize_t i = 0; i < nCoordinates; i++) {
        PyObject *item = PyList_GetItem(coordinates, i);
        if (!PyTuple_Check(item)) {
            PyErr_Format(PyExc_TypeError, "List item at index %zd must be a tuple.", i);
            return NULL;
        }

        if (PyTuple_Size(item) != 3) {
            PyErr_Format(PyExc_ValueError, "Tuple at index %zd must have exactly 3 items.", i);
            return NULL;
        }

        PyObject *micrograph = PyTuple_GetItem(item, 0);
        PyObject *x = PyTuple_GetItem(item, 1);
        PyObject *y = PyTuple_GetItem(item, 2);
        if (!PyUnicode_Check(micrograph) || !PyLong_Check(x) || !PyLong_Check(y)) {
            PyErr_Format(PyExc_TypeError, "Tuple at index %zd must contain a string and two integers.", i);
            return NULL;
        }

        row.setValue(MDL_MICROGRAPH, String(PyUnicode_AsUTF8(micrograph)));
        row.setValue(MDL_XCOOR, static_cast<int>(PyLong_AsLong(x)));
        row.setValue(MDL_YCOOR, static_cast<int>(PyLong_AsLong(y)));
        rows.push_back(row);
    }

    Autopicker_Value(self).train(rows, false, x, y, width, height); 
    Py_RETURN_NONE;
}

PyObject *
Autopicker_autopick(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *micrograph;
    int percent;
    if (!PyArg_ParseTuple(args, "si", &micrograph)) 
    {
        return NULL; 
    }

    std::vector<MDRowSql> md;
    Autopicker_Value(self).automaticallySelectParticles(String(micrograph), percent, md);
    const auto count = md.size();

    PyObject *coordinates = PyList_New(count);
    PyObject *costs = PyList_New(count);
    if (coordinates == NULL || costs == NULL) {
        Py_XDECREF(coordinates);
        Py_XDECREF(costs);
        return NULL;
    }

    int x, y;
    double c;
    for (Py_ssize_t i = 0; i < count; i++) {
        const auto &row = md[i];
        row.getValue(MDL_XCOOR, x);
		row.getValue(MDL_YCOOR, y);
		row.getValue(MDL_COST, c);

        PyObject *coordinate = Py_BuildValue("(ii)", x, y);
        PyObject *cost = PyFloat_FromDouble(c);
        if (coordinate == NULL || coordinate == NULL) {
            Py_XDECREF(coordinate);
            Py_XDECREF(cost);
            Py_DECREF(coordinates);
            Py_DECREF(costs);
            return NULL;
        }

        PyList_SetItem(coordinates, i, coordinate);
        PyList_SetItem(costs, i, cost);
    }

    return Py_BuildValue("(NN)", coordinates, costs);
}

PyObject *
Autopicker_correct(PyObject *self, PyObject *args, PyObject *kwargs)
{
    //Autopicker_Value(self).correction(); // TODO
    Py_RETURN_NONE;
}

PyObject *
Autopicker_setSize(PyObject *self, PyObject *args, PyObject *kwargs)
{
    int size;
    if (!PyArg_ParseTuple(args, "i", &size)) {
        return NULL; 
    }
    Autopicker_Value(self).setSize(size);
    Py_RETURN_NONE;
}

PyObject *
Autopicker_getParticlesThreshold(PyObject *self, PyObject*, PyObject*)
{
    const auto threshold = Autopicker_Value(self).getParticlesThreshold();
    return PyLong_FromLong(threshold);
}
