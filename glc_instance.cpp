/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_instance.cpp implementation of the GLC_Instance class.

#include "glc_instance.h"
#include "glc_selectionmaterial.h"
#include "glc_state.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default constructor
GLC_Instance::GLC_Instance()
: GLC_Object()
, m_pGeom(NULL)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsBoundingBoxValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)
, m_IsVisible(true)
{
	// Encode Color Id
	encodeIdInRGBA();

	//qDebug() << "GLC_Instance::GLC_Instance null instance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Contruct instance with a geometry
GLC_Instance::GLC_Instance(GLC_VboGeom* pGeom)
: GLC_Object()
, m_pGeom(pGeom)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(new int(1))
, m_MatPos()
, m_IsBoundingBoxValid(false)
, m_IsSelected(false)
, m_PolyFace(GL_FRONT_AND_BACK)
, m_PolyMode(GL_FILL)
, m_IsVisible(true)
{
	// Encode Color Id
	encodeIdInRGBA();

	setName(pGeom->name());

	//qDebug() << "GLC_Instance::GLC_Instance ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}

// Copy constructor
GLC_Instance::GLC_Instance(const GLC_Instance& inputNode)
: GLC_Object(inputNode)
, m_pGeom(inputNode.m_pGeom)
, m_pBoundingBox(NULL)
, m_pNumberOfInstance(inputNode.m_pNumberOfInstance)
, m_MatPos(inputNode.m_MatPos)
, m_IsBoundingBoxValid(inputNode.m_IsBoundingBoxValid)
, m_IsSelected(inputNode.m_IsSelected)
, m_PolyFace(inputNode.m_PolyFace)
, m_PolyMode(inputNode.m_PolyMode)
, m_IsVisible(inputNode.m_IsVisible)

{
	// Encode Color Id
	encodeIdInRGBA();

	if (NULL != inputNode.m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox(*inputNode.m_pBoundingBox);
	}
	// Increment the number of instance
	++(*m_pNumberOfInstance);
	//qDebug() << "GLC_Instance::GLC_Instance CopyConstructor ID = " << m_Uid;
	//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
}


// Assignement operator
GLC_Instance& GLC_Instance::operator=(const GLC_Instance& inputNode)
{
	if (this != &inputNode)
	{
		clear();
		m_Uid= inputNode.m_Uid;
		// Encode Color Id
		encodeIdInRGBA();

		m_Name= inputNode.m_Name;

		m_pGeom= inputNode.m_pGeom;
		if (NULL != inputNode.m_pBoundingBox)
		{
			m_pBoundingBox= new GLC_BoundingBox(*inputNode.m_pBoundingBox);
		}
		m_pNumberOfInstance= inputNode.m_pNumberOfInstance;
		++(*m_pNumberOfInstance);
		m_MatPos= inputNode.m_MatPos;
		m_IsBoundingBoxValid= inputNode.m_IsBoundingBoxValid;
		m_IsSelected= inputNode.m_IsSelected;
		m_PolyFace= inputNode.m_PolyFace;
		m_PolyMode= inputNode.m_PolyMode;
		m_IsVisible= inputNode.m_IsVisible;

		//qDebug() << "GLC_Instance::operator= :ID = " << m_Uid;
		//qDebug() << "Number of instance" << (*m_pNumberOfInstance);
	}

	return *this;
}

// Destructor
GLC_Instance::~GLC_Instance()
{
	clear();
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get the bounding box
GLC_BoundingBox GLC_Instance::getBoundingBox(void)
{
	GLC_BoundingBox resultBox;
	if (getBoundingBoxValidity())
	{
		resultBox= *m_pBoundingBox;
	}
	else if (m_pGeom != NULL)
	{
		computeBoundingBox();
		m_IsBoundingBoxValid= true;
		resultBox= *m_pBoundingBox;
	}

	return resultBox;
}

// Get the validity of the Bounding Box
const bool GLC_Instance::getBoundingBoxValidity(void) const
{
	bool result;
	if ((m_pGeom != NULL) && (m_pBoundingBox != NULL))
	{
		result= m_pGeom->boundingBoxIsValid() and m_IsBoundingBoxValid;
	}
	else result= false;

	return result;
}

// Clone the instance
GLC_Instance GLC_Instance::clone() const
{
	GLC_Instance cloneInstance;
	cloneInstance.m_pGeom= m_pGeom->clone();
	if (NULL != m_pBoundingBox)
	{
		cloneInstance.m_pBoundingBox= new GLC_BoundingBox(*m_pBoundingBox);
	}

	cloneInstance.m_MatPos= m_MatPos;
	cloneInstance.m_IsBoundingBoxValid= m_IsBoundingBoxValid;
	cloneInstance.m_IsSelected= m_IsSelected;
	cloneInstance.m_PolyFace= m_PolyFace;
	cloneInstance.m_PolyMode= m_PolyMode;
	cloneInstance.m_IsVisible= m_IsVisible;
	return cloneInstance;
}

// Instanciate the instance
GLC_Instance GLC_Instance::instanciate()
{
	GLC_Instance instance(*this);
	instance.m_Uid= glc::GLC_GenID();
	// Encode Color Id
	encodeIdInRGBA();

	return instance;
}

// Return the GLC_uint decoded ID from RGBA encoded ID
GLC_uint GLC_Instance::decodeRgbId(const GLubyte* pcolorId)
{
	GLC_uint returnId= 0;
	returnId|= (GLC_uint)pcolorId[0] << (0 * 8);
	returnId|= (GLC_uint)pcolorId[1] << (1 * 8);
	returnId|= (GLC_uint)pcolorId[2] << (2 * 8);
	// Only get first 24 bits
	//returnId|= (GLC_uint)pcolorId[3] << (3 * 8);

	return returnId;
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////


// Set the instance Geometry
bool GLC_Instance::setGeometry(GLC_VboGeom* pGeom)
{
		if (NULL == m_pGeom) return false;
		else
		{
			m_pGeom= pGeom;
			return true;
		}
}

// Instance translation
GLC_Instance& GLC_Instance::translate(double Tx, double Ty, double Tz)
{
	multMatrix(GLC_Matrix4x4(Tx, Ty, Tz));

	return *this;
}


// Move instance with a 4x4Matrix
GLC_Instance& GLC_Instance::multMatrix(const GLC_Matrix4x4 &MultMat)
{
	m_MatPos= MultMat * m_MatPos;
	m_IsBoundingBoxValid= false;

	return *this;
}

// Replace the instance Matrix
GLC_Instance& GLC_Instance::setMatrix(const GLC_Matrix4x4 &SetMat)
{
	m_MatPos= SetMat;
	m_IsBoundingBoxValid= false;

	return *this;
}

// Reset the instance Matrix
GLC_Instance& GLC_Instance::resetMatrix(void)
{
	m_MatPos.setToIdentity();
	m_IsBoundingBoxValid= false;

	return *this;
}

// Polygon's display style
void GLC_Instance::setPolygonMode(GLenum Face, GLenum Mode)
{
	// Change the polygon mode only if there is a change
	if ((m_PolyFace != Face) || (m_PolyMode != Mode))
	{
		m_PolyFace= Face;
		m_PolyMode= Mode;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Display the instance
void GLC_Instance::glExecute(bool transparent)
{
	if (NULL == m_pGeom) return;


	// Object ID for selection purpose
	//glLoadName(id());

	// Save current OpenGL Matrix
	glPushMatrix();
	glVisProperties();
	if(GLC_State::isInSelectionMode())
	{
		// D'ont use Alpha component
		glColor3ubv(m_colorId);
	}
	m_pGeom->glExecute(m_IsSelected, transparent);
	// Restore OpenGL Matrix
	glPopMatrix();
}

// Set instance visualisation properties
void GLC_Instance::glVisProperties()
{
	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);
	// Change the current matrix
	glMultMatrixd(m_MatPos.data());

}

//////////////////////////////////////////////////////////////////////
// private services functions
//////////////////////////////////////////////////////////////////////


// compute the instance bounding box
// m_pGeom should be not null
void GLC_Instance::computeBoundingBox(void)
{
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}

	m_pBoundingBox= new GLC_BoundingBox(m_pGeom->boundingBox());
	m_pBoundingBox->transform(m_MatPos);
}

// Clear current instance
void GLC_Instance::clear()
{
	Q_ASSERT(m_pNumberOfInstance != NULL);

	if ((--(*m_pNumberOfInstance)) == 0)
	{
		// this is the last instance, delete the geometry
		if (m_pGeom != NULL)
		{
			delete m_pGeom;
			m_pGeom= NULL;
		}
		// delete instance counter
		//qDebug() << "GLC_Instance::clear ID = " << m_Uid;
		//qDebug() << "- Number of instance" << (*m_pNumberOfInstance);
		delete m_pNumberOfInstance;
		m_pNumberOfInstance= NULL;

	}
	else
	{
		//qDebug() << "GLC_Instance::clear ID = " << m_Uid;
		//qDebug() << " - Number of instance" << (*m_pNumberOfInstance);
	}


	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}

	// invalidate the bounding box
	m_IsBoundingBoxValid= false;

}

//! Encode Id to RGBA color
void GLC_Instance::encodeIdInRGBA()
{
	m_colorId[0]= static_cast<GLubyte>((m_Uid >> (0 * 8)) & 0xFF);
	m_colorId[1]= static_cast<GLubyte>((m_Uid >> (1 * 8)) & 0xFF);
	m_colorId[2]= static_cast<GLubyte>((m_Uid >> (2 * 8)) & 0xFF);
	m_colorId[3]= static_cast<GLubyte>((m_Uid >> (3 * 8)) & 0xFF);
}



