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

//! \file glc_extendedgeomengine.cpp Implementation for the GLC_ExtendedGeomEngine class.

#include "glc_extendedgeomengine.h"
#include "../glc_state.h"

// Default constructor
GLC_ExtendedGeomEngine::GLC_ExtendedGeomEngine()
: GLC_GeomEngine()
, m_Positions()
, m_Normals()
, m_Texels()
, m_NormalVboId(0)
, m_TexelVboId(0)
, m_EngineLodList()
{
	m_EngineLodList.append(new GLC_EngineLod());

}

// Copy constructor
GLC_ExtendedGeomEngine::GLC_ExtendedGeomEngine(const GLC_ExtendedGeomEngine& engine)
: GLC_GeomEngine(engine)
, m_Positions(engine.m_Positions)
, m_Normals(engine.m_Normals)
, m_Texels(engine.m_Texels)
, m_NormalVboId(0)
, m_TexelVboId(0)
, m_EngineLodList()
{
	const int size= engine.m_EngineLodList.size();
	for (int i= 0; i < size; ++i)
	{
		m_EngineLodList.append(new GLC_EngineLod(*engine.m_EngineLodList.at(i)));
	}

}

GLC_ExtendedGeomEngine::~GLC_ExtendedGeomEngine()
{
	// Delete Texel VBO
	if (0 != m_TexelVboId)
	{
		glDeleteBuffers(1, &m_TexelVboId);
	}

	const int size= m_EngineLodList.size();
	for (int i= 0; i < size; ++i)
	{
		delete m_EngineLodList.at(i);
	}
}
//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return the Position Vector
GLfloatVector GLC_ExtendedGeomEngine::positionVector() const
{
	if (0 != m_VboId)
	{
		// VBO created get data from VBO
		const int sizeOfVbo= m_Positions.size();
		const GLsizeiptr dataSize= sizeOfVbo * sizeof(float);
		GLfloatVector positionVector(sizeOfVbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		GLvoid* pVbo = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(positionVector.data(), pVbo, dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return positionVector;
	}
	else
	{
		return m_Positions;
	}

}

// Return the normal Vector
GLfloatVector GLC_ExtendedGeomEngine::normalVector() const
{
	if (0 != m_NormalVboId)
	{
		// VBO created get data from VBO
		const int sizeOfVbo= m_Normals.size();
		const GLsizeiptr dataSize= sizeOfVbo * sizeof(float);
		GLfloatVector normalVector(sizeOfVbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_NormalVboId);
		GLvoid* pVbo = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		memcpy(normalVector.data(), pVbo, dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return normalVector;
	}
	else
	{
		return m_Normals;
	}
}



//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
//! Vbo creation
void GLC_ExtendedGeomEngine::createVBOs()
{
	// Create position VBO
	if (0 == m_VboId)
	{
		glGenBuffers(1, &m_VboId);
		glGenBuffers(1, &m_NormalVboId);

		// Create Texel VBO
		if (0 == m_TexelVboId and not m_Texels.isEmpty())
		{
			glGenBuffers(1, &m_TexelVboId);
		}

		const int size= m_EngineLodList.size();
		for (int i= 0; i < size; ++i)
		{
			m_EngineLodList.at(i)->createIBOs();
		}
	}
}
//! Ibo Usage
bool GLC_ExtendedGeomEngine::useVBO(bool use, GLC_ExtendedGeomEngine::VboType type)
{
	bool result= true;
	if (use)
	{
		// Chose the right VBO
		if (type == GLC_ExtendedGeomEngine::GLC_Vertex)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
		}
		else if (type == GLC_ExtendedGeomEngine::GLC_Normal)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_NormalVboId);
		}
		else if ((type == GLC_ExtendedGeomEngine::GLC_Texel) and (0 != m_TexelVboId))
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_TexelVboId);
		}
		else result= false;
	}
	else
	{
		// Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	return result;

}

//! Vbo Usage
void GLC_ExtendedGeomEngine::useIBO(bool use, GLC_EngineLod::IboType type)
{
	if (use)
	{
		const int size= m_EngineLodList.size();
		for (int i= 0; i < size; ++i)
		{
			m_EngineLodList.at(i)->useIBO(type);
		}
	}
	else
	{
		// Unbind Ibo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

