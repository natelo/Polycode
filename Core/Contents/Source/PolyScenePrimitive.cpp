/*
 Copyright (C) 2011 by Ivan Safrin
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
 
#include "PolyScenePrimitive.h"
#include "PolyMesh.h"

using namespace Polycode;

ScenePrimitive::ScenePrimitive(int type, Number v1, Number v2, Number v3,Number v4,Number v5) : SceneMesh(Mesh::QUAD_MESH) {

	this->type = type;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->v4 = v4;
	this->v5 = v5;
	
	recreatePrimitive();
}

int ScenePrimitive::getPrimitiveType() const {
    return type;
}

Number ScenePrimitive::getPrimitiveParameter1() const {
    return v1;
}

Number ScenePrimitive::getPrimitiveParameter2() const {
    return v2;
}
    
Number ScenePrimitive::getPrimitiveParameter3() const {
    return v3;
}

Number ScenePrimitive::getPrimitiveParameter4() const {
    return v4;
}

Number ScenePrimitive::getPrimitiveParameter5() const {
    return v5;
}

void ScenePrimitive::recreatePrimitive() {
	mesh->clearMesh();
	switch(type) {
		case TYPE_PLANE:
			mesh->createPlane(v1,v2);
			bBox.x = v1;
			bBox.y = 0.001;
			bBox.z = v2;
		break;
		case TYPE_VPLANE:
			mesh->createVPlane(v1,v2);
			bBox.x = v1;
			bBox.y = v2;
			bBox.z = 0.001;
		break;		
		case TYPE_BOX:
			mesh->createBox(v1,v2,v3);
			bBox.x = v1;
			bBox.y = v2;
			bBox.z = v3;			
		break;
		case TYPE_SPHERE:
			mesh->createSphere(v1,v2,v3);
			bBox.x = v1*2;
			bBox.y = v1*2;
			bBox.z = v1*2;						
		break;
		case TYPE_CYLINDER:
			mesh->createCylinder(v1,v2,v3);
			bBox.x = v2*2;
			bBox.y = v1;
			bBox.z = v2*2;						
		break;
		case TYPE_UNCAPPED_CYLINDER:
			mesh->createCylinder(v1,v2,v3, false);
			bBox.x = v2*2;
			bBox.y = v1;
			bBox.z = v2*2;						
		break;						
		case TYPE_CONE:
			mesh->createCone(v1,v2,v3);
			bBox.x = v2*2;
			bBox.y = v1;
			bBox.z = v2*2;						
		break;				
		case TYPE_TORUS:
			mesh->createTorus(v1,v2,v3,v4);
			bBox.x = v1*2;
			bBox.y = v2;
			bBox.z = v1*2;						
		break;						
		case TYPE_CIRCLE:
			mesh->createCircle(v1, v2, v3);
			bBox.x = v1;
			bBox.y = v2;
			bBox.z = 0.001;
		break;
		case TYPE_LINE_CIRCLE:
			mesh->createLineCircle(v1, v2, v3);
			bBox.x = v1;
			bBox.y = v2;
			bBox.z = 0.001;
        break;
	}
}

void ScenePrimitive::setPrimitiveOptions(int type, Number v1, Number v2, Number v3,Number v4,Number v5) {

	this->type = type;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->v4 = v4;
	this->v5 = v5;

	recreatePrimitive();
}

ScenePrimitive::~ScenePrimitive() {

}
