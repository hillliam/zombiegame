//SKELETON PROGRAM
//---------------------------------
#include <iostream >         
#include <iomanip>           
#include <conio.h>           
#include <string>        
#include <vector>
using namespace std;

//include our own libraries
#include "RandomUtils.h"
#include "ConsoleUtils.h"

//---------------------------------
//define constants
//---------------------------------
//define global constants
//defining the size of the grid
const int SIZEY(12);         //vertical dimension
const int SIZEX(20);         //horizontal dimension
//defining symbols used for display of the grid and content
const char SPOT('@');        //spot
const char TUNNEL(' ');      //open space
const char WALL('#');        //border
const char HOLE('O');        //hole
const char ZOMBIE('Z');      //zombie
const char PILL('.');        //pill (used in basic version insted of structure)
//defining the command letters to move the blob on the maze
const int  UP(72);           //up arrow
const int  DOWN(80);         //down arrow
const int  RIGHT(77);        //right arrow
const int  LEFT(75);         //left arrow
//defining the cheat command letters
const char FREEZ('F');        //stop the zombies moving
const char EXTERMINATE('X');  //remove all zombies
const char EAT('E');         //remove all pills
//defining the other command letters
const char QUIT('Q');        //end the game

//base data structure to store data for a grid item
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
	bool imobalized;		 // set true if the zombie cant move
};
struct pill {
	Item baseobject;         // the base class of all objects on the map
	bool eaten;				 // set true if the will not be displayed
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie> zombies, vector<Item> holes, vector<pill> pills);
	bool wantToQuit(int k);
	bool isArrowKey(int k);
	bool isCheatKey(int k);
	bool haswon(const char grid[][SIZEX]);
	bool haslost(player spot);
	int  getKeyPress();
	void ApplyCheat(char grid[][SIZEX], player& spot, int key);
	void updateGame(char g[][SIZEX], player& sp, int k, string& mess);
	void renderGame(const char g[][SIZEX], string mess);
	void endProgram();

	//local variable declarations 
	char grid[SIZEY][SIZEX];                //grid for display
	player spot = { SPOT };                 //Spot's symbol and position (0, 0) 
	vector<zombie> zombies;	// initalize the 4 zombies
	vector<pill> pills; // initalize avalible pills to 8
	vector<Item> holes; // 12 holes
	string message("LET'S START...      "); //current message to player

	//action...
	initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot)
	int key(' ');                         //create key to store keyboard events 
	do {
		renderGame(grid, message);        //render game state on screen
		message = "                    "; //reset message
		key = getKeyPress();              //read in next keyboard event
		if (isArrowKey(key))
			updateGame(grid, spot, key, message);
		else if (isCheatKey(key))
			ApplyCheat(grid, spot, key);
		else
			message = "INVALID KEY!        "; //set 'Invalid key' message
	} while (!wantToQuit(key) || !haswon(grid) || haslost(spot));               //while user does not want to quit
	endProgram();                             //display final message
	return 0;
} //end main

void updateGame(char grid[][SIZEX], player& spot, int key, string& message)
{ //updateGame state
	void updateSpotCoordinates(const char g[][SIZEX], player& spot, int key, string& mess);
	void updatezombieCoordinates(const char g[][SIZEX], vector<zombie> zombies); // zombie move
	void updateGrid(char g[][SIZEX], Item spot);

	updateSpotCoordinates(grid, spot, key, message);    //update spot coordinates
                                                        //according to key
	
	updateGrid(grid, spot.baseobject);                  //update grid information
}

void ApplyCheat(char grid[][SIZEX], player& spot, int key)
{
	if (key == EAT)//remove all pils from the grid
		cout << "replace me";
	else if (key == EXTERMINATE)//remove all zombies from board
		cout << "replace me";
	else if (key == FREEZ)// do nothing when it is the zombies turn to move
		cout << "replace me";
}
 
//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie> zombies, vector<Item> holes, vector<pill> pills)
{ //initialise grid and place spot in middle
	void setGrid(char[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot);
	void placeSpot(char gr[][SIZEX], Item spot);
	void placepillonmap(char grid[][SIZEX], vector<pill> pills);
	void placeholeonmap(char grid[][SIZEX], vector<Item> holes);
	void placezombiesonmap(char grid[][SIZEX], vector<zombie> zombies);

	Seed();                            //seed reandom number generator
	setSpotInitialCoordinates(spot.baseobject);//initialise spot position
	setGrid(grid);                     //reset empty grid
	placeSpot(grid, spot.baseobject);  //set spot in grid
	placepillonmap(grid, pills);	   // place pills on the map
	placeholeonmap(grid, holes);       // place holes on the map
	placezombiesonmap(grid, zombies);  // place the zombies on the map
}

void placepillonmap(char grid[][SIZEX], vector<pill> pills)
{
	for (int i = 0; i != 8; i++) // place 8 pills on the map
	{
		int x = Random(SIZEX - 1); //
		int y = Random(SIZEY - 1); // 
		while (grid[x][y] != TUNNEL)
		{
			Seed();
			int x = Random(SIZEX - 1); // get new chordinates
			int y = Random(SIZEY - 1); // 
		}
		pill pilla = { PILL, x, y };
		pills.push_back(pilla);
		grid[x][y] = PILL; // place it on the map	
	}
}

void placeholeonmap(char grid[][SIZEX], vector<Item> holes)
{
	for (int i = 0; i != 12; i++) // place 12 holes on the map
	{
		int x = Random(SIZEX - 1); // 
		int y = Random(SIZEY - 1); // 
		while (grid[x][y] != TUNNEL)
		{
			Seed();
			int x = Random(SIZEX-1); // get new chordinates
			int y = Random(SIZEY-1); // 
		}
		Item hole = { HOLE, x, y };
		grid[x][y] = HOLE;
		holes.push_back(hole);
	}
}

void placezombiesonmap(char grid[][SIZEX], vector<zombie> zombies)
{
	for (int i = 0; i != 4; i++) // place 8 pills on the map
	{
		int x = Random(SIZEX - 1); //
		int y = Random(SIZEY - 1); // 
		while (grid[x][y] != TUNNEL)
		{
			Seed();
			int x = Random(SIZEX - 1); // get new chordinates
			int y = Random(SIZEY - 1); // 
		}
		zombie zom = { ZOMBIE, x, y };
		zombies.push_back(zom);
		grid[x][y] = PILL; // place it on the map	
	}
}

void setSpotInitialCoordinates(Item& spot)
{ //set spot coordinates inside the grid at random at beginning of game
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

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], Item spot, vector<zombie> zombies, vector<pill> pills, vector<Item> holes)
{ //update grid configuration after each move
	void setGrid(char[][SIZEX]);
	void placeSpot(char g[][SIZEX], Item spot);
	void placezombies(char g[][SIZEX], vector<zombie> zombies);
	void placepill(char g[][SIZEX], vector<pill> pills);
	void placeitem(char g[][SIZEX], vector<Item> holes);

	setGrid(grid);	         //reset empty grid
	placeSpot(grid, spot);	 //set spot in grid
}

void placepill(char g[][SIZEX], vector<pill> pills)
{
	for (int i = 0; i != pills.size(); i++)
	{
		g[pills[i].baseobject.x][pills[i].baseobject.y] = pills[i].baseobject.symbol;
	}
}
void placeitem(char g[][SIZEX], vector<Item> holes)
{
	for (int i = 0; i != holes.size(); i++)
	{
		g[holes[i].x][holes[i].y] = holes[i].symbol;
	}
}
void placezombies(char g[][SIZEX], vector<zombie> zombies)
{
	for (int i = 0; i != zombies.size(); i++)
	{
		g[zombies[i].baseobject.x][zombies[i].baseobject.y] = zombies[i].baseobject.symbol;
	}
}

//---------------------------------------------------------------------------
//----- move the spot
//---------------------------------------------------------------------------
void updateSpotCoordinates(const char g[][SIZEX], player& sp, int key, string& mess)
{ //move spot in required direction
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
			sp.baseobject.y = 10;
			break;
		case DOWN:
			sp.baseobject.y = 1;
			break;
		case LEFT:
			sp.baseobject.x = 10;
			break;
		case RIGHT:
			sp.baseobject.x = 1;
			break;
		}
		mess = "CANNOT GO THERE!    ";
		break;
	}
} //end of updateSpotCoordinates

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(int key, int& dx, int& dy)
{ //
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
{ //get key or command selected by user
	int keyPressed;
	keyPressed = getch();      //read in the selected arrow key or command letter
	while (keyPressed == 224)     //ignore symbol following cursor key
		keyPressed = getch();
	return(keyPressed);   
} //end of getKeyPress

bool isArrowKey(int key)
{ //check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
} //end of isArrowKey

bool isCheatKey(int key)
{ //check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return ((key == EAT) || (key == EXTERMINATE) || (key == FREEZ));
} //end of isCheatKey

bool wantToQuit(int key)
{ //check if the key pressed is 'Q'
	return (key == QUIT || toupper(key) == QUIT);
} //end of wantToQuit

bool haswon(const char gd[][SIZEX])
{// check for any zombies on the map and pills
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			if (gd[row][col] == ZOMBIE || gd[row][col] == PILL)
				return false;				  // some may not like this
		}
	}
	return true;
}

bool haslost(player spot)
{
	if (spot.lives == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------
void clearMessage()
{ //clear message area on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	string str(20, ' ');
	cout << str;  //display blank message

} //end of setMessage

void renderGame(const char gd[][SIZEX], string mess)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(const char g[][SIZEX]);
	void showTitle();
	void showOptions();
	void showMessage(string);

	Gotoxy(0, 0);
	//display grid contents
	paintGrid(gd);
	//display game title
	showTitle();
	//display menu options available
	showOptions();
	//display message if any
	showMessage(mess);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
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
