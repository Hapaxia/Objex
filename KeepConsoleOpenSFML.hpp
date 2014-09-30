#ifndef DEV_KEEPCONSOLEOPENSFML_HPP
#define DEV_KEEPCONSOLEOPENSFML_HPP

// http:://github.com/hapaxia

#include <iostream>
#include <SFML\Window\Keyboard.hpp>

namespace DEV
{

//#define ENTER_KEY_TO_CLOSE_CONSOLE
class KeepConsoleOpenSFML
{
public:
	~KeepConsoleOpenSFML()
	{
		if (!m_allowToClose)
		{
			// make sure that a key isn't still being pressed
			while (isAnyKeyPressed())
			{ }

#ifdef ENTER_KEY_TO_CLOSE_CONSOLE
			// enter to close
			std::cout << std::endl << "Press Enter to close the console." << std::endl;
			while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{ }
#else
			// any key to close
			std::cout << std::endl << "Press any key to close the console." << std::endl;
			while (!isAnyKeyPressed())
			{ }
#endif // ENTER_KEY_TO_CLOSE_CONSOLE
		}
	}

	void allowToClose()
	{
		m_allowToClose = true;
	}

private:
	bool m_allowToClose = false;

	bool isAnyKeyPressed()
	{
		for (int k = -1; k < sf::Keyboard::KeyCount; ++k)
		{
			if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(k)))
				return true;
		}
		return false;
	}
};

} // namespace DEV

#endif // DEV_KEEPCONSOLEOPENSFML_HPP