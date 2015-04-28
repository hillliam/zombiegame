#include <iostream>
#include <iomanip>
#include <conio.h>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h>

#include "RandomUtils.h"
#include "ConsoleUtils.h"
#include "TimeUtils.h"

using namespace std;

const int SIZEY(12);         //vertical dimension
const int SIZEX(20);         //horizontal dimension

const char SPOT('@');        //spot
const char TUNNEL(' ');      //open space
const char WALL('#');        //border
const char HOLE('O');        //hole
const char ZOMBIE('Z');      //zombie
const char PILL('.');        //pill (used in basic version insted of structure)

const int  UP(72);           //up arrow
const int  DOWN(80);         //down arrow
const int  RIGHT(77);        //right arrow
const int  LEFT(75);         //left arrow

const char FREEZ('F');        //stop the zombies moving
const char EXTERMINATE('X');  //remove all zombies
const char EAT('E');         //remove all pills

const char PLAY('P');		//play buttion
const char INFO('I');
const char SAVE('S');		// save key
const char LOAD('L');		// load key
const char REPLAY('R');		//replay buttion
const char LEADERBOARD('B'); //key to display leaderboard

const char QUIT('Q');        //end the game

struct Item {
	const char symbol;	     //symbol on grid
	int x, y;			     //coordinates
};

struct player {
	Item baseobject;         // the base class of all objects on the map
	const string name;		 // the name of the player
	int lives;               // the number of lives the player has
	int score;               // the score the player has acheaved
	bool hascheated;		 // set true if the user has cheated
	bool isProtected;
	int protectedCount;
	int levelChoice;
};

struct zombie {
	Item baseobject;         // the base class of all objects on the map
	int startx;              // the start location of the zombie
	int starty;
	bool imobalized;		 // set true if the zombie cant move
	bool alive;
	bool hidden;
	zombie operator= (const zombie& it)
	{
		zombie a = it;
		return a;
	}
};

struct pill {
	Item baseobject;         // the base class of all objects on the map
	bool eaten;
	pill operator= (const pill& it)
	{
		pill a = it;
		return a;
	}
};

struct replay
{
	char grid[SIZEY][SIZEX]; // store the grid 
};

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie>& zombies, vector<Item>& holes, vector<pill>& pills);
	bool isArrowKey(const int k);
	bool isCheatKey(const int k);
	int getsize(const vector<pill>& pills);
	int levelSelection;
	int  getKeyPress();
	bool endconditions(vector<zombie>& zombies, const int pills, const player &spot, const int key, string& message);
	void ApplyCheat(const int key, vector<zombie>& zombies, vector<pill>& pills);
	void updateGame(char grid[][SIZEX], player& spot, const int key, string& message, vector<zombie>& zombies, vector<pill>& pills, const vector<Item>& holes);
	void renderGame(const char g[][SIZEX], const string &mess, const player &spot, const int zomlives, const int remaingpills, int diff);
	void endProgram(const string &message);
	string mainloop(int& levelSelection);
	void savescore(const string &name, const int score);
	bool readsavedcore(const string &name, const int score);
	bool haswon(vector<zombie>& zombies, string& message, const player &spot);
	bool haslost(const player &spot, string& message);
	bool wantToQuit(const int k, string& message);
	void updatescore(const string &name, const int score);
	bool issaveKey(const int k);
	bool isloadKey(const int k);
	bool isreplayKey(const int k);
	void displayallmoves(const vector<replay> &replayer);
	void savegame(const player& spot, const vector<zombie>& zombies, const vector<pill>& pills, const vector<Item>& holes);
	void loadgame(const string &name, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes);
	//local variable declarations 
	char grid[SIZEY][SIZEX];                //grid for display
	string message("LET'S START...      "); //current message to player
	int key(' ');
	vector<replay> replayer;
	player spot = { SPOT, 0, 0, mainloop(levelSelection), 5 };                        //create key to store keyboard events 
	spot.levelChoice = levelSelection;
	Clrscr();
	int hours, min, seconds; // do we need to reset the timer when we load the next level
	GetSystemTime(hours, min, seconds);// populates the varibles
	do {
		vector<zombie> zombies;					// initalize the 4 zombies
		vector<pill> pills; 					// initalize avalible pills to 8
		vector<Item> holes; 					// 12 holes
		initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot)
		int nhours, nmin, nseconds;
		GetSystemTime(nhours, nmin, nseconds);
		const int diff = (((nhours - hours) * 3600) + ((nmin - min) * 60) + (nseconds - seconds));
		renderGame(grid, message, spot, zombies.size(), pills.size(), diff);
		do {
			if (_kbhit())
			{
				message = "                    "; //reset message
				key = getKeyPress();              //read in next keyboard event
				if (isArrowKey(key))
					updateGame(grid, spot, key, message, zombies, pills, holes);
				else if (isCheatKey(key))
				{
					spot.hascheated = true;
					ApplyCheat(key, zombies, pills);
					updateGame(grid, spot, key, message, zombies, pills, holes);
				}
				else if (issaveKey(key))
					savegame(spot, zombies, pills, holes);
				else if (isloadKey(key))
					loadgame(spot.name, zombies, pills, holes);
				else if (isreplayKey(key))
					displayallmoves(replayer);
			}
			renderGame(grid, message, spot, zombies.size(), getsize(pills), diff);        //render game state on screen
		} while (!wantToQuit(key, message) && (!haswon(zombies, message, spot) && !haslost(spot, message)));      //while user does not want to quit
		spot.levelChoice++;
		spot.isProtected = false;
	} while (spot.levelChoice <= 3 && !wantToQuit(key, message) && !haslost(spot, message));
	if (!spot.hascheated)
	{
		if (!readsavedcore(spot.name, spot.lives))
			savescore(spot.name, spot.lives);
		updatescore(spot.name, spot.lives);
	}
	endProgram(message);                             //display final message
}
bool isreplayKey(const int key)
{
	return (toupper(key) == REPLAY);
}
bool issaveKey(const int k)
{
	if (toupper(k) == SAVE)
		return true;
	else
		return false;
}
void displayallmoves(const vector<replay> &replayer)
{
	void paintGrid(const char g[][SIZEX]);
	int getKeyPress();
	void showDescription();
	void showTitle();
	void showOptions();
	void showmenu();
	void showtime();
	void showMessage(const string&);
	int index = 0;
	char key = ' ';
	Clrscr();
	while (index != replayer.size())
	{
		showDescription();
		showTitle();
		showOptions();
		showmenu();
		showtime();
		stringstream a;
		a << "displaying move " << index << " of " << replayer.size();
		showMessage(a.str());
		paintGrid(replayer[index].grid);
		index++;
	}
}
bool isloadKey(const int k)
{
	if (toupper(k) == LOAD)
		return true;
	else
		return false;
}

int getsize(const vector<pill>& pills)
{
	int pils = 0;
	for (const pill& item : pills)
		if (!item.eaten)
			++pils;
	return pils;
}
void savegame(const player &spot, const vector<zombie> &zombies, const vector<pill> &pills, const vector<Item> &holes)
{
	ofstream writer(spot.name + ".save");
	writer << spot.baseobject.x << endl;
	writer << spot.baseobject.y << endl;
	writer << spot.hascheated << endl;
	writer << spot.isProtected << endl;
	writer << spot.lives << endl;
	writer << spot.score << endl;
	writer << zombies.size() << endl;
	for (zombie a : zombies)
	{
		writer << a.baseobject.x << endl;
		writer << a.baseobject.y << endl;
		writer << a.imobalized << endl;
		writer << a.startx << endl;
		writer << a.starty << endl;
	}
	writer << pills.size() << endl;
	for (pill a : pills)
	{
		writer << a.baseobject.x << endl;
		writer << a.baseobject.y << endl;
		writer << a.eaten << endl;
	}
	writer << holes.size() << endl;
	for (Item a : holes)
	{
		writer << a.x << endl;
		writer << a.y << endl;
	}
}

void loadgame(player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes)
{
	zombies.clear();
	pills.clear();
	holes.clear();
	ifstream reader(spot.name + ".save");
	reader >> spot.baseobject.x;
	reader >> spot.baseobject.y;
	reader >> spot.hascheated;
	reader >> spot.isProtected;
	reader >> spot.lives;
	reader >> spot.score;
	int numofzom;
	reader >> numofzom;
	for (int i = 0; i != numofzom; i++)
	{
		zombie a = { ZOMBIE };
		reader >> a.baseobject.x;
		reader >> a.baseobject.y;
		reader >> a.imobalized;
		reader >> a.startx;
		reader >> a.starty;
		zombies.push_back(a);
	}
	reader >> numofzom;
	for (int i = 0; i != numofzom; i++)
	{
		pill a = { PILL };
		reader >> a.baseobject.x;
		reader >> a.baseobject.y;
		reader >> a.eaten;
		pills.push_back(a);
	}
	reader >> numofzom;
	for (int i = 0; i != numofzom; i++)
	{
		Item a = { HOLE };
		reader >> a.x;
		reader >> a.y;
		holes.push_back(a);
	}
}
string mainloop(int& levelSelection)
{
	void requestname();
	void showTitle();
	void showOptions();
	void showmenu();
	void showtime();
	void showgametitle();
	int getscore(const string&);
	int  getKeyPress();
	void clearMessage();
	void showscore(const int score);
	void getLevel();
	void displayhighscores();
	void showDescription();
	string name = "";
	char key = ' ';
	while (toupper(key) != PLAY)
	{
		showTitle();
		showgametitle();
		showOptions();
		showtime();
		showmenu();
		key = getKeyPress();
		if (toupper(key) == INFO)
			showDescription();
		else if (toupper(key) == LEADERBOARD)
			displayhighscores();
		else if (toupper(key) == QUIT)
			return 0;
		else if (toupper(key) != PLAY)
		{
			SelectBackColour(clRed);
			SelectTextColour(clYellow);
			Gotoxy(40, 13);
			cout << "INVALID KEY!  ";
		}
	}
	requestname();
	cin >> name;
	clearMessage();
	getLevel();
	cin >> levelSelection;
	clearMessage();
	int previousscore = getscore(name);
	showscore(previousscore);
	return name;
}

void savescore(const string &name, const int score)
{
	ofstream out(name + ".scr");
	if (!out.fail())
		out << score;
	out.close();
}

bool readsavedcore(const string &name, const int score)
{
	ifstream in(name + ".scr");
	if (!in.fail())// the file may not be found
	{
		int storedscore;
		in >> storedscore;
		if (storedscore > score)
			return true;
		else
			return false;
	}
	in.close();
	return false;
}

int getscore(const string &name)
{
	ifstream in(name + ".scr");
	if (!in.fail())// the file may not be found
	{
		int storedscore;
		in >> storedscore;
		return storedscore;
	}
	in.close();
	return -1;
}

void updateGame(char grid[][SIZEX], player& spot, const int key, string& message, vector<zombie>& zombies, vector<pill>& pills, const vector<Item>& holes)
{
	void updateSpotCoordinates(const char g[][SIZEX], player& spot, const int key, string& mess, vector<zombie>& zombies, vector<pill>& pills); // player move 
	void updatezombieCoordinates(const char g[][SIZEX], player& spot, vector<zombie>& zombies); // zombies move
	void updateGrid(char grid[][SIZEX], const Item &spot, const vector<zombie> &zombies, const vector<pill> &pills, const vector<Item> &holes);

	updateSpotCoordinates(grid, spot, key, message, zombies, pills);    //update spot coordinates
	//according to key
	updatezombieCoordinates(grid, spot, zombies);				// zombies move
	// this can be just passed a vector<item> made from the .baseobject of all objects needing to be renderd
	updateGrid(grid, spot.baseobject, zombies, pills, holes);    //update grid information
}

void updatezombieCoordinates(const char g[][SIZEX], player& spot, vector<zombie>& zombies) // zombies move
{
	void getrandommove(const player&, int& x, int& y);
	void retreat(const player&, int& x, int& y);
	for (int i = 0; i < zombies.size(); i++)
	{
		if (zombies[i].imobalized == false)
		{
			//calculate direction of movement required by key - if any
			int dx(zombies[i].baseobject.x), dy(zombies[i].baseobject.y);
			if (!spot.isProtected)
			{
				getrandommove(spot, dx, dy);
			}
			else
			{
				retreat(spot, dx, dy);
			}
			const int targetY(zombies[i].baseobject.y + dy);
			const int targetX(zombies[i].baseobject.x + dx);
			switch (g[targetY][targetX])
			{		//...depending on what's on the target position in grid...
			case PILL:
			case TUNNEL:      //can move
				zombies[i].baseobject.y += dy;   //go in that Y direction
				zombies[i].baseobject.x += dx;   //go in that X direction
				break;
			case SPOT:// dont know if neede
				spot.lives--;
				zombies[i].baseobject.x = zombies[i].startx;
				zombies[i].baseobject.y = zombies[i].starty;
				break;
			case ZOMBIE:
				zombies[i].baseobject.x = zombies[i].startx;
				zombies[i].baseobject.y = zombies[i].starty;
				for (zombie& item : zombies)
				{
					if (item.baseobject.x == targetX && item.baseobject.y == targetY)
					{
						item.baseobject.x = item.startx;
						item.baseobject.y = item.starty;
					}
				}
				break;
			case HOLE://remove the zombie from map
				zombies[i].alive = false;
			}
		}
	}
}

void ApplyCheat(const int key, vector<zombie>& zombies, vector<pill>& pills)
{
	if (toupper(key) == EAT)//remove all pils from the grid
		pills.clear();
	else if (toupper(key) == EXTERMINATE)//remove all zombies from board
	{
		for (int i = 0; i != zombies.size(); i++)
		{
			zombies[i].hidden = !zombies[i].hidden;
			zombies[i].baseobject.x = zombies[i].startx;
			zombies[i].baseobject.y = zombies[i].starty;
		}
	}
	else if (toupper(key) == FREEZ)// do nothing when it is the zombies turn to move
		for (int i = 0; i != zombies.size(); i++)
			zombies[i].imobalized = !zombies[i].imobalized;
}

void getrandommove(const player &spot, int& x, int& y)
{
	if (spot.baseobject.x > x)
		x = 1;
	else
		x = -1;
	if (spot.baseobject.y > y)
		y = 1;
	else
		y = -1;
}

void retreat(const player &spot, int& x, int& y)
{
	if (spot.baseobject.x > x)
		x = -1;
	else
		x = 1;
	if (spot.baseobject.y > y)
		y = -1;
	else
		y = 1;
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie>& zombies, vector<Item>& holes, vector<pill>& pills)
{ //initialise grid and place spot in middle
	void setGrid(char[][SIZEX]);
	void setSpotInitialCoordinates(char grid[][SIZEX], Item& spot);
	void placewallonmap(char grid[][SIZEX]);
	void placeSpot(char gr[][SIZEX], const Item &spot);
	void placepillonmap(char grid[][SIZEX], vector<pill>& pills, player& spot);
	void placeholeonmap(char grid[][SIZEX], vector<Item>& holes, player& spot);
	void placezombiesonmap(char grid[][SIZEX], vector<zombie>& zombies);
	void getLevel();

	Seed();                            //seed reandom number generator
	setGrid(grid);                     //reset empty grid
	placewallonmap(grid);
	placezombiesonmap(grid, zombies);  // place the zombies on the map
	setSpotInitialCoordinates(grid, spot.baseobject);//initialise spot position
	placeSpot(grid, spot.baseobject);  //set spot in grid
	switch (spot.levelChoice)
	{
	case 1:
		spot.lives = 8;
		break;
	case 2:
		spot.lives = 5;
		break;
	case 3:
		spot.lives = 3;
		break;
	}
	placepillonmap(grid, pills, spot);	   // place pills on the map
	placeholeonmap(grid, holes, spot);       // place holes on the map
}
void getLevel()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 15);
	cout << "please select a level: ";
}

void placepillonmap(char grid[][SIZEX], vector<pill>& pills, player& spot)
{
	void occupyPills(int numberOfPills, char grid[][SIZEX], vector<pill>& pills);
	int selection;
	switch (spot.levelChoice)
	{
	case 1:
		 selection = 8;
		occupyPills(selection, grid, pills);
		break;
	case 2:
		 selection = 5;
		occupyPills(selection, grid, pills);
		break;
	case 3:
		 selection = 3;
		occupyPills(selection, grid, pills);
		break;
	}
}

void occupyPills(int numberOfPills, char grid[][SIZEX], vector<pill>& pills)
{
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y);
	for (int i = 0; i < numberOfPills; i++) // place 8 pills on the map
	{
		int x = Random(SIZEX - 2); //
		int y = Random(SIZEY - 2); // 
		while (ocupiedpeace(grid, x, y))
		{
			Seed();
			x = Random(SIZEX - 2); // get new chordinates
			y = Random(SIZEY - 2); // 
		}
		pill pilla = { PILL, x, y };
		pills.push_back(pilla);
		grid[y][x] = PILL; // place it on the map	
	}
}

void placeholeonmap(char grid[][SIZEX], vector<Item>& holes, player& spot)
{
	void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles);
	int selection;
	switch (spot.levelChoice)
	{
	case 1:
		selection = 12;
		occupyHoles(grid, holes, selection);
		break;
	case 2:
		selection = 5;
		occupyHoles(grid, holes, selection);
		break;
	case 3:
		selection = 2;
		occupyHoles(grid, holes, selection);
		break;
	}
}

void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles)
{
	void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles);
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y);
	for (int i = 0; i < numberOfHoles; i++) // place 12 holes on the map
	{
		int x = Random(SIZEX - 2); //
		int y = Random(SIZEY - 2); // 
		while (ocupiedpeace(grid, x, y))
		{
			Seed();
			x = Random(SIZEX - 2); // get new chordinates
			y = Random(SIZEY - 2); // 
		}
		Item hole = { HOLE, x, y };
		grid[y][x] = HOLE;
		holes.push_back(hole);
	}
}

void placewallonmap(char grid[][SIZEX])
{
	for (int y = 3; y < 6; y++)
	{
		for (int x = 4; x < 7; x++)
		{
			grid[y][x] = WALL;
		}
			
	}
	for (int y2 = 8; y2 > 5; y2--)
	{
		for (int x2 = 15; x2 > 12; x2--)
		{
			grid[y2][x2] = WALL;
		}

	}
		
}

void placezombiesonmap(char grid[][SIZEX], vector<zombie>& zombies)
{
	const zombie zom1 = { ZOMBIE, 1, 1, 1, 1, false, true }; // {{item}, startx, starty, imobilzed, alive}
	const zombie zom2 = { ZOMBIE, SIZEX - 2, 1, SIZEX - 2, 1, false, true };
	const zombie zom3 = { ZOMBIE, 1, SIZEY - 2, 1, SIZEY - 2, false, true };
	const zombie zom4 = { ZOMBIE, SIZEX - 2, SIZEY - 2, SIZEX - 2, SIZEY - 2, false, true };
	zombies.push_back(zom1);
	zombies.push_back(zom2);
	zombies.push_back(zom3);
	zombies.push_back(zom4);
	grid[1][1] = ZOMBIE; // place it on the map	
	grid[SIZEY - 2][1] = ZOMBIE;
	grid[1][SIZEX - 2] = ZOMBIE;
	grid[SIZEY - 2][SIZEX - 2] = ZOMBIE;
}

void setSpotInitialCoordinates(char grid[][SIZEX], Item& spot)
{
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y);
	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
	while (ocupiedpeace(grid, spot.x, spot.y))
	{
		Seed();
		spot.x = Random(SIZEX - 2); // get new chordinates
		spot.y = Random(SIZEY - 2); // 
	}
}

void setGrid(char grid[][SIZEX])
{ //reset the empty grid configuration
	for (int row(0); row < SIZEY; ++row) //for each column
	{
		for (int col(0); col < SIZEX; ++col) //for each col
		{
			if ((row == 0) || (row == SIZEY - 1))     //top and bottom walls
				grid[row][col] = WALL;                //draw a wall symbol
			else
				if ((col == 0) || (col == SIZEX - 1)) //left and right walls
					grid[row][col] = WALL;            //draw a wall symbol
				else
					grid[row][col] = TUNNEL;          //draw a space
		} //end of row-loop
	} //end of col-loop
}

void placeSpot(char gr[][SIZEX], const Item &spot)
{ //place spot at its new position in griupa
	gr[spot.y][spot.x] = spot.symbol;
}

void updateGrid(char grid[][SIZEX], const Item &spot, const vector<zombie> &zombies, const vector<pill> &pills, const vector<Item> &holes)
{
	void setGrid(char[][SIZEX]);
	void placewallonmap(char g[][SIZEX]);
	void placeSpot(char g[][SIZEX], const Item &spot);
	void placezombies(char g[][SIZEX], const vector<zombie> &zombies);
	void placepill(char g[][SIZEX], const vector<pill> &pills);
	void placeitem(char g[][SIZEX], const vector<Item> &holes);

	setGrid(grid);	         //reset empty grid
	placewallonmap(grid);
	placezombies(grid, zombies); //set zombies on map
	placepill(grid, pills);      //set pills on map
	placeitem(grid, holes); // set the holes on the grid
	placeSpot(grid, spot);	 //set spot in grid
}

void placepill(char g[][SIZEX], const vector<pill> &pills)
{
	for (const pill& item : pills)
		if (!item.eaten)
			g[item.baseobject.y][item.baseobject.x] = item.baseobject.symbol;
}

void placeitem(char g[][SIZEX], const vector<Item> &holes)
{
	for (const Item& it : holes)
		g[it.y][it.x] = it.symbol;
}

void placezombies(char g[][SIZEX], const vector<zombie> &zombies)
{
	for (const zombie& item : zombies)
		if (item.alive == true && item.hidden == false)
			g[item.baseobject.y][item.baseobject.x] = item.baseobject.symbol;
}

void updateSpotCoordinates(const char g[][SIZEX], player& sp, const int key, string& mess, vector<zombie>& zombies, vector<pill>& pills)
{
	void setKeyDirection(const int k, int& dx, int& dy);

	//calculate direction of movement required by key - if any
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy); 	//find direction indicated by key
	//check new target position in grid 
	//and update spot coordinates if move is possible
	const int targetY(sp.baseobject.y + dy);
	const int targetX(sp.baseobject.x + dx);

	switch (g[targetY][targetX])
	{		//...depending on what's on the target position in grid...
	case TUNNEL:      //can move
		sp.baseobject.y += dy;   //go in that Y direction
		sp.baseobject.x += dx;   //go in that X direction
		if (sp.isProtected)
			sp.protectedCount--;
		break;
	case WALL:        //hit a wall and stay there
		cout << '\a'; //beep the alarm
		mess = "CANNOT GO THERE!    ";
		if (sp.isProtected)
			sp.protectedCount--;
		break;
	case ZOMBIE:
		sp.baseobject.y += dy;   //go in that Y direction
		sp.baseobject.x += dx;   //go in that X direction
		if (sp.isProtected)
		{
			sp.protectedCount--;
			for (int i = 0; i < zombies.size(); i++)
				if (zombies[i].baseobject.x == sp.baseobject.x && zombies[i].baseobject.y == sp.baseobject.y) // fix me removing the wrong pill
					zombies[i].alive = false; // again needs to be fixed
			break;
		}
		else
			sp.lives--;
		for (zombie& it : zombies)
		{
			if (sp.baseobject.x == it.baseobject.x && sp.baseobject.y == it.baseobject.y)
			{
				it.baseobject.x = it.startx;
				it.baseobject.y = it.starty;
			}
		}
		break;
	case HOLE:
		sp.baseobject.y += dy;   //go in that Y direction
		sp.baseobject.x += dx;   //go in that X direction
		sp.lives--;
		if (sp.isProtected)
			sp.protectedCount--;
		break;
	case PILL:
		sp.baseobject.y += dy;   //go in that Y direction
		sp.baseobject.x += dx;   //go in that X direction
		sp.lives++;
		if (sp.isProtected)
			sp.protectedCount--;
		sp.isProtected = true;	 // protect the player
		switch (sp.levelChoice)
		{
		case 1:
			sp.protectedCount = 10;// set number of levels to protect
			break;
		case 2:
			sp.protectedCount = 8;// set number of levels to protect
			break;
		case 3:
			sp.protectedCount = 5;// set number of levels to protect
			break;
		}
		for (int i = 0; i < pills.size(); i++)
			if (pills[i].baseobject.x == sp.baseobject.x && pills[i].baseobject.y == sp.baseobject.y) // fix me removing the wrong pill
				pills[i].eaten = true; // again needs to be fixed
		break;
	}
	if (sp.protectedCount == 0)
		sp.isProtected = false;
}


void setKeyDirection(const int key, int& dx, int& dy)
{
	switch (key)    //...depending on the selected key...
	{
	case UP:		//when UP arrow pressed...
		dx = 0;
		dy = -1;	//decrease the Y coordinate
		break;
	case DOWN:		//when DOWN arrow pressed...
		dx = 0;
		dy = 1;		//increase the Y coordinate
		break;
	case LEFT:      //when LEFT arrow pressed...
		dx = -1;    //decrease the X coordinate
		dy = 0;
		break;
	case RIGHT:     //when RIGHT arrow pressed...
		dx = +1;    //increase the X coordinate
		dy = 0;
	}
}

int getKeyPress()
{
	int keyPressed;
	keyPressed = getch();      //read in the selected arrow key or command letter
	while (keyPressed == 224)     //ignore symbol following cursor key
		keyPressed = getch();
	return(keyPressed);
}

bool isArrowKey(const int key)
{
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
}

bool isCheatKey(const int key)
{
	return ((toupper(key) == EAT) || (toupper(key) == EXTERMINATE) || (toupper(key) == FREEZ));
}

bool wantToQuit(const int key, string& message)
{
	bool exit = (toupper(key) == QUIT);
	if (exit)
		message = "you have quit";
	return exit;
}

bool haswon(vector<zombie>& zombies, string& message, const player& spot)
{		
	if (zombies[0].alive == true || zombies[1].alive == true || zombies[2].alive == true || zombies[3].alive == true)
	{
		return false;
	}
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 16);
	cout << "Congratulations, you win ";
	Gotoxy(40, 17);
	cout << "Your score is: " << spot.lives;
	return true;
}


bool haslost(const player &spot, string& message)
{
	if (spot.lives == 0)
	{
		message = "you have no lives";
		return true;
	}
	else
		return false;
}

bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y)
{
	if (gd[y][x] == PILL || gd[y][x] == HOLE || gd[y][x] == ZOMBIE || gd[y][x] == SPOT || gd[y][x] == WALL)
		return true;
	else
		return false;
}

void clearMessage()
{
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	string str(20, ' ');
	cout << str;  //display blank message
}

void renderGame(const char gd[][SIZEX], const string &mess, const player &spot, const int zombielives, const int remainingpill, int diff)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(const char g[][SIZEX]);
	void showLives(const player &spot);
	void showDescription();
	void showzomLives(const int lives);
	void showrempill(const int pils);
	void showTitle();
	void showOptions();
	void showtime();
	void showMessage(const string&);
	void showname(const string &name);
	void showscore(const int score);
	void showdiff(int diff);

	Gotoxy(0, 0);
	//display grid contents
	paintGrid(gd);
	//display game title
	showTitle();
	showDescription();
	showdiff(diff);
	showtime();
	showLives(spot);
	showname(spot.name);
	int previousscore = getscore(spot.name);
	showscore(previousscore);
	//show number of zombie lives
	showzomLives(zombielives);
	//show number of remaing pills
	showrempill(remainingpill);
	//display menu options available
	showOptions();
	//display message if any
	showMessage(mess);
}

void paintGrid(const char g[][SIZEX])
{
	SelectBackColour(clBlack);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			switch (g[row][col])
			{
			case SPOT:
			case WALL:
				SelectTextColour(clWhite);
				break;
			case ZOMBIE:
				SelectTextColour(clGreen);
				break;
			case HOLE:
				SelectTextColour(clRed);
				break;
			case PILL:
				SelectTextColour(clYellow);
			}
			cout << g[row][col];              //output cell content
		} //end of col-loop
		cout << endl;
	} //end of row-loop
}

void showzomLives(const int lives)
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 11);
	cout << "zombie lives: " << lives;
}

void showrempill(const int pils)
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 10);
	cout << "pills left: " << pils;
}

void showDescription()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 2);
	cout << "This is a game where you must escape";
	Gotoxy(40, 3);
	cout << "the zombies and survive. Pills mean";
	Gotoxy(40, 4);
	cout << "a life is gained.";
	Gotoxy(40, 5);
	cout << "Contact with a hole(0) or zombie(Z)";
	Gotoxy(40, 6);
	cout << "means a life is lost ";
}

void showTitle()
{ //display game title
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "___ZOMBIES GAME SKELETON___\n" << endl;
	SelectBackColour(clWhite);
	SelectTextColour(clRed);
	Gotoxy(40, 0);
	cout << "Oliver Parker, Liam Hill, Alex Odgen";
	Gotoxy(40, 1);
	cout << "1RR - COMPUTER SCIENCE";
}

void showname(const string &name)
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 13);
	cout << "your name: " << name;
}

void showOptions()
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 7);
	cout << "TO MOVE USE KEYBOARD ARROWS  ";
	Gotoxy(40, 8);
	cout << "TO QUIT ENTER 'Q'   ";
}

void showLives(const player &spot)
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 9);
	cout << spot.lives << " lives left";
}

void showMessage(const string &m)
{ //print auxiliary messages if any
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	cout << m;	//display current message
}

void endProgram(const string &message)
{ //end program with appropriate message
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	Gotoxy(40, 8);
	cout << message;
	//hold output screen until a keyboard key is hit
	Gotoxy(40, 9);
	system("pause");
}

void showmenu()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 10);
	cout << "press p to play";
	Gotoxy(40, 11);
	cout << "press i to get infomation";
	Gotoxy(40, 12);
	cout << "press b to display leaderboard";

}

void showscore(const int score)
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 15);
	cout << "player score: " << score;

}

void showtime()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 11);
	cout << GetDate();
	Gotoxy(40, 12);
	cout << GetTime();
}

void showgametitle()
{
	SelectBackColour(clBlue);
	SelectTextColour(clYellow);
	Gotoxy(2, 4);
	cout << "------------------------";
	Gotoxy(2, 5);
	cout << "| SPOT AND ZOMBIE GAME |";
	Gotoxy(2, 6);
	cout << "------------------------";
}

void requestname()
{
	SelectBackColour(clBlue);
	SelectTextColour(clYellow);
	Gotoxy(2, 11);
	cout << "please enter your name: ";
}

void displayhighscores()
{
	ifstream in("best.scr");
	Gotoxy(2, 13);
	cout << "name	   score";
	if (!in.fail())// the file may not be found
	{
		for (int i = 0; i != 3; ++i)
		{
			int storedscore; // the score 
			string name; // the name
			in >> name;
			in >> storedscore;
			Gotoxy(2, 14 + i);
			cout << name << "	" << storedscore;
		}
	}
	in.close();
}

void updatescore(const string &name, const int score)
{
	ifstream in("best.scr");
	if (!in.fail())
	{
		Gotoxy(2, 13);
		string name1;
		string name2;
		string name3;
		int score1;
		int score2;
		int score3;
		in >> name1;
		in >> score1;
		in >> name2;
		in >> score2;
		in >> name3;
		in >> score3;
		in.close();
		ofstream out("best.scr");
		if (score1 < score)
		{
			score3 = score2;
			score2 = score1;
			name3 = name2;
			name2 = name1;
			score1 = score;
			name1 = name;
		}
		else if (score2 < score)
		{
			score3 = score2;
			name3 = name2;
			score2 = score;
			name2 = name;
		}
		else if (score3 < score)
		{
			score3 = score;
			name3 = name;
		}
		out << name1 << endl;

		out << score1 << endl;
		out << name2 << endl;
		out << score2 << endl;
		out << name3 << endl;
		out << score3 << endl;
		out.close();
	}

}
	void showdiff(int diff)
	{
		SelectBackColour(clRed);
		SelectTextColour(clYellow);
		Gotoxy(40, 16);
		cout << "time spent in game: " << diff;
	}

