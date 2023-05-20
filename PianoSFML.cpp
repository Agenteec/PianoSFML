// PianoSFML.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <RtMidi.h>
const int dKeyBoardx = 100;
const int dKeyBoardy = 100;
// Создание объекта RtMidiOut для отправки сообщений MIDI
static RtMidiOut midiOut;
bool keyStates[88] = { false };
bool systain = false;
struct OldSet
{
	sf::Color color;
	int index;
	bool pressed;
	OldSet() :color(sf::Color::Blue),index(-1),pressed(0){}
	OldSet(sf::Color color,int index, bool pressed) :color(color), index(index), pressed(pressed){}
};
void Render(sf::RenderWindow* window);
void QWERTYKeyReleased(sf::RectangleShape* NoteRects[], const sf::Event& event);
int main()
{
	unsigned int numDevices = midiOut.getPortCount();

	// Вывод информации о доступных MIDI-устройствах
	std::cout << "Available MIDI Output Devices:" << std::endl;
	for (unsigned int i = 0; i < numDevices; i++)
		std::cout << "Device " << i << ": " << midiOut.getPortName(i) << std::endl;

	// Открытие последнего доступного MIDI-устройства вывода
	midiOut.openPort(numDevices-1);
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(1920,400),"");
	
	Render(window);
	return 0;
}
unsigned char instrument = 0;
void ChangeInstrument(int dinstrument)
{
	// Создание сообщения Control Change
	unsigned int channel = 0; // Канал MIDI (0-15)
	instrument += dinstrument;
	if (instrument>127&& dinstrument<0)
	{
		instrument = 127;
	}
	if (instrument > 127 && dinstrument > 0)
	{
		instrument = 0;
	}
	std::cout<<"Instrument: " << (int)instrument << std::endl;
	std::vector<unsigned char> message;
	message.push_back(0xC0 | channel); // Байт статуса Program Change
	message.push_back(instrument); // Номер инструмента

	midiOut.sendMessage(&message);
}
void PlayNote(int index)
{
	// Создание сообщения Note On для текущей ноты
	std::vector<unsigned char> message;
	message.push_back(0x90);                 // Статусное сообщение Note On
	message.push_back(index + 21);            // Номер ноты (от 21 до 108)
	message.push_back(0x7F);                 // Сила нажатия клавиши (максимум - 127)
	// Отправка сообщения Note On
	midiOut.sendMessage(&message);
}
void StopNote(int index)
{

	// Создание сообщения Note Off для текущей ноты
	std::vector<unsigned char> message;
	message.push_back(0x80);   // Статусное сообщение Note Off     
	message.push_back(index + 21);// Номер ноты (от 21 до 108)
	message.push_back(0x00);  // Нажатие клавиши с нулевой силой
	// Отправка сообщения Note Off
	midiOut.sendMessage(&message);
}
void KeyPosSetter(sf::RectangleShape* NoteRects[])
{
	int ns = 4; bool nsb = true;
	int whitestep = 0;
	for (int i = 0; i < 88; i++)
	{

		NoteRects[i] = new sf::RectangleShape(sf::Vector2f(32,180));
		if (ns > 6 && nsb){nsb = false;ns = 0;}
		if (ns > 4 && !nsb) { nsb = true; ns = 0; }
		if (ns%2==0)
		{
			NoteRects[i]->setFillColor(sf::Color::White);
			NoteRects[i]->setOutlineColor(sf::Color::Black);
			NoteRects[i]->setOutlineThickness(1);
			bool sk = 0;
			NoteRects[i]->setPosition(sf::Vector2f(whitestep * 32+ dKeyBoardx, dKeyBoardy));
			whitestep++;
		}
		else 
		{
			NoteRects[i]->setSize(sf::Vector2f(16,90));
			NoteRects[i]->setFillColor(sf::Color::Black);
			NoteRects[i]->setOutlineColor(sf::Color::Black);
			NoteRects[i]->setOutlineThickness(1);
			NoteRects[i]->setPosition(sf::Vector2f(NoteRects[i-1]->getPosition().x+25, dKeyBoardy));
			
		}
		ns++;
		
	}
}
void KeyPrinter(sf::RectangleShape* NoteRects[],sf::RenderWindow* window)
{
	int ns = 4; bool nsb = true;
	int whitestep = 0;
	for (int i = 0; i < 88; i++)
	{
		if (ns > 6 && nsb) { nsb = false; ns = 0; }
		if (ns > 4 && !nsb) { nsb = true; ns = 0; }
		if (ns % 2 == 0)
		{
			window->draw(*NoteRects[i]);
		}
		ns++;
	}
	ns = 4;nsb = true;
	for (int i = 0; i < 88; i++)
	{
		if (ns > 6 && nsb) { nsb = false; ns = 0; }
		if (ns > 4 && !nsb) { nsb = true; ns = 0; }
		if (ns % 2 != 0)
		{
			window->draw(*NoteRects[i]);
		}
		ns++;
	}
}
void KeyPressed(sf::RectangleShape* NoteRects[], sf::Event event, OldSet* OLDSET)
{

		int ns = 4; bool nsb = true;
		int whitestep = 0;
		for (int i = 0; i < 88; i++)
		{
			if (ns > 6 && nsb) { nsb = false; ns = 0; }
			if (ns > 4 && !nsb) { nsb = true; ns = 0; }
			if (ns % 2 != 0)
			{
				if (event.mouseButton.button == sf::Mouse::Left && NoteRects[i]->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
				{
					
					*OLDSET =OldSet(NoteRects[i]->getFillColor(), i, true);
					NoteRects[i]->setFillColor(sf::Color::Green);
					PlayNote(i);
					keyStates[i] = true;
					return;
				}
			}
			ns++;
		}
		ns = 4; nsb = true;
		for (int i = 0; i < 88; i++)
		{
			if (ns > 6 && nsb) { nsb = false; ns = 0; }
			if (ns > 4 && !nsb) { nsb = true; ns = 0; }
			if (ns % 2 == 0)
			{
				if (event.mouseButton.button == sf::Mouse::Left && NoteRects[i]->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
				{
					*OLDSET = OldSet(NoteRects[i]->getFillColor(), i, true);
					NoteRects[i]->setFillColor(sf::Color::Green);
					PlayNote(i);
					keyStates[i] = true;
					return;
				}
			}
			ns++;
		}

	
}
void KeyReleased(OldSet* OLDSET, sf::RectangleShape* NoteRects[],const sf::Event& event = sf::Event())
{
		if(OLDSET->index !=-1)
		{
			if (!systain)
			StopNote(OLDSET->index);
			keyStates[OLDSET->index] = false;
			NoteRects[OLDSET->index]->setFillColor(OLDSET->color);
			OLDSET->index = -1;

		}
		
}
int KeySwither(const sf::Event& event)
{
	switch (event.key.code)
	{
	case sf::Keyboard::A:
		return 40;
	case sf::Keyboard::W:
		return 41;
	case sf::Keyboard::S:
		return 42;
	case sf::Keyboard::E:
		return 43;
	case sf::Keyboard::D:
		return 44;
	case sf::Keyboard::F:
		return 45;
	case sf::Keyboard::T:
		return 46;
	case sf::Keyboard::G:
		return 47;
	case sf::Keyboard::Y:
		return 48;
	case sf::Keyboard::H:
		return 49;
	case sf::Keyboard::U:
		return 50;
	case sf::Keyboard::J:
		return 51;
	case sf::Keyboard::K:
		return 52;
	case sf::Keyboard::O:
		return 53;
	case sf::Keyboard::L:
		return 54;
	case sf::Keyboard::P:
		return 55;
	case sf::Keyboard::Semicolon:
		return 56;
	case sf::Keyboard::Quote:
		return 57;
	case sf::Keyboard::RBracket:
		return 58;
	default:
		return -1;
	}
}
void QWERTYKeyPressed(sf::RectangleShape* NoteRects[], const sf::Event& event)
{
	int key = KeySwither(event) - 1;
	if (key >= 0)
	{
		if (!keyStates[key])  // Проверяем, что клавиша не была уже нажата
		{
			keyStates[key] = true;
			PlayNote(key);
			NoteRects[key]->setFillColor(sf::Color::Green);
		}
	}
}

void QWERTYKeyReleased(sf::RectangleShape* NoteRects[], const sf::Event& event)
{
	int key = KeySwither(event) - 1;
	if (key >= 0)
	{
		keyStates[key] = false;
		if (!systain)
		StopNote(key);
		int ns = 4; bool nsb = true;
		int whitestep = 0;
		for (int i = 0; i < 88; i++)
		{
			if (ns > 6 && nsb) { nsb = false; ns = 0; }
			if (ns > 4 && !nsb) { nsb = true; ns = 0; }
			if (i == key)
			{
				if (ns % 2 == 0)
				{
					NoteRects[i]->setFillColor(sf::Color::White); break;
				}
				else
				{
					NoteRects[i]->setFillColor(sf::Color::Black); break;
				}
			}
			ns++;
		}
	}
}

void Render(sf::RenderWindow* window)
{
	sf::RectangleShape* NoteRects[88];
	std::vector<OldSet> oldset;
	OldSet oldsetmouse;
	KeyPosSetter(NoteRects);
	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window->close();
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				KeyPressed(NoteRects,event, &oldsetmouse);
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				KeyReleased(&oldsetmouse, NoteRects);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::F1)
				{
					// Сменить на инструмент 1
					ChangeInstrument(-1);
				}
				else if (event.key.code == sf::Keyboard::F2)
				{
					// Сменить на инструмент 2
					ChangeInstrument(1);
				}
			
				QWERTYKeyPressed(NoteRects, event);
				if (event.key.code == sf::Keyboard::Space)
				{
					systain = 1;
					std::cout << "systain: " << systain << std::endl;
					if (!systain)
					{
						for (size_t i = 0; i < 88; i++)
						{
							if (keyStates[i]==0)
							{
								StopNote(i);
							}
						}
					}
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				QWERTYKeyReleased(NoteRects, event);
				if (event.key.code == sf::Keyboard::Space)
				{
					systain = 0;
					std::cout << "systain: " << systain << std::endl;
					if (!systain)
					{
						for (size_t i = 0; i < 88; i++)
						{
							if (keyStates[i] == 0)
							{
								StopNote(i);
							}
						}
					}
				}
			}

		}
		window->clear(sf::Color(78,99,120));
		KeyPrinter(NoteRects, window);
		window->display();
	}

}

