#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include "WaveFrontOBJ.h" 
#include "Vector.h"

using namespace std;
//------------------------------------------------------------------------------
// ��ü�� �����ϸ鼭 filename ���� object �� �д´�
// Construct object and read object from filename
WaveFrontOBJ::WaveFrontOBJ(char *filename) 
{
	isFlat = true;
	mode = GL_POLYGON;

	char *line = new char[200];
	char *line_back = new char[200];
	char wspace[] = " \t\n";
	char separator[] = "/";
	char *token;
	int indices[3];
	float x, y, z;
	float tex_u, tex_v;

	ifstream file(filename);
	if ( !file ) {
		cerr <<"Cannot open file: " <<filename <<" exiting." <<endl;
		exit ( -1 );
	}

	while ( !file.eof() ) {
		file.getline( line, 199 );
		// first, strip off comments
		if ( line[0] == '#' )
			continue;
		else if ( !strcmp( line, "" ) )
			continue;
		else {
			strcpy( line_back, line ); // strtok destroys line.
      
			token = strtok( line, wspace);

			if ( !strcmp( token, "v" ) ) {
				x = atof( strtok( NULL, wspace ) );
				y = atof( strtok( NULL, wspace ) );
				z = atof( strtok( NULL, wspace ) );
				verts.push_back( Vertex( x, y, z ) );
			}

			else if ( !strcmp( token, "vn" ) ) {
				x = atof( strtok( NULL, wspace ) );
				y = atof( strtok( NULL, wspace ) );
				z = atof( strtok( NULL, wspace ) );
				Vector vn(x, y, z);
				vn.Normalize();
				normals.push_back( vn );
			}

			else if ( !strcmp( token, "vt" ) ) {
				tex_u = atof( strtok( NULL, wspace ) );
				tex_v = atof( strtok( NULL, wspace ) );
				texCoords.push_back( TexCoord(tex_u, tex_v ) );
			}

			else if ( !strcmp( token, "f" ) ) {
				int vi = (int)vIndex.size();
				faces.push_back( Face( vi ) );
				Face& curFace = faces.back();
				for (char *p = strtok( NULL, wspace ); p ; p = strtok( NULL, wspace ) ) {
					indices[0] = 0;
					indices[1] = 0;
					indices[2] = 0;
					char* pos = p;
					int len = (int)strlen(p);

					for ( int j=0, i=0;  j <= len && i < 3; j++ ) {
						if ( p[j] == '/' || p[j] == 0) {
							p[j] = 0;
							indices[i++] = atoi( pos );
							pos = p + j+1;
						}
					}

					vIndex.push_back( indices[0] - 1 );
					tIndex.push_back( indices[1] - 1 );
					nIndex.push_back( indices[2] - 1 );                        
					curFace.vCount++;

					if (indices[2] != 0)
						isFlat = false;
				}
				if( curFace.vCount > 2 ){
					curFace.normal = faceNormal(verts[vIndex[vi]], verts[vIndex[vi+1]], verts[vIndex[vi+2]] );
					curFace.normal.Normalize();
					faceNormals.push_back(curFace.normal);
				}
			}


			else if ( !strcmp( token, "g" ) ) {      // group
			}
			else if ( !strcmp( token, "s" ) ) {      // smoothing group
			}
			else if ( !strcmp( token, "u" ) ) {      // material line
			}
			else if ( !strcmp( token, "" ) ) {       // blank line
			}
			else {
				cout <<line_back <<endl;
			}
		}
	}
	
	vertexNormal();

	computeBoundingBox();
}


//------------------------------------------------------------------------------
Vector WaveFrontOBJ::faceNormal(Vertex& v0, Vertex& v1, Vertex& v2) {
	/*******************************************************************/
	//(PA #4) : �� ���� ��ǥ�� �̿��Ͽ� face normal�� ����ϴ� �Լ��� �ϼ��Ͻʽÿ�.
	// - ����� face normal�� �� Face class�� normal �� ���� �ǵ��� �����Ͻʽÿ�.
	/*******************************************************************/
	Vector u = v0.pos - v1.pos; // face ���� ���� v1v0
	Vector v = v2.pos - v1.pos; // face ���� ���� v1v2
	Vector temp = v.CrossProduct(u); // ��� ���� �� ���͸� CrossProduct�ϸ� ��鿡 ������ normal vector�� ���� �� �ִ�.
	return temp;
}

void WaveFrontOBJ::vertexNormal() {
	/*******************************************************************/
	//(PA #4) : �ֺ� face normal�� �̿��Ͽ� vertex normal�� ����ϴ� �Լ��� �ϼ��Ͻʽÿ�.
	// - ����� vertex normal�� �� Vertex Class�� normal�� ���� �ǵ��� �����Ͻʽÿ�.
	/*******************************************************************/
	/*��� vertex���� �ϳ��� ���鼭 ���� ������ ���� �� vertex�� � face�� ���ԵǾ� �ִ� �� ã��,
	���� �����ִٸ�, �ش� face�� normal ���͸� vertex�� normal ���� ����Ͽ� ���Ѵ�. for���� ���� ��,
	���� ������ face�� ������� count���� ������ �� ������ ���� ������ normalize�Ͽ� vertex normal ���͸� ���Ͽ���.*/
	for (int k = 0; k < (int)verts.size(); k++) { //vertex�� �ϳ��� ���� for��
		int count = 0;//vertex�� ������ face�� ����� �����ִ� ����
		for (int j = 0; j < (int)faces.size(); j++) {//���� vertex�� � face�� �����ִ� �� ã�� ���� for��
			Face& curFace = faces[j];
			for (int i = curFace.vIndexStart; i < curFace.vIndexStart + curFace.vCount; i++) { 
				//���� face�� ���Ե� vertex���� �ϳ��ϳ� Ȯ���ϸ� ���� Ȯ���ϰ� �ִ� vertex�� ��ġ�ϴ� �� üũ�Ѵ�.
				if (k == vIndex[i]) {
					verts[k].normal = verts[k].normal + curFace.normal;//�����ִٸ� normal���� �߰��� ���Ѵ�.
					count++;
					break;
				}
			}
		}
		//������ face���� normal vector���� �� ���������� face ������ ������ ����� ���Ѵ�.
		verts[k].normal.x /= (float)count;
		verts[k].normal.y /= (float)count;
		verts[k].normal.z /= (float)count;
		verts[k].normal.Normalize();
	}
}

//------------------------------------------------------------------------------
// OpenGL API �� ����ؼ� ���Ͽ��� �о�� object �� �׸��� �Լ�.
// Draw object which is read from file
void WaveFrontOBJ::Draw() {
	int i;

	for (int f = 0; f < (int)faces.size(); f++) {
		Face& curFace = faces[f];        
		glBegin(mode);
		for (int v = 0; v < curFace.vCount; v++) {
			int vi = curFace.vIndexStart + v;
			
			if (isFlat) {
				if (v == 0) {
					glNormal3f(curFace.normal.x, curFace.normal.y, curFace.normal.z);
				}
			}
			
			else if ((i = vIndex[vi]) >= 0) {
				glNormal3f(verts[i].normal.x, verts[i].normal.y, verts[i].normal.z);
			}
			
			if ((i = tIndex[vi]) >= 0) {
				glTexCoord2f(texCoords[i].u, texCoords[i].v);
			}
			if ((i = vIndex[vi]) >= 0) {
				glVertex3f(verts[i].pos.x, verts[i].pos.y, verts[i].pos.z);
			}
		}
		glEnd();
	}
}

void WaveFrontOBJ::Draw_FN() {
	glDisable(GL_LIGHTING);
	/*******************************************************************/
	//(PA #4) : �� face�� ���� face normal�� �׸��� �Լ��� �ۼ��Ͻʽÿ�.
	// - �ۼ��� �Լ��� drawCow, drawbunny�� Ȱ���Ͻʽÿ�.
	/*******************************************************************/
	glBegin(GL_LINES);
	for (int k = 0; k < (int)faces.size(); k++) { //face normal vector�̹Ƿ� face���� ���鼭 �ϳ��� vector�� �׷��ִ� ����� �����ߴ�.
		Face& curFace = faces[k]; 
		Vertex temp; //���� face�� ������ ��� �������� vertex
		int vi = curFace.vIndexStart;
		if (curFace.vCount > 2) { //������ ���ϴ� ����
			temp.pos.x = (verts[vIndex[vi]].pos.x + verts[vIndex[vi + 1]].pos.x + verts[vIndex[vi + 2]].pos.x) / 3;
			temp.pos.y = (verts[vIndex[vi]].pos.y + verts[vIndex[vi + 1]].pos.y + verts[vIndex[vi + 2]].pos.y) / 3;
			temp.pos.z = (verts[vIndex[vi]].pos.z + verts[vIndex[vi + 1]].pos.z + verts[vIndex[vi + 2]].pos.z) / 3;
		}
		glColor3d(0, 1, 0);
		glVertex3d(temp.pos.x, temp.pos.y, temp.pos.z); //���� face�� ��������
		glVertex3d(temp.pos.x + curFace.normal.x, temp.pos.y + curFace.normal.y, temp.pos.z + curFace.normal.z);//face�� normal vector����
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void WaveFrontOBJ::Draw_VN() {
	glDisable(GL_LIGHTING);
	/*******************************************************************/
	//(PA #4) : �� vertex�� ���� vertex normal�� �׸��� �Լ��� �ۼ��Ͻʽÿ�.
	// - �ۼ��� �Լ��� drawCow, drawbunny�� Ȱ���Ͻʽÿ�.
	/*******************************************************************/
	glBegin(GL_LINES);
	for (int k = 0; k < (int)verts.size(); k++) {
		glColor3d(0, 0, 0);
		glVertex3d(verts[k].pos.x, verts[k].pos.y, verts[k].pos.z); //���� vertex������
		glVertex3d(verts[k].pos.x + verts[k].normal.x, verts[k].pos.y + verts[k].normal.y, verts[k].pos.z + verts[k].normal.z);//vertex normal vector ��ŭ
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------
void WaveFrontOBJ::computeBoundingBox()
{
	if( verts.size() > 0 )
	{
		bbmin.pos.x = verts[0].pos.x;
		bbmin.pos.y = verts[0].pos.y;
		bbmin.pos.z = verts[0].pos.z;
		bbmax.pos.x = verts[0].pos.x;
		bbmax.pos.y = verts[0].pos.y;
		bbmax.pos.z = verts[0].pos.z;
		for( int i=1; i < (int)verts.size(); i++ )
		{
			if( verts[i].pos.x < bbmin.pos.x ) bbmin.pos.x = verts[i].pos.x;
			if( verts[i].pos.y < bbmin.pos.y ) bbmin.pos.y = verts[i].pos.y;
			if( verts[i].pos.z < bbmin.pos.z ) bbmin.pos.z = verts[i].pos.z;
			if( verts[i].pos.x > bbmax.pos.x ) bbmax.pos.x = verts[i].pos.x;
			if( verts[i].pos.y > bbmax.pos.y ) bbmax.pos.y = verts[i].pos.y;
			if( verts[i].pos.z > bbmax.pos.z ) bbmax.pos.z = verts[i].pos.z;
		}
	}
}