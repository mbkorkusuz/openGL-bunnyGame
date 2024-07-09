#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <random>
#include <ft2build.h>
#include FT_FREETYPE_H
#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define BUFFER_OFFSET(i) ((char*)NULL + (i))
using namespace std;

float vec[] = {1.f,  1.f, 0.0f, 1.0f, 1.0f,1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,-1.0f,  1.0f, 0.0f, 0.0f, 1.0f};
unsigned int ind[] = {0, 1, 3,1, 2, 3};
GLuint gProgram[4];
int gWidth, gHeight;
float rabbitLeftRight = 0;
float rabbitLeftRightIncrement = 0;
int keyAPress = 0;
int keyARelease = 0;
int keyDPress = 0;
int keyDRelease = 0;
int collisionYellow = 0;
int collisionRed = 0;
float cubearr[3] = {-2.5, 0, 2.5};
int index1, index2, randed;
int collisioncount = 0;
float firstCollisionCount = 0;
float angle = 0;
float angle_fall = 0;
int fall = 0;
int restart =0;
int yellowCubeRemove = 0;
float score = 0;
float speed = 1;
float speedRabbit = 1;
string scoreText;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 0, 0);

int activeProgramIndex = 0;
int widthP, heightP, nrChannels;
unsigned char *imageData;
GLuint texture;

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};

struct Character {
    GLuint TextureID;   
    glm::ivec2 Size;    
    glm::ivec2 Bearing; 
    GLuint Advance;    
};

std::map<GLchar, Character> Characters;
vector<Vertex> gVertices;
vector<Texture> gTextures;
vector<Normal> gNormals;
vector<Face> gFaces;
vector<Vertex> gCubeVertices;
vector<Texture> gCubeTextures;
vector<Normal> gCubeNormals;
vector<Face> gCubeFaces;

GLuint vao[4];
GLuint vbo[4];
GLuint ebo[4];

GLuint gTextVBO;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
int facesbunny;
int facesquad;
int facesCube;
bool ParseObj(const string& fileName)
{
	fstream myfile;
	gFaces.clear();
	gVertices.clear();
	gNormals.clear();
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;
		while (getline(myfile, curLine))
		{
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			string tmp;
			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't')
					{
						str >> tmp;
						str >> c1 >> c2;			
						gTextures.push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n')
					{
						str >> tmp;
						str >> c1 >> c2 >> c3;
						gNormals.push_back(Normal(c1, c2, c3));
					}
					else 
					{
						str >> tmp;
						str >> c1 >> c2 >> c3;			
						gVertices.push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f')
				{
					str >> tmp;
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c;
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c;
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c;
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); 

					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}
					gFaces.push_back(Face(vIndex, tIndex, nIndex));
				}
			}
		}
		myfile.close();
	}
	else
	{
		return false;
	}
	assert(gVertices.size() == gNormals.size());
	return true;
}
bool ReadDataFromFile(const string& fileName,string& data)     
{
	fstream myfile;
	myfile.open(fileName.c_str(), std::ios::in);
	if (myfile.is_open())
	{
		string curLine;
		while (getline(myfile, curLine))
		{
			data += curLine;
			if (!myfile.eof())
			{
				data += "\n";
			}
		}
		myfile.close();
	}
	else
	{
		return false;
	}
	return true;
}
GLuint createVS(const char* shaderName)
{
	string shaderSource;
	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);
	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	return vs;
}
GLuint createFS(const char* shaderName)
{
	string shaderSource;
	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}
	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);
	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	return fs;
}
void initShaders()
{
	gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();
    gProgram[2] = glCreateProgram();
	gProgram[3] = glCreateProgram();

	GLuint vs1 = createVS("vert.glsl");
	GLuint fs1 = createFS("frag.glsl");

	GLuint vs2 = createVS("cubeVertex.glsl");
	GLuint fs2 = createFS("cubeFrag.glsl");

	GLuint vs3 = createVS("vert_text.glsl");
	GLuint fs3 = createFS("frag_text.glsl");

	GLuint vs4 = createVS("vertexTexture.glsl");
	GLuint fs4 = createFS("fragTexture.glsl");

	glAttachShader(gProgram[0], vs1);
	glAttachShader(gProgram[0], fs1);

	glAttachShader(gProgram[1], vs2);
	glAttachShader(gProgram[1], fs2);
	
	glAttachShader(gProgram[2], vs3);
	glAttachShader(gProgram[2], fs3);
	glBindAttribLocation(gProgram[2], 2, "vertex");

	glAttachShader(gProgram[3], vs4);
	glAttachShader(gProgram[3], fs4);

	glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[2]);
	glGetProgramiv(gProgram[2], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[3]);
	glGetProgramiv(gProgram[3], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	for (int i = 0; i < 2; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
}
void initBunnyVBO()
{
	glGenVertexArrays(1, &vao[0]);
	assert(vao[0] > 0);
	glBindVertexArray(vao[0]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vbo[0]);
	glGenBuffers(1, &ebo[0]);

	assert(vbo[0] > 0 && ebo[0] > 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

	gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[gVertices.size() * 3];
	GLfloat* normalData = new GLfloat[gNormals.size() * 3];
	GLuint* indexData = new GLuint[gFaces.size() * 3];

	for (int i = 0; i < gVertices.size(); ++i)
	{
		vertexData[3 * i] = gVertices[i].x;
		vertexData[3 * i + 1] = gVertices[i].y;
		vertexData[3 * i + 2] = gVertices[i].z;
	}

	for (int i = 0; i < gNormals.size(); ++i)
	{
		normalData[3 * i] = gNormals[i].x;
		normalData[3 * i + 1] = gNormals[i].y;
		normalData[3 * i + 2] = gNormals[i].z;
	}

	for (int i = 0; i < gFaces.size(); ++i)
	{
		indexData[3 * i] = gFaces[i].vIndex[0];
		indexData[3 * i + 1] = gFaces[i].vIndex[1];
		indexData[3 * i + 2] = gFaces[i].vIndex[2];
	}

	facesbunny = gFaces.size()*3;

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}
void initQuadVBO()
{
	glGenVertexArrays(1, &vao[2]);
	assert(vao[2] > 0);
	glBindVertexArray(vao[2]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vbo[2]);
	glGenBuffers(1, &ebo[2]);

	assert(vbo[2] > 0 && ebo[2] > 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);

	gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[gVertices.size() * 3];
	GLfloat* normalData = new GLfloat[gNormals.size() * 3];
	GLuint* indexData = new GLuint[gFaces.size() * 3];

	for (int i = 0; i < gVertices.size(); ++i)
	{
		vertexData[3 * i] = gVertices[i].x;
		vertexData[3 * i + 1] = gVertices[i].y;
		vertexData[3 * i + 2] = gVertices[i].z;
	}

	for (int i = 0; i < gNormals.size(); ++i)
	{
		normalData[3 * i] = gNormals[i].x;
		normalData[3 * i + 1] = gNormals[i].y;
		normalData[3 * i + 2] = gNormals[i].z;
	}

	for (int i = 0; i < gFaces.size(); ++i)
	{
		indexData[3 * i] = gFaces[i].vIndex[0];
		indexData[3 * i + 1] = gFaces[i].vIndex[1];
		indexData[3 * i + 2] = gFaces[i].vIndex[2];
	}

	facesquad = gFaces.size()*3;

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}
void initCubeVBO()
{
	glGenVertexArrays(1, &vao[1]);
	assert(vao[1] > 0);
	glBindVertexArray(vao[1]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vbo[1]);
	glGenBuffers(1, &ebo[1]);

	assert(vbo[1] > 0 && ebo[1] > 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);

	gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[gVertices.size() * 3];
	GLfloat* normalData = new GLfloat[gNormals.size() * 3];
	GLuint* indexData = new GLuint[gFaces.size() * 3];

	for (int i = 0; i < gVertices.size(); ++i)
	{
		vertexData[3 * i] = gVertices[i].x;
		vertexData[3 * i + 1] = gVertices[i].y;
		vertexData[3 * i + 2] = gVertices[i].z;
	}

	for (int i = 0; i < gNormals.size(); ++i)
	{
		normalData[3 * i] = gNormals[i].x;
		normalData[3 * i + 1] = gNormals[i].y;
		normalData[3 * i + 2] = gNormals[i].z;
	}

	for (int i = 0; i < gFaces.size(); ++i)
	{
		indexData[3 * i] = gFaces[i].vIndex[0];
		indexData[3 * i + 1] = gFaces[i].vIndex[1];
		indexData[3 * i + 2] = gFaces[i].vIndex[2];
	}

	facesCube = gFaces.size()*3;

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}
void initSkyVBO()
{
    glGenVertexArrays(1, &vao[3]);
    glGenBuffers(1, &vbo[3]);
    glGenBuffers(1, &ebo[3]);

    glBindVertexArray(vao[3]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec), vec, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
	
	glGenTextures(1, &texture); 
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);

	imageData = stbi_load("sky.jpg", &widthP, &heightP, &nrChannels, 0);
	if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthP, heightP, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
	stbi_image_free(imageData);
	
	glUseProgram(gProgram[3]);
	glUniform1i(glGetUniformLocation(gProgram[3], "ourTexture"), 0);
}
void initFonts(int windowWidth, int windowHeight)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(gProgram[2]);
    glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void initBunny()
{
	ParseObj("bunny.obj");
	glEnable(GL_DEPTH_TEST);
	initShaders();
	initBunnyVBO();
}
void initCube()
{
	ParseObj("cube.obj");
	glEnable(GL_DEPTH_TEST);
	initShaders();
	initCubeVBO();
}
void initQuad()
{
	ParseObj("quad.obj");
	glEnable(GL_DEPTH_TEST);
	initShaders();
	initQuadVBO();
}
void drawBunny()
{
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, facesbunny, GL_UNSIGNED_INT, 0);
}
void drawCube()
{
	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, facesCube, GL_UNSIGNED_INT, 0);
}
void drawQuad()
{
	glBindVertexArray(vao[2]);
	glDrawElements(GL_TRIANGLES, facesquad, GL_UNSIGNED_INT, 0);
}
void drawSky()
{
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(gProgram[3]);
	glBindVertexArray(vao[3]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDepthMask(GL_TRUE);
}
void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    glUseProgram(gProgram[2]);
    glUniform3f(glGetUniformLocation(gProgram[2], "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void display()
{
	speed += 0.001;
	speedRabbit += 0.0001;
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	drawSky();

	activeProgramIndex = 0;
	static float angle = 0;
	static float rabbit = 0;
	static float increment = 0.2;
	static float cubeMovement = 0;
	static float cubeIncrement = 0.1;

	static float quadMovement = 0;
	static float quadIncrement = 0.1;

	if(restart == 1)
	{
		speed = 1;
		collisionRed = 0;
		angle = 0;
		rabbit = 0;
		increment = 0.2;
		cubeMovement = 0;
		cubeIncrement = 0.1;
		quadMovement = 0;
		quadIncrement = 0.1;
		fall = 0;
		angle_fall = 0;
		collisioncount = 0;
		collisionYellow = 0;
		keyAPress = 0;
		keyARelease = 0;
		keyDPress = 0;
		keyDRelease = 0;
		score = 0;
		yellowCubeRemove = 0;
		speedRabbit = 1;
	}

	if(rabbit >2 )
	{
		increment = -0.2;
	}
	if(rabbit <0 )
	{
		increment = 0.2;
	}
	
	rabbit += increment * speedRabbit;

	if(cubeMovement > 40 )
	{
		cubeMovement = 0;
		yellowCubeRemove = 0;
	}

	if(cubeMovement == 0)
	{	
		std::random_device rd; 
    	std::mt19937 gen(rd()); 
    	std::uniform_int_distribution<> dis(0, 2); 

    	randed = dis(gen);
		
		if(randed == 0){
			index1 = 1;
			index2 = 2;
		}
		if(randed == 1){
			index1 = 0;
			index2 = 2;
		}
		if(randed == 2){
			index1 = 0;
			index2 = 1;
		}
	}
	score += quadIncrement * 3 * speed;
	quadMovement += quadIncrement * speed;
	if(quadMovement > 4 )
	{
		quadMovement = 0;
	}


	if(randed == 0 &&  36.8 >= cubeMovement && cubeMovement >= 35.8) // yellow left
	{
		if(rabbitLeftRight >= 4.5)
		{
			collisionYellow = 1;
			yellowCubeRemove = 1;
		}
		else if((rabbitLeftRight <= 2.25 && rabbitLeftRight >= -2.25)) // red collision
		{
			collisionRed = 1;
			fall = 2;
		}
		else if((rabbitLeftRight <= -4.5)) // red collision
		{
			collisionRed = 1;
			fall = 3;
		}

	}
	if(randed == 1 &&  36.8 >= cubeMovement && cubeMovement >= 35.8) // yellow middle
	{
		if(rabbitLeftRight <= 2.25 && rabbitLeftRight >= -2.25)
		{
			collisionYellow = 1;
			yellowCubeRemove = 1;
		}
		else if((rabbitLeftRight >= 4.5)) // red collision
		{
			collisionRed = 1;
			fall = 1;
		}
		else if((rabbitLeftRight <= -4.5)) // red collision
		{
			collisionRed = 1;
			fall = 3;
		}

	}
	if(randed == 2 &&  36.8 >= cubeMovement && cubeMovement >= 35.8) // yellow right
	{
		if(rabbitLeftRight <= -4.5)
		{
			collisionYellow = 1;
			yellowCubeRemove = 1;
		}
		else if((rabbitLeftRight >= 4.5)) // red collision
		{
			collisionRed = 1;
			fall = 1;
		}
		else if((rabbitLeftRight <= 2.25 && rabbitLeftRight >= -2.25)) // red collision
		{
			collisionRed = 1;
			fall = 2;
		}

	}

	if(collisionYellow == 1 && 36.6 <= cubeMovement)
	{
		score += 1000;
		firstCollisionCount = 100 * 1/speed;
		collisioncount = firstCollisionCount;
		collisionYellow = 0;
	}
	if (collisioncount>0)
	{
		angle += 360/firstCollisionCount;
		collisioncount--;
	}
	else{
		angle = 0.f;
	}

	if(collisionRed == 1){
		cubeIncrement = 0;
		quadIncrement = 0;
		increment = 0;
		rabbit = 0;
		angle_fall = -90;
	}

	if(keyAPress == 1)
	{
		rabbitLeftRightIncrement = 0.3 * speedRabbit;
		if(collisionRed != 1){
			rabbitLeftRight += rabbitLeftRightIncrement ;
		}
		if(rabbitLeftRight > 9)
		{
			rabbitLeftRight = 9;
		}
	}
	if(keyARelease == 1)
	{
		keyAPress = 0;
		keyARelease = 0;
	}
	if(keyDPress == 1)
	{
		rabbitLeftRightIncrement = -0.3 * speedRabbit; 
		if(collisionRed != 1){
			rabbitLeftRight += rabbitLeftRightIncrement;
		}
		if(rabbitLeftRight < -9)
		{
			rabbitLeftRight = -9;
		}
	}
	if(keyDRelease == 1)
	{
		keyDPress = 0;
		keyDRelease = 0;
	}

	glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -3.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.3, 0.3, 0.3));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 matT2 = glm::translate(glm::mat4(1.0), glm::vec3(0.f , -4.0f  + rabbit, 0.f + rabbitLeftRight));
	glm::mat4 matR2 = glm::rotate<float>(glm::mat4(1.0), (angle / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 matR3 = glm::rotate<float>(glm::mat4(1.0), (angle_fall / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	modelingMatrix = matT * matS * matR * matT2 * matR2 * matR3;

	glUseProgram(gProgram[activeProgramIndex]);
	glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));

	drawBunny();

	activeProgramIndex = 1; // cube drawing
	// yellow cube
	matS = glm::scale(glm::mat4(1.0), glm::vec3(0.4f, 1.f, 0.4f));
	matT = glm::translate(glm::mat4(1.0), glm::vec3(cubearr[randed], -1.f, -40.f + cubeMovement));
	modelingMatrix = matT * matS;
	glUseProgram(gProgram[activeProgramIndex]);
	GLint myUniformLocation = glGetUniformLocation(gProgram[activeProgramIndex], "yellowCube");
	glUniform1i(myUniformLocation, 1);
	glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
	if(yellowCubeRemove != 1)
	{
		drawCube();
	}
	matS = glm::scale(glm::mat4(1.0), glm::vec3(0.4f, 1.f, 0.4f));
	matT = glm::translate(glm::mat4(1.0), glm::vec3(cubearr[index1], -1.f, -40.f + cubeMovement));
	modelingMatrix = matT * matS;
	glUseProgram(gProgram[activeProgramIndex]);
	glUniform1i(myUniformLocation, 0);
	glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
	if(fall != index1 + 1)
	{
		drawCube();
	}
	matS = glm::scale(glm::mat4(1.0), glm::vec3(0.4f, 1.f, 0.4f));
	matT = glm::translate(glm::mat4(1.0), glm::vec3(cubearr[index2], -1.f, -40.f + cubeMovement));
	modelingMatrix = matT * matS;
	glUseProgram(gProgram[activeProgramIndex]);
	glUniform1i(myUniformLocation, 0);
	glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
	if(fall != index2 + 1)
	{
		drawCube();
	}

	cubeMovement += cubeIncrement * speed;
	// quad Drawing
	matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -3.f));
	matR = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	activeProgramIndex = 1;
	for (int j = 0; j < 24; j++){
		for(int i = 0; i < 4; i++)
		{
			if((i+j) %2 == 0)
			{
				glUniform1i(myUniformLocation, 3);
			}
			if((i+j) %2 == 1)
			{
				glUniform1i(myUniformLocation, 4);
			}
			matT2 = glm::translate(glm::mat4(1.0), glm::vec3(3-i*2 , 2*j - quadMovement , -2.f));
			modelingMatrix = matT  * matR * matT2;
	
			glUseProgram(gProgram[activeProgramIndex]);
			glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
			glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
			glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
			drawQuad();
		}
	}
	scoreText = to_string(int(score));
	if(collisionRed == 1){
		renderText("Score: " + scoreText, 0,765, 1, glm::vec3(1, 0, 0));
	}
	else{
		renderText("Score: " + scoreText, 0,765, 1, glm::vec3(1, 1, 0));
	}
}
void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;
	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float)h, 0.1f, 200.0f);
	viewingMatrix = glm::lookAt(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

}
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		keyAPress = 1;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		keyARelease = 1;
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		keyDPress = 1;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		keyDRelease = 1;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		restart = 1;
	}
	else if (key == GLFW_KEY_R && action == GLFW_RELEASE)
	{
		restart = 0;
	}
}

void mainLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char** argv)
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1000, height = 800;
	window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
	strcat(rendererInfo, " - ");
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION));
	glfwSetWindowTitle(window, rendererInfo);
	
	initSkyVBO();
	initBunny();
	initCube();
	initQuad();
    initFonts(width, height);

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height);
	mainLoop(window);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
