#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Objex.hpp"

#include "KeepConsoleOpenSFML.hpp"
#include <iostream> // for errors/log

int main()
{
	DEV::KeepConsoleOpenSFML keepConsoleOpen;

	Objex objex;
	if (!objex.loadFromFile("cow-nonormals.obj")) // .OBJ file
	{
		std::cerr << "Failed to load obj." << std::endl;
		return EXIT_FAILURE;
	}
	objex.scale(25.f);
	std::clog << "Number of triangles: " << objex.getNumberOfTriangles() << std::endl;

	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 32;
	contextSettings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, contextSettings);

	sf::Font font;
	if (!font.loadFromFile("G:/Resource Pool/fonts/mriamc.ttf")) // font file
	{
		std::cerr << "Failed to load font." << std::endl;
		return EXIT_FAILURE;
	}
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(12);
	text.setColor(sf::Color::Green);
	text.setPosition(5, 5);
	text.setStyle(sf::Text::Style::Regular);
	text.setString("");

	std::string textString{ "Nothing to display." };

	if (objex.getUnprocessedLines().size() >= 1)
	{
		textString = "UNPROCESSED LINES:\n";
		unsigned int lineNumber{ 0 };
		for (auto line : objex.getUnprocessedLines())
		{
			++lineNumber;
			textString += std::to_string(lineNumber) + ":" + line + "\n";
		}
	}

	if (objex.getCommentLines().size() >= 1)
	{
		textString = "COMMENT LINES:\n";
		unsigned int lineNumber{ 0 };
		for (auto line : objex.getCommentLines())
		{
			++lineNumber;
			textString += std::to_string(lineNumber) + ":" + line + "\n";
		}
	}

	textString = "Local bounding box:\nx: [" + std::to_string(objex.getLocalBoundingBox().left) + " , " + std::to_string(objex.getLocalBoundingBox().right) + "]\n" +
		"y: [" + std::to_string(objex.getLocalBoundingBox().bottom) + " , " + std::to_string(objex.getLocalBoundingBox().top) + "]\n" +
		"z: [" + std::to_string(objex.getLocalBoundingBox().back) + " , " + std::to_string(objex.getLocalBoundingBox().front) + "]\n" +
		"Local bounding box sizes:\nwidth: " + std::to_string(objex.getLocalBoundingBox().width) + "\n" +
		"height: " + std::to_string(objex.getLocalBoundingBox().height) + "\n" +
		"depth: " + std::to_string(objex.getLocalBoundingBox().depth) + "\n" +
		"Local bounding box centre: (" + std::to_string(objex.getLocalBoundingBoxCenter().x) + ", " + std::to_string(objex.getLocalBoundingBoxCenter().y) + ", " + std::to_string(objex.getLocalBoundingBoxCenter().z) + ")"
		;

	text.setString(textString);


	// OPEN GL

	window.setActive();
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glViewport(0, 0, window.getSize().x, window.getSize().y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	sf::Clock clock;
	sf::Time currentTime = clock.getElapsedTime();
	sf::Time previousTime = currentTime;

	bool appRunning = true;
	while (appRunning)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
				appRunning = false;
			else if (event.type == sf::Event::Resized)
				glViewport(0, 0, event.size.width, event.size.height);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			srand(0);
			for (unsigned int vertexNumber{ 0 }; vertexNumber < objex.getAllVertices().size() - 1; ++vertexNumber)
			{
				Objex::Vertex v;
				v = objex.getVertex(vertexNumber);
				v.x += rand() % 10;
				v.y += rand() % 10;
				v.z += rand() % 10;
				objex.setVertex(vertexNumber, v);
			}
			objex.refreshData();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			srand(0);
			for (unsigned int vertexNumber{ 0 }; vertexNumber < objex.getAllVertices().size() - 1; ++vertexNumber)
			{
				Objex::Vertex v;
				v = objex.getVertex(vertexNumber);
				v.x -= rand() % 10;
				v.y -= rand() % 10;
				v.z -= rand() % 10;
				objex.setVertex(vertexNumber, v);
			}
			objex.refreshData();
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), objex.vertexData);
		glNormalPointer(GL_FLOAT, 3 * sizeof(GLfloat), objex.normalData);
		glTexCoordPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), objex.textureData);
		glColorPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), objex.colorData);
		glLoadIdentity();

		glTranslatef(0.f, 0.f, -200.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 50, 1.f, 0.f, 0.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 30, 0.f, 1.f, 0.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 90, 0.f, 0.f, 1.f);

		glDrawArrays(GL_TRIANGLES, 0, objex.getNumberOfVertices());




		window.pushGLStates();
		currentTime = clock.getElapsedTime();


		//window.clear(sf::Color::White);

		window.draw(text);

		window.popGLStates();

		window.display();
	}
	window.close();

	keepConsoleOpen.allowToClose();
	return EXIT_SUCCESS;
}