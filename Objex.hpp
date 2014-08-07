#ifndef INC_OBJEX_HPP
#define INC_OBJEX_HPP

/**********************************
**                               **
**             OBJEX             **
**             v0.1              **
**                               **
** by Hapax                      **
** github.com/hapaxia            **
**                               **
**  2014 Golden Eagle Software   **
**                               **
***********************************

--------------------------------------------------------------------------------------------------------------------------------------------
Imports .OBJ files, stores and allows manipulation of the data, and generates vertex arrays for OpenGL (updatable from the manipulated data)
--------------------------------------------------------------------------------------------------------------------------------------------



CURRENTLY IMPORTS:

[x] geometry vertices (faces with multiple triangles - all triangles use the face's first vertex and the previous triangle's last vertex)
[x] normal vertices (fully imported but untested with lighting)
[x] texture vertices (fully imported but untested with any materials - these aren't imported yet)

[ ] .MTL file
[ ] files referenced inside .MTL file

[ ] object groups
[ ] shading groups
[ ] materials
[ ] texture images


DOES NOT IMPORT:

 X  POINTS/LINES (imports actual faces only)
 X  CURVES
 X  NURBS
 X  ANY FREE-FORM GEOMETRY


NOTES:
 - does not (currently) support relative vertex indexing e.g. "f -4 -3 -2 -1"
 - manipulation of data requires recreation of the entire object and is therefore very slow and not intended for use with animation
 - outputs using "clog" (for information) and "cerr" (for errors)
 - there are a number of possible errors that can occur if used incorrectly and they are not prepared for
 - the colour creation is not strictly required and may be removed or bypassed






/************************/

#include <SFML/OpenGL.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h> // for filesize (for loading progress)

#include <iostream>


class Objex
{
public:
	GLfloat* vertexData{ nullptr };
	GLfloat* normalData{ nullptr };
	GLfloat* textureData{ nullptr };
	GLfloat* colorData{ nullptr };

	struct Vertex
	{
		float x, y, z;
	};
	struct Face
	{
		std::vector<int> vertexIndices;
		std::vector<int> normalIndices;
		std::vector<int> textureIndices;
	};
	struct Box
	{
		float left{ 0.f }, right{ 0.f }, top{ 0.f }, bottom{ 0.f }, front{ 0.f }, back{ 0.f }, width{ 0.f }, height{ 0.f }, depth{ 0.f };
	};

	Objex();
	~Objex();

	void refreshData();
	bool loadFromFile(const std::string& filename);
	void scale(float scale);

	void setVertex(unsigned int vertexNumber, Vertex& vertexData);
	void setVertexNormal(unsigned int vertexNormalNumber, Vertex& vertexNormalData);
	void setTextureVertex(unsigned int textureVertexNumber, Vertex& textureVertexData);
	void setFace(unsigned int faceNumber, Face& faceData);

	Vertex getVertex(unsigned int vertexNumber);
	Vertex getVertexNormal(unsigned int vertexNormalNumber);
	Vertex getTextureVertex(unsigned int textureVertexNumber);
	Face getFace(unsigned int faceNumber);

	std::vector<Vertex> getAllVertices();
	std::vector<Vertex> getAllVertexNormals();
	std::vector<Vertex> getAllTextureVertices();
	std::vector<Face> getAllFaces();

	Vertex getLocalBoundingBoxCenter();
	Box getLocalBoundingBox();
	float getLocalBoundingBoxLeft();
	float getLocalBoundingBoxRight();
	float getLocalBoundingBoxTop();
	float getLocalBoundingBoxBottom();
	float getLocalBoundingBoxFront();
	float getLocalBoundingBoxBack();
	float getLocalBoundingBoxWidth();
	float getLocalBoundingBoxHeight();
	float getLocalBoundingBoxDepth();

	unsigned int getNumberOfVertices();
	unsigned int getNumberOfTriangles();

	void randomizeColorData();

	// retrieve some of the "lines" from .obj file
	std::vector<std::string> getCommentLines();
	std::vector<std::string> getUnprocessedLines();

private:

	std::vector<GLfloat> mVertexArray; // stride is 3 i.e. 3 coordinates per vertex (no other information is interleaved e.g. colour/texture, normal)
	std::vector<GLfloat> mNormalArray; // stride is 3 i.e. 3 coordinates per vertex normal (no other information is interleaved e.g. colour/texture, geometry)
	std::vector<GLfloat> mTextureArray; // stride is 3 i.e. 3 coordinates per vertex normal (no other information is interleaved e.g. geometry, normal, colour)
	std::vector<GLfloat> mColorArray; // random colours for testing - easier to see (when no shading is applied)

	std::vector<Vertex> mVertices;
	std::vector<Vertex> mNormals;
	std::vector<Vertex> mTextures;
	std::vector<Face> mFaces;

	Box mLocalBoundingBox;

	std::vector<std::string> mCommentLines;
	std::vector<std::string> mUnprocessedLines;

	std::string trimWhitespaceLeft(std::string string);
	std::string trimWhitespaceRight(std::string string);
	std::string trimWhitespace(const std::string& string);
	unsigned int getTokens(const std::string& line, std::vector<std::string>& tokens);
	void addTokensToVertexVector(const std::vector<std::string>& tokens, std::vector<Vertex>& vertices);

	void transferFaceToArray(const std::vector<int>& indices, const std::vector<Vertex>& vertices, std::vector<GLfloat>& destinationArray, const Vertex& defaultVertex = { 0.f, 0.f, 0.f });
	void refreshLocalBoundingBox();
	bool parse(const std::vector<std::string>& lines);

	void createColorArray();
};

#endif // INC_OBJEX_HPP