#include "Objex.hpp"


Objex::Objex()
{
}


Objex::~Objex()
{
}


void Objex::refreshData()
{
	mVertexArray.resize(0);
	mNormalArray.resize(0);
	mTextureArray.resize(0);
	for (const auto& face : mFaces)
	{
		transferFaceToArray(face.vertexIndices, mVertices, mVertexArray);
		transferFaceToArray(face.normalIndices, mNormals, mNormalArray, Vertex{ 0, 0, 1 });
		transferFaceToArray(face.textureIndices, mTextures, mTextureArray);
	}
	vertexData = &mVertexArray.front();
	normalData = &mNormalArray.front();
	textureData = &mTextureArray.front();

	refreshLocalBoundingBox();
}

bool Objex::loadFromFile(const std::string& filename)
{

	std::ifstream file(filename, std::ifstream::in);
	if (!file.is_open())
		return false;

	struct stat filestatus;
	stat(filename.c_str(), &filestatus);
	std::string fileLine;
	float previousProgress{ 0.f };

	// cache file
	std::vector<std::string> mRawImportLines; // all lines directly from the file (get deleted when parsed)
	while (getline(file, fileLine))
	{
		mRawImportLines.push_back(fileLine);

		// show percentage of progress. only updates if percentage has changed (whole numbers only)
		float progress{ round(static_cast<float>(file.tellg()) * 100 / filestatus.st_size) };
		if (progress != previousProgress)
			std::clog << "Loading " << filename << " " << progress << "%" << std::endl;
		previousProgress = progress;
	}
	file.close();

	std::clog << "Importing .obj:" << std::endl;
	if (!parse(mRawImportLines))
	{
		std::cerr << "Importing failed!" << std::endl;
		return false;
	}
	else
	{
		std::clog << "Importing completed!" << std::endl;
		return true;
	}
}

Objex::Vertex Objex::getLocalBoundingBoxCenter()
{
	return{ mLocalBoundingBox.width / 2 + mLocalBoundingBox.left, mLocalBoundingBox.height / 2 + mLocalBoundingBox.bottom, mLocalBoundingBox.depth / 2 + mLocalBoundingBox.back };
}

Objex::Box Objex::getLocalBoundingBox()
{
	return mLocalBoundingBox;
}

float Objex::getLocalBoundingBoxLeft()
{
	return mLocalBoundingBox.left;
}

float Objex::getLocalBoundingBoxRight()
{
	return mLocalBoundingBox.right;
}

float Objex::getLocalBoundingBoxTop()
{
	return mLocalBoundingBox.top;
}

float Objex::getLocalBoundingBoxBottom()
{
	return mLocalBoundingBox.bottom;
}

float Objex::getLocalBoundingBoxFront()
{
	return mLocalBoundingBox.front;
}

float Objex::getLocalBoundingBoxBack()
{
	return mLocalBoundingBox.back;
}

float Objex::getLocalBoundingBoxWidth()
{
	return mLocalBoundingBox.width;
}

float Objex::getLocalBoundingBoxHeight()
{
	return mLocalBoundingBox.height;
}

float Objex::getLocalBoundingBoxDepth()
{
	return mLocalBoundingBox.depth;
}

unsigned int Objex::getNumberOfVertices()
{
	return mVertexArray.size() / 3;
}

unsigned int Objex::getNumberOfTriangles()
{
	return getNumberOfVertices() / 3;
}

void Objex::scale(float scale)
{
	for (auto& vertex : mVertices)
	{
		vertex.x *= scale;
		vertex.y *= scale;
		vertex.z *= scale;
	}
	refreshData();
}

void Objex::setVertex(unsigned int vertexNumber, Vertex& vertexData)
{
	mVertices[vertexNumber] = vertexData;
	refreshLocalBoundingBox();
}

void Objex::setVertexNormal(unsigned int vertexNormalNumber, Vertex& vertexNormalData)
{
	mNormals[vertexNormalNumber] = vertexNormalData;
}

void Objex::setTextureVertex(unsigned int textureVertexNumber, Vertex& textureVertexData)
{
	mTextures[textureVertexNumber] = textureVertexData;
}

void Objex::setFace(unsigned int faceNumber, Face& faceData)
{
	mFaces[faceNumber] = faceData;
	refreshLocalBoundingBox();
}

Objex::Vertex Objex::getVertex(unsigned int vertexNumber)
{
	return mVertices[vertexNumber];
}

Objex::Vertex Objex::getVertexNormal(unsigned int vertexNormalNumber)
{
	return mNormals[vertexNormalNumber];
}

Objex::Vertex Objex::getTextureVertex(unsigned int textureVertexNumber)
{
	return mTextures[textureVertexNumber];
}

Objex::Face Objex::getFace(unsigned int faceNumber)
{
	return mFaces[faceNumber];
}

std::vector<Objex::Vertex> Objex::getAllVertices()
{
	return mVertices;
}

std::vector<Objex::Vertex> Objex::getAllVertexNormals()
{
	return mNormals;
}

std::vector<Objex::Vertex> Objex::getAllTextureVertices()
{
	return mTextures;
}

std::vector<Objex::Face> Objex::getAllFaces()
{
	return mFaces;
}

void Objex::randomizeColorData()
{
	unsigned int colorElementNumber{ 0 };
	for (auto& colorElement : mColorArray)
	{
		++colorElementNumber;
		if (colorElementNumber < 4)
			colorElement = static_cast<float>(rand() % 1000) / 1000;
		else
		{
			colorElementNumber = 0;
			colorElement = 1.f;
		}
	}
}

std::vector<std::string> Objex::getCommentLines()
{
	return mCommentLines;
}

std::vector<std::string> Objex::getUnprocessedLines()
{
	return mUnprocessedLines;
}








// PRIVATE

std::string Objex::trimWhitespaceLeft(std::string s)
{
	const std::string delimiters{ " \f\n\r\t\v" };
	s.erase(0, s.find_first_not_of(" "));
	return s;
}

std::string Objex::trimWhitespaceRight(std::string s)
{
	const std::string delimiters{ " \f\n\r\t\v" };
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

std::string Objex::trimWhitespace(const std::string& s)
{
	return trimWhitespaceLeft(trimWhitespaceRight(s));
}

unsigned int Objex::getTokens(const std::string& line, std::vector<std::string>& tokens)
{
	tokens.resize(0);
	std::string token;
	std::istringstream ss(line);
	while (getline(ss, token, ' '))
		tokens.push_back(token);
	return tokens.size();
}

void Objex::addTokensToVertexVector(const std::vector<std::string>& tokens, std::vector<Vertex>& vertices)
{
	vertices.push_back({ std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]) });
}

bool Objex::parse(const std::vector<std::string>& lines)
{
	mUnprocessedLines.resize(0);
	mCommentLines.resize(0);

	std::clog << "parsing lines" << std::endl;
	// parse lines
	const unsigned int numberOfLines = lines.size();
	unsigned int currentLine{ 0 };
	float previousProgress{ 0.f };
	for (auto line : lines)
	{
		line = trimWhitespace(line);
		if (line.length() > 1)
		{
			const std::string singleCharCommandParameters{ trimWhitespace(line.substr(1)) };
			const std::string doubleCharCommandParameters{ trimWhitespace(line.substr(2)) };
			std::vector<std::string> tokens;
			if (line.substr(0, 2) == "v ")
			{
				if (getTokens(singleCharCommandParameters, tokens) == 3)
					addTokensToVertexVector(tokens, mVertices);
			}
			else if (line.substr(0, 3) == "vn ")
			{
				if (getTokens(doubleCharCommandParameters, tokens) == 3)
					addTokensToVertexVector(tokens, mNormals);
			}
			else if (line.substr(0, 3) == "vt ")
			{
				getTokens(doubleCharCommandParameters, tokens);
				if ((tokens.size() >= 1) && (tokens.size() <= 3))
				{
					tokens.resize(3, "0");
					addTokensToVertexVector(tokens, mTextures);
				}
			}
			else if (line.substr(0, 2) == "f ")
			{
				Objex::Face face;
				std::istringstream ss(line);
				std::vector<std::string> tokens;
				getTokens(singleCharCommandParameters, tokens);
				if ((tokens.size() >= 3) && (tokens.size() <= 4))
				{
					for (auto t : tokens)
					{
						std::string index;
						std::vector<std::string> indices;
						std::istringstream ts(t);
						while (getline(ts, index, '/')) // split token into 3 values using slash as delimiter. when the index is not present, an empty one may need to be created when stored (when importer is expanded to include other index types)
							indices.push_back(index);
						if (indices.size() >= 1) // first index in the token is the vertex index
						{
							face.vertexIndices.push_back({ std::stoi(indices[0]) - 1 }); // "- 1" is to convert index from one-based index (in the file) to zero-based index (in an array/vector)
							if (indices.size() >= 2) // texture vertex index is present (second index)
							{
								face.textureIndices.push_back({ std::stoi(indices[2]) - 1 });
								if (indices.size() >= 3) // vertex normal index is present (third index)
									face.normalIndices.push_back({ std::stoi(indices[2]) - 1 });
							}
						}
					}
					if (face.normalIndices.size() != face.vertexIndices.size()) // if normals are not provided for each vertex, set them to the default normal (index of -1)
					{
						// normal index of -1 becomes default normal of (0.0, 0.0, 1.0)
						face.normalIndices.resize(face.vertexIndices.size());
						for (auto& normalIndex : face.normalIndices)
							normalIndex = -1;
					}
					if (face.textureIndices.size() != face.vertexIndices.size()) // if texture vertices are not provided for each vertex, set them to the default vertex (index of -1)
					{
						// texture vertex index of -1 becomes default texture index of (0.0, 0.0, 0.0)
						face.textureIndices.resize(face.vertexIndices.size());
						for (auto& textureIndex : face.textureIndices)
							textureIndex = -1;
					}
				}
				mFaces.push_back(face);

			}
			else if (line.front() == '#')
				mCommentLines.push_back(singleCharCommandParameters);
			else
				mUnprocessedLines.push_back(line);
		}
		else if (line == "#")
			mCommentLines.push_back("");
		else
			mUnprocessedLines.push_back(line);

		// show percentage of progress. only updates if percentage has changed (whole numbers only)
		float progress{ round(static_cast<float>(currentLine)* 100 / numberOfLines) };
		if (progress != previousProgress)
			std::clog << "Parsing " << progress << "%" << std::endl;
		previousProgress = progress;
		++currentLine;
	}

	std::clog << "Lines in file: " << lines.size() << std::endl;
	std::clog << "Comment lines: " << mCommentLines.size() << std::endl;
	std::clog << "Unprocessed lines: " << mUnprocessedLines.size() << std::endl;

	// generate container to store all data in order required by OpenGL
	refreshData();
	createColorArray();

	return true;
}

void Objex::transferFaceToArray(const std::vector<int>& indices, const std::vector<Vertex>& vertices, std::vector<GLfloat>& destinationArray, const Vertex& defaultVertex)
{
	if (indices.size() >= 3)
	{
		unsigned int numberOfTriangles{ 1 };
		if (indices.size() > 3)
			numberOfTriangles = indices.size() - 2;
		for (unsigned int triangle = 0; triangle < numberOfTriangles; ++triangle)
		{
			for (unsigned int v = 0; v < 3; ++v)
			{
				if (vertices.size() > 0)
				{
					if (v == 0)
					{
						destinationArray.push_back(vertices[indices[0]].x);
						destinationArray.push_back(vertices[indices[0]].y);
						destinationArray.push_back(vertices[indices[0]].z);
					}
					else
					{
						destinationArray.push_back(vertices[indices[triangle + v]].x);
						destinationArray.push_back(vertices[indices[triangle + v]].y);
						destinationArray.push_back(vertices[indices[triangle + v]].z);
					}
				}
				else // default vertex
				{
					destinationArray.push_back(defaultVertex.x);
					destinationArray.push_back(defaultVertex.y);
					destinationArray.push_back(defaultVertex.z);
				}
			}
		}
	}

}

void Objex::createColorArray()
{
	mColorArray.resize(0);
	enum ColorCreationType{ RandomPerTriangle, RandomPerVertex, Other };
	const ColorCreationType colorCreation{ RandomPerTriangle };
	if (colorCreation == RandomPerTriangle)
	{
		// create random colour per triangle
		for (unsigned int triangle{ 0 }; triangle < (mVertexArray.size() / 9); ++triangle)
		{
			//const float luminosity{ static_cast<float>(rand() % 500) / 1000 + 0.5f }; // use for random greys
			const Vertex random{ static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f };
			for (unsigned int vertex{ 0 }; vertex < 3; ++vertex)
			{
				mColorArray.push_back(random.x); // red
				mColorArray.push_back(random.y); // green
				mColorArray.push_back(random.z); // blue
				mColorArray.push_back(1); // alpha
			}
		}
	}
	else if (colorCreation == RandomPerVertex)
	{
		// create random colour per vertex
		for (unsigned int triangle{ 0 }; triangle < (mVertexArray.size() / 3); ++triangle)
		{
			const Vertex random{ static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f };
			mColorArray.push_back(random.x); // red
			mColorArray.push_back(random.y); // green
			mColorArray.push_back(random.z); // blue
			mColorArray.push_back(1); // alpha
		}
	}
	else // (colorCreation == Other)
	{
		// set all colours to white
		for (unsigned int triangle{ 0 }; triangle < mVertexArray.size(); ++triangle)
		{
			const Vertex random{ static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f, static_cast<float>(rand() % 500) / 1000 + 0.25f };
			mColorArray.push_back(1); // red
			mColorArray.push_back(1); // green
			mColorArray.push_back(1); // blue
			mColorArray.push_back(1); // alpha
		}
	}
	colorData = &mColorArray.front();
}

void Objex::refreshLocalBoundingBox()
{
	mLocalBoundingBox.left = mVertices[0].x;
	mLocalBoundingBox.right = mVertices[0].x;
	mLocalBoundingBox.top = mVertices[0].y;
	mLocalBoundingBox.bottom = mVertices[0].y;
	mLocalBoundingBox.front = mVertices[0].z;
	mLocalBoundingBox.back = mVertices[0].z;
	for (auto vertex : mVertices)
	{
		if (vertex.x < mLocalBoundingBox.left)
			mLocalBoundingBox.left = vertex.x;
		if (vertex.x > mLocalBoundingBox.right)
			mLocalBoundingBox.right = vertex.x;
		if (vertex.y > mLocalBoundingBox.top)
			mLocalBoundingBox.top = vertex.y;
		if (vertex.y < mLocalBoundingBox.bottom)
			mLocalBoundingBox.bottom = vertex.y;
		if (vertex.z > mLocalBoundingBox.front)
			mLocalBoundingBox.front = vertex.z;
		if (vertex.z < mLocalBoundingBox.back)
			mLocalBoundingBox.back = vertex.z;
	}
	mLocalBoundingBox.width = mLocalBoundingBox.right - mLocalBoundingBox.left;
	mLocalBoundingBox.height = mLocalBoundingBox.top - mLocalBoundingBox.bottom;
	mLocalBoundingBox.depth = mLocalBoundingBox.front - mLocalBoundingBox.back;
}