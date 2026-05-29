/***************************************************************************
 *
 * Authors: Oier Lauzirika Zarrabeitia
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

#ifndef _PYTHON_AUTOPICKER_H
#define _PYTHON_AUTOPICKER_H

#include "Python.h"
#include <reconstruction/micrograph_automatic_picking2.h>
#include <memory>

extern PyObject * PyXmippError;
/***************************************************************/
/*                            Autopicker                       */
/***************************************************************/

#define Autopicker_Check(v) (((v)->ob_type == &AutopickerType))
#define Autopicker_Value(v) ((*((AutopickerObject*)(v))->autopicker))

/*Autopicker Object*/
typedef struct
{
    PyObject_HEAD
    std::unique_ptr<AutoParticlePicking2> autopicker;
}
AutopickerObject;

/* Destructor */
void Autopicker_dealloc(AutopickerObject* self);

/* Constructor */
PyObject *
Autopicker_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);

PyObject *
Autopicker_train(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
Autopicker_autopick(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
Autopicker_correct(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
Autopicker_setSize(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
Autopicker_getParticlesThreshold(PyObject *self, PyObject*, PyObject*);

extern PyMethodDef Autopicker_methods[];
extern PyTypeObject AutopickerType;

#endif
