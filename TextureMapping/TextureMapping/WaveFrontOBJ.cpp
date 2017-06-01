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
// 객체를 생성하면서 filename 에서 object 를 읽는다
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
	//(PA #4) : 세 점의 좌표를 이용하여 face normal을 계산하는 함수를 완성하십시오.
	// - 계산한 face normal이 각 Face class의 normal 에 저장 되도록 구현하십시오.
	/*******************************************************************/
	Vector u = v0.pos - v1.pos; // face 위의 벡터 v1v0
	Vector v = v2.pos - v1.pos; // face 위의 벡터 v1v2
	Vector temp = v.CrossProduct(u); // 평면 위의 두 벡터를 CrossProduct하면 평면에 수직한 normal vector를 얻을 수 있다.
	return temp;
}

void WaveFrontOBJ::vertexNormal() {
	/*******************************************************************/
	//(PA #4) : 주변 face normal을 이용하여 vertex normal을 계산하는 함수를 완성하십시오.
	// - 계산한 vertex normal이 각 Vertex Class의 normal에 저장 되도록 구현하십시오.
	/*******************************************************************/
	/*모든 vertex들을 하나씩 돌면서 삼중 포문을 통해 이 vertex가 어떤 face에 포함되어 있는 지 찾고,
	만약 속해있다면, 해당 face의 normal 벡터를 vertex의 normal 값에 계속하여 더한다. for문이 끝난 후,
	나와 인접한 face가 몇개였는지 count해준 변수로 각 포지션 값을 나누고 normalize하여 vertex normal 벡터를 구하였다.*/
	for (int k = 0; k < (int)verts.size(); k++) { //vertex를 하나씩 도는 for문
		int count = 0;//vertex와 인접한 face가 몇개인지 세어주는 변수
		for (int j = 0; j < (int)faces.size(); j++) {//현재 vertex가 어떤 face에 속해있는 지 찾기 위한 for문
			Face& curFace = faces[j];
			for (int i = curFace.vIndexStart; i < curFace.vIndexStart + curFace.vCount; i++) { 
				//현재 face에 포함된 vertex들을 하나하나 확인하며 지금 확인하고 있는 vertex와 일치하는 지 체크한다.
				if (k == vIndex[i]) {
					verts[k].normal = verts[k].normal + curFace.normal;//속해있다면 normal값에 추가로 더한다.
					count++;
					break;
				}
			}
		}
		//인접한 face들의 normal vector값이 다 더해졌으면 face 개수로 나누어 평균을 구한다.
		verts[k].normal.x /= (float)count;
		verts[k].normal.y /= (float)count;
		verts[k].normal.z /= (float)count;
		verts[k].normal.Normalize();
	}
}

//------------------------------------------------------------------------------
// OpenGL API 를 사용해서 파일에서 읽어둔 object 를 그리는 함수.
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
	//(PA #4) : 각 face에 대해 face normal을 그리는 함수를 작성하십시오.
	// - 작성한 함수는 drawCow, drawbunny에 활용하십시오.
	/*******************************************************************/
	glBegin(GL_LINES);
	for (int k = 0; k < (int)faces.size(); k++) { //face normal vector이므로 face들을 돌면서 하나씩 vector를 그려주는 방법을 생각했다.
		Face& curFace = faces[k]; 
		Vertex temp; //현재 face의 중점을 잠시 저장해줄 vertex
		int vi = curFace.vIndexStart;
		if (curFace.vCount > 2) { //중점을 구하는 과정
			temp.pos.x = (verts[vIndex[vi]].pos.x + verts[vIndex[vi + 1]].pos.x + verts[vIndex[vi + 2]].pos.x) / 3;
			temp.pos.y = (verts[vIndex[vi]].pos.y + verts[vIndex[vi + 1]].pos.y + verts[vIndex[vi + 2]].pos.y) / 3;
			temp.pos.z = (verts[vIndex[vi]].pos.z + verts[vIndex[vi + 1]].pos.z + verts[vIndex[vi + 2]].pos.z) / 3;
		}
		glColor3d(0, 1, 0);
		glVertex3d(temp.pos.x, temp.pos.y, temp.pos.z); //현재 face의 중점부터
		glVertex3d(temp.pos.x + curFace.normal.x, temp.pos.y + curFace.normal.y, temp.pos.z + curFace.normal.z);//face의 normal vector까지
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void WaveFrontOBJ::Draw_VN() {
	glDisable(GL_LIGHTING);
	/*******************************************************************/
	//(PA #4) : 각 vertex에 대해 vertex normal을 그리는 함수를 작성하십시오.
	// - 작성한 함수는 drawCow, drawbunny에 활용하십시오.
	/*******************************************************************/
	glBegin(GL_LINES);
	for (int k = 0; k < (int)verts.size(); k++) {
		glColor3d(0, 0, 0);
		glVertex3d(verts[k].pos.x, verts[k].pos.y, verts[k].pos.z); //현재 vertex점부터
		glVertex3d(verts[k].pos.x + verts[k].normal.x, verts[k].pos.y + verts[k].normal.y, verts[k].pos.z + verts[k].normal.z);//vertex normal vector 만큼
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