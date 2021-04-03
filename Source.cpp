#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <fstream>


#define Liczba_wyborow 2

int windowwidth = 1024;
int windowheight = 768;

using namespace sf;
using namespace std;

class Menu
{

private:
	Font FontMenu;
	Text menu[Liczba_wyborow];//maksymalna liczba poziomow
	int selectedItem = 0;


public:
	Menu(float width, float height);
	~Menu() {};
	void moveUp();//przesun do gory
	void moveDown();//przesun w dol
	int getSelectedItem() { return selectedItem; }//zwroc poziom menu
	void draw(sf::RenderWindow& window);//rysuj menu w oknie
};

Menu::Menu(float width, float height)
{	//laduj czcionke
	if (!FontMenu.loadFromFile("Fonts/Metropolian-Display.ttf"))
	{
		return;
	}
	//rysowanie elementow menu
	menu[0].setFont(FontMenu);
	menu[0].setFillColor(sf::Color::Cyan);
	menu[0].setString("Nowa gra");
	menu[0].setPosition(sf::Vector2f(width / 3, height / (Liczba_wyborow + 1) * 1));
	menu[1].setFont(FontMenu);
	menu[1].setFillColor(sf::Color::White);
	menu[1].setString("Wyjscie");
	menu[1].setPosition(sf::Vector2f(width / 3, height / (Liczba_wyborow + 1) * 2));

}

void Menu::draw(sf::RenderWindow& window)
{
	for (int i = 0; i < Liczba_wyborow; i++)
	{
		window.draw(menu[i]);
	}
}

void Menu::moveUp()
{
	if (selectedItem >= 0 && selectedItem < Liczba_wyborow)
	{
		menu[selectedItem].setFillColor(sf::Color::White);
		menu[selectedItem].setStyle(sf::Text::Regular);
		selectedItem--;
		if (selectedItem < 0)
			selectedItem = Liczba_wyborow - 1;
		menu[selectedItem].setFillColor(sf::Color::Cyan);
		menu[selectedItem].setStyle(sf::Text::Bold);
	}


}

void Menu::moveDown()
{
	if (selectedItem >= 0 && selectedItem < Liczba_wyborow)
	{
		menu[selectedItem].setFillColor(sf::Color::White);
		menu[selectedItem].setStyle(sf::Text::Regular);
		selectedItem++;
		if (selectedItem >= Liczba_wyborow)
			selectedItem = 0;
		menu[selectedItem].setFillColor(sf::Color::Cyan);
		menu[selectedItem].setStyle(sf::Text::Bold);
	}

}

void myDelay(int opoznienie)
{
	sf::Clock zegar;
	sf::Time czas;
	while (1)
	{
		czas = zegar.getElapsedTime();
		if (czas.asMilliseconds() > opoznienie)
		{
			zegar.restart();
			break;
		}

	}
}

class pocisk
{
public:
	CircleShape shape;
	Vector2f currVelocity;
	float maxSpeed;

	pocisk(float radius = 2.f)
		: currVelocity(0.f, 0.f), maxSpeed(20.f)
	{
		this->shape.setRadius(radius);
		this->shape.setFillColor(Color::Red);
	}
};

class playerchar
{

};

class Enemy
{
public:
	Sprite shape;
	int eHP;
	int eMaxHP;
	int pozycja = rand() % 4;

	Enemy(Texture* texture, Vector2u windowSize)
		{
			this->eMaxHP = rand() % 3 + 1;
			this->eHP = this->eMaxHP;
	
			this->shape.setTexture(*texture);
			this->shape.setScale(0.1f, 0.1f);
			//this->shape.setPosition(Vector2f(rand() % windowSize.x, rand() % windowSize.y));
			
			//int pozycja = rand() % 4;
			switch (pozycja)
			{
			case 0://resp z gory
				this->shape.setPosition(rand() % windowSize.x, 0);
				break;
			case 1://resp z prawej
				this->shape.setPosition(windowSize.x, rand() % windowSize.y);
				break;
			case 2://resp z dolu
				this->shape.setPosition(rand() % windowSize.x, windowSize.y);
				break;
			case 3://resp z lewej
				this->shape.setPosition(0, rand() % windowSize.y);
				break;
			default:
				break;
			}

			
		}
	~Enemy(){}
};


int main()
{
	sf::Clock ClockT;

	int menu_selected_flag = 0;
	bool game_paused = 0;
	srand(time(NULL));
	int Time_key = 10;
	RenderWindow window(VideoMode(windowwidth, windowheight), "Square");
	Menu menu(window.getSize().x, window.getSize().y);
	Menu menu2(window.getSize().x, window.getSize().y);
	window.setFramerateLimit(60);
	pocisk b1;
	vector<pocisk> bullets;

	//Inicjalizacja tekstur
	Texture background;
	background.loadFromFile("Textures/kosmos.jpg");
	Sprite Background(background, IntRect(0, 0, 3840, 2400));
	Background.setOrigin(3840 / 2, 2400 / 2);
	Background.setPosition(windowwidth / 2, windowheight / 2);
	Texture enemyTex;
	enemyTex.loadFromFile("Textures/enemy.png");
	Texture gamePause;
	gamePause.loadFromFile("Textures/pause.jpg");
	Sprite Pauza(gamePause, IntRect(0, 0, 1024, 768));
	Pauza.setColor(sf::Color(255, 255, 255, 128));


	//inicjalizacja fonta
	Font font;
	font.loadFromFile("Fonts/Metropolian-Display.ttf");

	//Inicjalizacja gracza
	RectangleShape player;
	player.setSize(Vector2f(50, 50));
	player.setFillColor(Color::Green);
	player.setOrigin(25.f, 25.f);
	player.setPosition(windowwidth / 2, windowheight / 2);
	int shootTimer = 20;
	int score = 0;
	int HP;
	int MaxHP = 5;
	int mnoznik = 1;
	int _highScore;
	HP = MaxHP;
	Text HPText;
	HPText.setFont(font);
	HPText.setCharacterSize(15);
	HPText.setFillColor(Color::White);

	//Inicjalizacja przeciwnika
	Text eHPText;
	eHPText.setFont(font);
	eHPText.setCharacterSize(12);
	eHPText.setFillColor(Color::White);
	std::vector<Enemy> enemies;
	float SpawnCounter = 0.f;

	int spawnCounter = 20;
	Vector2f playerCenter;
	Vector2f mousePosWindow;
	Vector2f aimDir;
	Vector2f aimDirNorm;

	//Inicjalizacja UI
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(10.f, 10.f);

	Text HSText;
	HSText.setFont(font);
	HSText.setCharacterSize(35);
	HSText.setFillColor(Color::White);
	HSText.setPosition((window.getSize().x / 2)-150, (window.getSize().y / 2)+100);

	Text YscoreText;
	YscoreText.setFont(font);
	YscoreText.setCharacterSize(35);
	YscoreText.setFillColor(Color::White);
	YscoreText.setPosition((window.getSize().x / 2) - 150, (window.getSize().y / 2));

	//Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(Color::Red);
	gameOverText.setPosition((window.getSize().x / 2) - 200, (window.getSize().y / 2)-100);
	gameOverText.setString("GAME OVER!");

	bool gameplay = 0;


	while (window.isOpen())
	{

		Event event;
		if (Keyboard::isKeyPressed(Keyboard::F1))
		{
			game_paused = !game_paused;
			sf::sleep(sf::milliseconds(200));
		}
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Up)
				{
					myDelay(100);
					menu.moveUp();
				}

				if (event.key.code == sf::Keyboard::Down)
				{
					myDelay(100);
					menu.moveDown();
				}
				if (menu_selected_flag == 0)
				{

					if (event.key.code == sf::Keyboard::Enter && menu.getSelectedItem() == 0)
					{
						gameplay = 1;
						menu_selected_flag = 1;
						ClockT.restart();
					}
					
					if (event.key.code == sf::Keyboard::Enter && menu.getSelectedItem() == 1)
					{
						window.close();
					}
				}
			}
		}
				if (gameplay == 1)
				{
				
					if (!game_paused)
					{
						sf::Time CzasT = ClockT.getElapsedTime();
						if (CzasT.asMilliseconds()>10000)
						{
							mnoznik=mnoznik+1;
							ClockT.restart();
						}
						playerCenter = Vector2f(player.getPosition());
						mousePosWindow = Vector2f(Mouse::getPosition(window));
						aimDir = mousePosWindow - playerCenter;
						aimDirNorm = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

						float PI = 3.14159265f;
						float deg = atan2(aimDirNorm.y, aimDirNorm.x) * 180 / PI;
						player.setRotation(deg + 90);

						//Rotacja myszki dla sprawdzenia
					//	cout << deg << "\n";

						//Player
						if (HP > 0)
						{
							Background.rotate(0.1f);

							if (Keyboard::isKeyPressed(Keyboard::A))
								player.move(-10.f, 0.f);
							if (Keyboard::isKeyPressed(Keyboard::D))
								player.move(10.f, 0.f);
							if (Keyboard::isKeyPressed(Keyboard::W))
								player.move(0.f, -10.f);
							if (Keyboard::isKeyPressed(Keyboard::S))
								player.move(0.f, 10.f);

							HPText.setPosition(player.getPosition().x, player.getPosition().y - HPText.getGlobalBounds().height);
							HPText.setString(std::to_string(HP) + "/" + std::to_string(MaxHP));

							if (player.getPosition().x <= 0) //Left
								player.setPosition(0.f, player.getPosition().y);
							if (player.getPosition().x >= window.getSize().x) //Right
								player.setPosition(window.getSize().x, player.getPosition().y);
							if (player.getPosition().y <= 0) //Top
								player.setPosition(player.getPosition().x, 0.f);
							if (player.getPosition().y >= window.getSize().y) //Bottom
								player.setPosition(player.getPosition().x, window.getSize().y);

							//Przeciwnicy
							if (spawnCounter < 20)
								spawnCounter++;

							if (spawnCounter >= 20)
							{
								enemies.push_back(Enemy(&enemyTex, window.getSize()));
								spawnCounter = 0;
							}
							if (shootTimer < 10)
								shootTimer++;
							//Strzelanie
							if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 10)
							{
								b1.shape.setPosition(playerCenter);
								b1.currVelocity = aimDirNorm * b1.maxSpeed;

								bullets.push_back(pocisk(b1));
								shootTimer = 0;
							}

							for (size_t i = 0; i < bullets.size(); i++)
							{
								bullets[i].shape.move(bullets[i].currVelocity);

								//Poza oknem
								if (bullets[i].shape.getPosition().x < 0 || bullets[i].shape.getPosition().x > window.getSize().x
									|| bullets[i].shape.getPosition().y < 0 || bullets[i].shape.getPosition().y > window.getSize().y)
								{
									bullets.erase(bullets.begin() + i);
								}
								else
								{
									//Trafienie pociskiem
									for (size_t k = 0; k < enemies.size(); k++)
									{
										if (bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
										{
											if (enemies[k].eHP <= 1)
											{
												cout << mnoznik << endl;
												score = score+(enemies[k].eMaxHP)*mnoznik;
												enemies.erase(enemies.begin() + k);
											}
											else
											{
												enemies[k].eHP--;
											}
											bullets.erase(bullets.begin() + i);
											break;
										}
									}
								}

							}
							for (size_t i = 0; i < enemies.size(); i++)
							{
								switch (enemies[i].pozycja)
								{
								case 0://ruch z gory
									enemies[i].shape.move(0, 1*mnoznik);
									break;

								case 1://ruch z prawej
									enemies[i].shape.move(-1 * mnoznik, 0);
									break;

								case 2://ruch z dolu
									enemies[i].shape.move(0, -1 * mnoznik);
									break;

								case 3://ruch z lewej
									enemies[i].shape.move(1 * mnoznik, 0);
								default:

									break;
								}


								if (enemies[i].shape.getGlobalBounds().intersects(player.getGlobalBounds()))
								{
									enemies.erase(enemies.begin() + i);

									HP--;
									break;
								}
							}
							scoreText.setString("Score: " + std::to_string(score));
							YscoreText.setString("Your Score: " + std::to_string(score));
							if (HP == 0)
							{
								std::ifstream readFile;
								readFile.open("wyniki.txt");


								if (readFile.is_open())
								{
									while (!readFile.eof())
									{
										readFile >> _highScore;
									}
								}
								readFile.close();

								std::ofstream writeFile("wyniki.txt");

								if (writeFile.is_open())
								{
									if (score > _highScore)
									{
										_highScore = score;
									}

									writeFile << _highScore;
								}
								writeFile.close();
								HSText.setString("High Score: " + std::to_string(_highScore));

							}
							YscoreText.setString("Your Score: " + std::to_string(score));
						} 

						
					//	sf::Time CzasT = ClockT.getElapsedTime();
					}
				
			
		}

			//Draw
			window.clear();
			window.draw(Background);
				for (size_t i = 0; i < enemies.size(); i++)
				{

					eHPText.setString(std::to_string(enemies[i].eHP) + "/" + std::to_string(enemies[i].eMaxHP));
					eHPText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - eHPText.getGlobalBounds().height);
					window.draw(eHPText);
					window.draw(enemies[i].shape);
				}
				window.draw(player);

				for (size_t i = 0; i < bullets.size(); i++)
				{
					window.draw(bullets[i].shape);
				}
				window.draw(scoreText);
				window.draw(HPText);

				if (HP <= 0)
				{
					window.draw(gameOverText);
					window.draw(YscoreText);
					window.draw(HSText);
				}
				if (game_paused)
				{
					window.draw(Pauza);
				}
			
				if (menu_selected_flag == 0)
				{
					window.clear();
					window.draw(Background);
					menu.draw(window);
				}
			window.display();
		}

		return 0;
	}

