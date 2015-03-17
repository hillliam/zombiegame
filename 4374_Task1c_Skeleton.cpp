#include <iostream>
#include <iomanip>
#include <conio.h>
#include <string>
#include <vector>

using namespace std;

#include "RandomUtils.h"
#include "ConsoleUtils.h"

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

const char QUIT('Q');        //end the game

struct Item {
	const char symbol;	     //symbol on grid
	int x, y;			     //coordinates
};

struct player {
	Item baseobject;         // the base class of all objects on the map
	int lives;               // the number of lives the player has
	int score;               // the score the player has acheaved
	bool hascheated;		 // set true if the user has cheated
};

struct zombie {
	Item baseobject;         // the base class of all objects on the map
	int startx;              // the start location of the zombie
	int starty;             
	bool imobalized;		 // set true if the zombie cant move
};
struct pill {
	Item baseobject;         // the base class of all objects on the map
	bool eaten;				 // set true if the will not be displayed
	int numberOfPills;		 // number of pills on map
};

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie>& zombies, vector<Item>& holes, vector<pill>& pills);
	bool isArrowKey(int k);
	bool isCheatKey(int k);
	int  getKeyPress();
	bool endconditions(char grid[][SIZEX], player spot, int key, string& message);
	void ApplyCheat(char grid[][SIZEX], player& spot, int key, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes);
	void updateGame(char grid[][SIZEX], player& spot, int key, string& message, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes);
	void renderGame(const char g[][SIZEX], string mess, player spot, const int zomlives, const int remaingpills);
	void endProgram();
    int zombielives = 3;
	//local variable declarations 
	char grid[SIZEY][SIZEX];                //grid for display
	player spot = { SPOT };                 //Spot's symbol and position (0, 0) 
	vector<zombie> zombies;					// initalize the 4 zombies
	vector<pill> pills; 					// initalize avalible pills to 8
	vector<Item> holes; 					// 12 holes
	spot.lives = 5;
	string message("LET'S START...      "); //current message to player
	
	
	initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot)
	int key(' ');                         //create key to store keyboard events 
	do {
		renderGame(grid, message, spot,zombielives, pills.size());        //render game state on screen
		message = "                    "; //reset message
		key = getKeyPress();              //read in next keyboard event
		if (isArrowKey(key))
			updateGame(grid, spot, key, message,zombies, pills, holes);
		else if (isCheatKey(key))
			ApplyCheat(grid, spot, key, zombies, pills, holes);
		else
			message = "INVALID KEY!        ";
	} while (endconditions(grid, spot, key, message));      //while user does not want to quit
	endProgram();                             //display final message
	return 0;
}

void updateGame(char grid[][SIZEX], player& spot, int key, string& message, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes)
{
	void updateSpotCoordinates(const char g[][SIZEX], player& spot, int key, string& mess); // player move 
	void updatezombieCoordinates(const char g[][SIZEX], vector<zombie>& zombies); // zombies move
	void updateGrid(char grid[][SIZEX], Item spot, vector<zombie> zombies, vector<pill> pills, vector<Item> holes);
	void checkpillcolition(Item spot, vector<pill>& pills);
	void checkzombiecolition(Item spot, vector<zombie> zombies);

	updateSpotCoordinates(grid, spot, key, message);    //update spot coordinates
                                                        //according to key
	updatezombieCoordinates(grid, zombies);				// zombies move
	// check colition 
	checkpillcolition(spot.baseobject, pills);
	checkzombiecolition(spot.baseobject, zombies);
	// this can be just passed a vector<item> made from the .baseobject of all objects needing to be renderd
	updateGrid(grid, spot.baseobject, zombies, pills, holes);    //update grid information
}
void checkpillcolition(Item spot, vector<pill>& pills)
{
	for (int i = 0; i != pills.size(); i++)
	{
		if (pills[i].baseobject.x == spot.x && pills[i].baseobject.y == spot.y)
		{
			pills.erase(pills.begin() + i);
		}
	}
}
void checkzombiecolition(Item spot, vector<zombie> zombies)
{
	for (int i = 0; i != zombies.size(); i++)
	{
		if (zombies[i].baseobject.x == spot.x && zombies[i].baseobject.y == spot.y)
		{
			zombies.erase(zombies.begin() + i);
		}
	}
}
void updatezombieCoordinates(const char g[][SIZEX], vector<zombie>& zombies) // zombies move
{
	void getrandommove(int& x, int& y);
	for (int i = 0; i != zombies.size(); i++)
	{
		if (zombies[i].imobalized == false)
		{
			//calculate direction of movement required by key - if any
			int dx(0), dy(0);
			getrandommove(dx, dy); // if we pass the grid to this we can check to make it rare that the rombie falls down a hole 
			//check new target position in grid 
			//and update spot coordinates if move is possible
			const int targetY(zombies[i].baseobject.y + dy);
			const int targetX(zombies[i].baseobject.x + dx);
			switch (g[targetY][targetX])
			{		//...depending on what's on the target position in grid...
			case TUNNEL:      //can move
				zombies[i].baseobject.y += dy;   //go in that Y direction
				zombies[i].baseobject.x += dx;   //go in that X direction
				break;
			case SPOT:// dont know if needex 
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
			case HOLE: // may need to store the starting chord of eatch zombie to make it spawn in its corner
				zombies[i].baseobject.x = zombies[i].startx;
				zombies[i].baseobject.y = zombies[i].starty;
				break;
			}
		}
	}
}
void ApplyCheat(char grid[][SIZEX], player& spot, int key, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes)
{
	if (key == EAT)//remove all pils from the grid
		pills.clear();
	else if (key == EXTERMINATE)//remove all zombies from board
		zombies.clear();
	else if (key == FREEZ)// do nothing when it is the zombies turn to move
		for (int i = 0; i != 4; i++)
		{
			zombie& a = zombies[i];
			a.imobalized = true;
		}
}
 
void getrandommove(int& x, int& y)
{
	x = Random(4) - 2;// number between -1 and 1
	y = Random(4) - 2;// number between -1 and 1
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie>& zombies, vector<Item>& holes, vector<pill>& pills)
{ //initialise grid and place spot in middle
	void setGrid(char[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot);
	void placeSpot(char gr[][SIZEX], Item spot);
	void placepillonmap(char grid[][SIZEX], vector<pill>& pills);
	void placeholeonmap(char grid[][SIZEX], vector<Item>& holes);
	void placezombiesonmap(char grid[][SIZEX], vector<zombie>& zombies);

	Seed();                            //seed reandom number generator
	setSpotInitialCoordinates(spot.baseobject);//initialise spot position
	setGrid(grid);                     //reset empty grid
	placeSpot(grid, spot.baseobject);  //set spot in grid
	placepillonmap(grid, pills);	   // place pills on the map
	placeholeonmap(grid, holes);       // place holes on the map
	placezombiesonmap(grid, zombies);  // place the zombies on the map
}

void placepillonmap(char grid[][SIZEX], vector<pill>& pills)
{
	bool ocupiedpeace(const char gd[][SIZEX], int x, int y);
	for (int i = 0; i != 8; i++) // place 8 pills on the map
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
		cout << "placed pill at " << x << " " << y << endl;
		pills.push_back(pilla);
		grid[y][x] = PILL; // place it on the map	
	}
}

void placeholeonmap(char grid[][SIZEX], vector<Item>& holes)
{
	bool ocupiedpeace(const char gd[][SIZEX], int x, int y);
	for (int i = 0; i != 12; i++) // place 12 holes on the map
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
		cout << "placed hole at " << x << " " << y << endl;
		grid[y][x] = HOLE;
		holes.push_back(hole);
	}
}

void placezombiesonmap(char grid[][SIZEX], vector<zombie>& zombies)
{
	zombie zom1 = { ZOMBIE, 1, 1, false, 1, 1 };
	zombie zom2 = { ZOMBIE, SIZEX - 2, 1, false, SIZEX - 2, 1 };
	zombie zom3 = { ZOMBIE, 1, SIZEY - 2, false, 1, SIZEY - 2 };
	zombie zom4 = { ZOMBIE, SIZEX - 2, SIZEY - 2, false, SIZEX - 2, SIZEY - 2 };
	zombies.push_back(zom1);
	zombies.push_back(zom2);
	zombies.push_back(zom3);
	zombies.push_back(zom4);
	grid[1][1] = ZOMBIE; // place it on the map	
	grid[SIZEY - 2][1] = ZOMBIE;
	grid[1][SIZEX - 2] = ZOMBIE;
	grid[SIZEY - 2][SIZEX - 2] = ZOMBIE;
}

void setSpotInitialCoordinates(Item& spot)
{
	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
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

void placeSpot(char gr[][SIZEX], Item spot)
{ //place spot at its new position in grid
	gr[spot.y][spot.x] = spot.symbol;
}

void updateGrid(char grid[][SIZEX], Item spot, vector<zombie> zombies, vector<pill> pills, vector<Item> holes)
{
	void setGrid(char[][SIZEX]);
	void placeSpot(char g[][SIZEX], Item spot);
	void placezombies(char g[][SIZEX], vector<zombie> zombies);
	void placepill(char g[][SIZEX], vector<pill> pills);
	void placeitem(char g[][SIZEX], vector<Item> holes);

	setGrid(grid);	         //reset empty grid
	placeSpot(grid, spot);	 //set spot in grid
	placezombies(grid, zombies); //set zombies on map
	placepill(grid, pills);      //set pills on map
	placeitem(grid, holes); // set the holes on the grid
}

void placepill(char g[][SIZEX], vector<pill> pills)
{
	for (pill item : pills)
	{
		g[item.baseobject.y][item.baseobject.x] = item.baseobject.symbol;
	}
}

void placeitem(char g[][SIZEX], vector<Item> holes)
{
	for (Item it : holes)
	{
		g[it.y][it.x] = it.symbol;
	}
}

void placezombies(char g[][SIZEX], vector<zombie> zombies)
{
	for (zombie item : zombies)
	{
		g[item.baseobject.y][item.baseobject.x] = item.baseobject.symbol;
	}
}

void updateSpotCoordinates(const char g[][SIZEX], player& sp, int key, string& mess)
{
	void setKeyDirection(int k, int& dx, int& dy);

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
		break;
	case WALL:        //hit a wall and stay there
		cout << '\a'; //beep the alarm
		switch (key)
		{
		case UP:
			sp.baseobject.y = SIZEY-2;
			break;
		case DOWN:
			sp.baseobject.y = 1;
			break;
		case LEFT:
			sp.baseobject.x = SIZEX-2;
			break;
		case RIGHT:
			sp.baseobject.x = 1;
			break;
		}
		mess = "CANNOT GO THERE!    ";
		break;
	case ZOMBIE:
		sp.lives--;
		break;
	case HOLE:
		sp.lives--;
		break;
	case PILL:
		sp.lives++;
		break;
	}
}

void setKeyDirection(int key, int& dx, int& dy)
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
		break;
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

bool isArrowKey(int key)
{
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
}

bool isCheatKey(int key)
{
	return ((key == EAT) || (key == EXTERMINATE) || (key == FREEZ));
}

bool wantToQuit(int key, string& message)
{
	bool exit = (key == QUIT || toupper(key) == QUIT);
	if (exit)
	    message = "you have quit";
	return exit;
}

bool haswon(const char gd[][SIZEX], string& message)
{
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col) 
		{
			if (gd[row][col] == ZOMBIE || gd[row][col] == PILL)
				return false;				  // some may not like this
		}
	}
	message = "there are no zombies or pils on the map";
	return true;
}
bool endconditions(char grid[][SIZEX], player spot, int key, string& message)
{
	bool haswon(const char grid[][SIZEX], string& message);
	bool haslost(player spot, string& message);
	bool wantToQuit(int k, string& message);
	return (!wantToQuit(key, message) && (!haswon(grid, message) && !haslost(spot, message)));
}

bool haslost(player spot, string& message)
{
	if (spot.lives == 0)
	{
		message = "you have no lives";
		return true;
	}
	else
	{
		return false;
	}
}

bool ocupiedpeace(const char gd[][SIZEX], int x, int y)
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

void renderGame(const char gd[][SIZEX], string mess, player spot, int zombielives, int remainingpill)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(const char g[][SIZEX]);
	void showLives(const player spot);
	void showzomLives(const int lives);
	void showrempill(const int pils);
	void showTitle();
	void showOptions();
	void showMessage(string);

	Gotoxy(0, 0);
	//display grid contents
	paintGrid(gd);
	//display game title
	showTitle();
	showLives(spot);
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
	SelectTextColour(clWhite);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			cout << g[row][col];              //output cell content
		} //end of col-loop
		cout << endl;
	} //end of row-loop
}
void showzomLives(const int lives)
{

}
void showrempill(const int pils)
{

}
void showTitle()
{ //display game title
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "___ZOMBIES GAME SKELETON___\n" << endl;
	SelectBackColour(clWhite);
	SelectTextColour(clRed);
	Gotoxy(40, 0);
	cout << "Pascale Vacher: March 15";
}

void showOptions()
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 5);
	cout << "TO MOVE USE KEYBOARD ARROWS  ";
	Gotoxy(40, 6);
	cout << "TO QUIT ENTER 'Q'   ";
}
void showLives(player spot)
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 7);
	cout << spot.lives << " lives left";
}

void showMessage(string m)
{ //print auxiliary messages if any
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	cout << m;	//display current message
}

void endProgram()
{ //end program with appropriate message
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	Gotoxy(40, 8);
	cout << "PLAYER QUITS!          ";
	//hold output screen until a keyboard key is hit
	Gotoxy(40, 9);
	system("pause");
}
