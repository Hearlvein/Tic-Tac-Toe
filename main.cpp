#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

const float gridSize = 600.f;
const float tileSize = 200.f;
RenderWindow window;

enum Cell
{
	Empty = 0,
	Cross,
	Circle
};
Cell grid[3][3];
int nbFreeCells;

enum Turn
{
	Player,
	Computer,
	NoTurn
};
Turn currentTurn;

Texture backgroundTexture;
Texture tagsTexture;

Sprite background;
Sprite cross;
Sprite circle;

Font font;
int playerScore, computerScore;
Text playerScoreText, computerScoreText;

bool debug = true;

void reset()
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			grid[i][j] = Empty;

	nbFreeCells = 9;
	currentTurn = Player;
}

void debugMsg(const char* msg)
{
	if (debug)
		cout << msg << endl;
}

std::string int2string(int n)
{
	ostringstream stream;
	stream << n;
	return stream.str();
}

void updateScore(Turn actor)
{
	if (actor == Player)
		playerScoreText.setString("Player score: " + int2string(playerScore));
	else
		computerScoreText.setString("Computer score: " + int2string(computerScore));
}

Cell* winningTriple(Cell& a, Cell& b, Cell& c)
{
	if (a == b && a == c && a != Empty)
		return &a;

	return nullptr;
}

Cell* missingCell(Cell& a, Cell& b, Cell& c, Cell pattern)
{
	if 		(a == pattern && b == pattern && c == Empty)	return &c;
	else if (a == pattern && c == pattern && b == Empty)	return &b;
	else if (b == pattern && c == pattern && a == Empty)	return &a;

	return nullptr;
}

void init()
{
	srand(time(0));

	window.create(VideoMode(gridSize + 350, gridSize), "Tic Tac Toe");

	backgroundTexture.loadFromFile("bg.png");
	tagsTexture.loadFromFile("tags.png");

	background.setTexture(backgroundTexture);
	cross.setTexture(tagsTexture);
	cross.setTextureRect(IntRect(0, 0, tileSize, tileSize));
	circle.setTexture(tagsTexture);
	circle.setTextureRect(IntRect(tileSize, 0, tileSize, tileSize));

	font.loadFromFile("lucida.ttf");

	playerScore = computerScore = 0;
	updateScore(Player);
	updateScore(Computer);

	playerScoreText.setFont(font);
	playerScoreText.setCharacterSize(30);
	playerScoreText.setPosition(610.f, 50.f);
	computerScoreText.setFont(font);
	computerScoreText.setCharacterSize(30);
	computerScoreText.setPosition(610.f, 150.f);

	reset();
}

bool gameOver()
{
	Cell* winner = nullptr;

	if		((winner = winningTriple(grid[0][0], grid[1][1], grid[2][2])))	debugMsg("\\ completed");
	else if ((winner = winningTriple(grid[0][2], grid[1][1], grid[2][0])))	debugMsg("/ completed");
	else if ((winner = winningTriple(grid[0][1], grid[1][1], grid[2][1])))	debugMsg("- completed");
	else if ((winner = winningTriple(grid[1][0], grid[1][1], grid[1][2])))	debugMsg(".|. completed");
	else if ((winner = winningTriple(grid[0][0], grid[1][0], grid[2][0])))	debugMsg("^ completed");
	else if ((winner = winningTriple(grid[0][0], grid[0][1], grid[0][2])))	debugMsg("|.. completed");
	else if ((winner = winningTriple(grid[0][2], grid[1][2], grid[2][2])))	debugMsg("_ completed");
	else if ((winner = winningTriple(grid[2][0], grid[2][1], grid[2][2])))	debugMsg("..| completed");

	if (winner)
	{
		if (*winner == Cross)
		{
			playerScore++;
			updateScore(Player);
		}
		else if (*winner == Circle)
		{
			computerScore++;
			updateScore(Computer);
		}	

		return true;
	}

	if (nbFreeCells == 0)
	{
		debugMsg("no more room");
		return true;
	}

	return false;
}

void finishTurn()
{
	nbFreeCells--;

	if (gameOver())
	{
		cout << "Game over" << endl;
		currentTurn = NoTurn;
	}
	else
	{
		if (currentTurn == Player)
			currentTurn = Computer;
		else
			currentTurn = Player;
	}
}

void handleEvent(Event &e)
{
	if (e.type == Event::Closed)
		window.close();

	else if (e.type == Event::MouseButtonPressed && Mouse::isButtonPressed(Mouse::Left) && currentTurn == Player)
	{
		Vector2i position = Mouse::getPosition(window);
		int x = position.x / tileSize;
		int y = position.y / tileSize;

		if (x >= 0 && x < 3 && y >= 0 && y < 3 && grid[x][y] == Empty)
		{
			grid[x][y] = Cross;
			finishTurn();
		}
	}
	else if (e.type == Event::KeyPressed && e.key.code == Keyboard::Space && currentTurn == NoTurn)
		reset();
}

bool computerWin()
{
	Cell* choice = nullptr;

	if		((choice = missingCell(grid[0][0], grid[1][1], grid[2][2], Circle)))	debugMsg("\\ won");
	else if ((choice = missingCell(grid[0][2], grid[1][1], grid[2][0], Circle)))	debugMsg("/ won");
	else if ((choice = missingCell(grid[0][1], grid[1][1], grid[2][1], Circle)))	debugMsg("- won");
	else if ((choice = missingCell(grid[1][0], grid[1][1], grid[1][2], Circle)))	debugMsg(".|. won");
	else if ((choice = missingCell(grid[0][0], grid[1][0], grid[2][0], Circle)))	debugMsg("^ won");
	else if ((choice = missingCell(grid[0][0], grid[0][1], grid[0][2], Circle)))	debugMsg("|.. won");
	else if ((choice = missingCell(grid[0][2], grid[1][2], grid[2][2], Circle)))	debugMsg("_ won");
	else if ((choice = missingCell(grid[2][0], grid[2][1], grid[2][2], Circle)))	debugMsg("..| won");

	if (choice != nullptr)
    {
    	*choice = Circle;
    	return true;
    }

	return false;
}

bool computerProtect()
{
	Cell* choice = nullptr;

	if		((choice = missingCell(grid[0][0], grid[1][1], grid[2][2], Cross)))	debugMsg("\\ protected");
	else if ((choice = missingCell(grid[0][2], grid[1][1], grid[2][0], Cross)))	debugMsg("/ protected");
	else if ((choice = missingCell(grid[0][1], grid[1][1], grid[2][1], Cross)))	debugMsg("- protected");
	else if ((choice = missingCell(grid[1][0], grid[1][1], grid[1][2], Cross)))	debugMsg(".|. protected");
	else if ((choice = missingCell(grid[0][0], grid[1][0], grid[2][0], Cross)))	debugMsg("^ protected");
	else if ((choice = missingCell(grid[0][0], grid[0][1], grid[0][2], Cross)))	debugMsg("|.. protected");
	else if ((choice = missingCell(grid[0][2], grid[1][2], grid[2][2], Cross)))	debugMsg("_ protected");
	else if ((choice = missingCell(grid[2][0], grid[2][1], grid[2][2], Cross)))	debugMsg("..| protected");

	if (choice != nullptr)
    {
    	*choice = Circle;
    	return true;
    }

	return false;
}

void randomMove()
{
	int choice = rand() % nbFreeCells;
	int test = 0;
	for (int j = 0; j < 3; ++j)
		for (int i = 0; i < 3; ++i)
			if (grid[i][j] == Empty && test++ == choice)
				grid[i][j] = Circle;
}

void update()
{
	if (currentTurn == Computer)
	{
		if (!computerWin())
			if (!computerProtect())
				randomMove();

		finishTurn();
	}
}

void draw()
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (grid[i][j] == Cross)
			{
				cross.setPosition(i * tileSize, j * tileSize);
				window.draw(cross);
			}
			else if (grid[i][j] == Circle)
			{
				circle.setPosition(i * tileSize, j * tileSize);
				window.draw(circle);
			}
			else
            {
                background.setPosition(i * tileSize, j * tileSize);
                window.draw(background);
            }

	window.draw(playerScoreText);
	window.draw(computerScoreText);
}

int main()
{
	init();

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
			handleEvent(e);

		update();

		window.clear();
		draw();
		window.display();
	}

	return 0;
}
