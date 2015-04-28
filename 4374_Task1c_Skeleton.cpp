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
const char INFO('I');		//info key
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
	bool alive;				 // variable to set the zombies being dead or not
	bool hidden;			 // variable to set whether the zombies have been hidden
	zombie operator= (const zombie& it)
	{
		zombie a = it;
		return a;
	}
};

struct pill {
	Item baseobject;         // the base class of all objects on the map
	bool eaten;				 // variable that determines if the pill had been eaten or not
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
	int levelSelection = 0;
	int  getKeyPress();
	bool endconditions(vector<zombie>& zombies, const int pills, const player &spot, const int key, string& message);
	void ApplyCheat(const int key, player& spot, vector<zombie>& zombies, vector<pill>& pills);
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
	void loadgame(player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes);
	void saveboard(vector<replay>& replayer, const char grid[][SIZEX]);
	bool canload(const string& name);
	void setGrid(char[][SIZEX]);
	//These are all the functions we call in our main body of code, they all pass different paramters
	char grid[SIZEY][SIZEX];					//grid for display
	string message("LET'S START...      ");		//current message to player
	int key(' ');								//declares the input key
	vector<replay> replayer;					//creates a list of moves to replay
	player spot = { SPOT, 0, 0, mainloop(levelSelection), 5 };     //creates the player based on what level and name they choose
	bool loadgames = canload(spot.name);		//determines if that can load a current game or not
	spot.levelChoice = levelSelection;			//this sets the level that is selected in the main loop
	Clrscr();									//this clears the screan
	int hours, min, seconds;					//sets up the current time
	GetSystemTime(hours, min, seconds);			//gets the current time on the system
	do {
		vector<zombie> zombies;					//initalize the zombies
		vector<pill> pills; 					//initalize pills
		vector<Item> holes; 					//initalize holes
		
		if (loadgames)							//if the player has a saved game this is called
		{
			setGrid(grid);						//this sets up the grid to load the game onto
			loadgame(spot, zombies, pills, holes); //this loads the game based on variables read in
			updateGame(grid, spot, key, message, zombies, pills, holes); // this then updates the menu screen
			loadgames = false;					//resets the value of loadgames to false so this isnt entered again
		}
		else
		{
			initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot etc)
		}
		renderGame(grid, message, spot, zombies.size(), pills.size(), 0); //this renders all the information on the screen (lives, pills left etc)
		do {
			if (_kbhit()) //if the keyboard has been hit this if statement enters
			{
				saveboard(replayer, grid);		//this adds the current state of the grid to the replay structure
			message = "                    ";	//reset message
			key = getKeyPress();				//read in next keyboard event
			if (isArrowKey(key))				//if this is an arrow key enter this
				updateGame(grid, spot, key, message, zombies, pills, holes); //this calls the update game function based on what button pressed
			else if (isCheatKey(key))			//if its a cheat character enter this
			{
				spot.hascheated = true;			//makes it so the player has cheated, so score isnt saved
				ApplyCheat(key, spot, zombies, pills); //calls the function to actually apply the cheat
				updateGame(grid, spot, key, message, zombies, pills, holes); //calls the function to update the game
			}
				else if (issaveKey(key))		//if its the save game key
					savegame(spot, zombies, pills, holes); //this calls the save game function to save to a file
				else if (isloadKey(key))		//if its the load key
				{
					loadgame(spot, zombies, pills, holes); //calls the load game function then updates the game to apply load
					updateGame(grid, spot, key, message, zombies, pills, holes);
				}					
				else if (isreplayKey(key))		//if its the replay key
					displayallmoves(replayer);	//replays all moves up till the point pressed
			}
			int nhours, nmin, nseconds;			//declares the time to track
			GetSystemTime(nhours, nmin, nseconds); //gets the new time after this has passed
			const int diff = (((nhours - hours) * 3600) + ((nmin - min) * 60) + (nseconds - seconds)); //gets the time based on the difference
			//of the two times
			renderGame(grid, message, spot, zombies.size(), getsize(pills), diff);        //render game state on screen
		} while (!wantToQuit(key, message) && (!haswon(zombies, message, spot) && !haslost(spot, message))); 
		//while the user doesn't want to quit, hasn't lost or won this will run
		spot.levelChoice++;						// after they have finished one level it will increase their lifes
		spot.isProtected = false;				// resets the protection
	} while (spot.levelChoice <= 3 && !wantToQuit(key, message) && !haslost(spot, message));
	//this while loop runs until they reach the end of the game or quit or lose
	if (!spot.hascheated)						//this is called if spot cheats
	{
		if (!readsavedcore(spot.name, spot.lives)) //checks if there is a current score
			savescore(spot.name, spot.lives);      //if there isn't this is called to save the score
		updatescore(spot.name, spot.lives);		   //if not this is called to update it if the new score is higher
	}
	endProgram(message);                             //display final message
}
bool canload(const string& name)	
{
	int  getKeyPress();
	ifstream reader(name + ".save");				//this starts a reader
	if (reader.fail())								//if it cant a read a saved file from the name
		return false;
	else
	{
		cout << "save file avalible press l to continue";	
		cout << "from save any other key is no";	//asks the user if they want to load the saved game or start a new one
		int key = getKeyPress();					//gets their input
		if (toupper(key) == LOAD)					//determines their answer
			return true;
		else
			return false;
	}
}
bool isreplayKey(const int key)
{
	return (toupper(key) == REPLAY);				//returns true if replay key
}
bool issaveKey(const int k)
{
	if (toupper(k) == SAVE)							//returns true if save key
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
	//all the menu functions defined here
	int index = 0;
	char key = ' ';
	//sets up the index an key
	Clrscr();										//clears the screen
	while (index != replayer.size())				//whilst there is still moves to show run this loop
	{
		showDescription();
		showTitle();
		showOptions();
		showmenu();
		showtime();
		//call all display functions
		stringstream a;								//creates a stringstream to display the amount of moves
		a << "displaying move " << index << " of " << replayer.size();
		showMessage(a.str());						// displays the above message;
		paintGrid(replayer[index].grid);			//this calls the paint grid function with the grid at that point in the list
		index++;									//increments index
	}
	Clrscr();
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
	//gets the size of the pills based on what is eaten
}
void savegame(const player &spot, const vector<zombie> &zombies, const vector<pill> &pills, const vector<Item> &holes)
{
	ofstream writer(spot.name + ".save");			//starts up a writer to a file
	writer << spot.baseobject.x << endl;
	writer << spot.baseobject.y << endl;
	writer << spot.hascheated << endl;
	writer << spot.isProtected << endl;
	writer << spot.lives << endl;
	writer << spot.score << endl;
	writer << spot.levelChoice << endl;
	writer << spot.protectedCount << endl;
	//writes all of spots features to the file
	writer << zombies.size() << endl;				//gets the amount of zombies
	for (zombie a : zombies)						//runs for all zombies
	{
		writer << a.baseobject.x << endl;
		writer << a.baseobject.y << endl;
		writer << a.imobalized << endl;
		writer << a.startx << endl;
		writer << a.starty << endl;
		writer << a.alive << endl;
		writer << a.hidden << endl;
		//writes all zombie features to the file
	}
	writer << pills.size() << endl;					//runs for all pills
	for (pill a : pills)
	{
		writer << a.baseobject.x << endl;
		writer << a.baseobject.y << endl;
		writer << a.eaten << endl;
		//writes all pill features to the file
	}
	writer << holes.size() << endl;					//runs for all holes
	for (Item a : holes)
	{
		writer << a.x << endl;
		writer << a.y << endl;
		//writes all hole features to the file
	}
	writer.close();									//closes the file
}
void saveboard(vector<replay>& replayer, const char grid[][SIZEX])
{
	replay newstep;
	for (int row(0); row < SIZEY; ++row) //for each column
	{
		for (int col(0); col < SIZEX; ++col) //for each col
		{
			newstep.grid[row][col] = grid[row][col]; // save the board
		}
	}
	replayer.push_back(newstep);
}
void loadgame(player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes)
{
	ifstream reader(spot.name + ".save");			//creates a reader for the program
	if (reader.fail())
		cout << "Error: no save avalible";			//if it fails output this message
	else
	{
		zombies.clear();
		pills.clear();
		holes.clear();
		//clears all the items so new ones can replace them
		reader >> spot.baseobject.x;
		reader >> spot.baseobject.y;
		reader >> spot.hascheated;
		reader >> spot.isProtected;
		reader >> spot.lives;
		reader >> spot.score;
		reader >> spot.levelChoice;
		reader >> spot.protectedCount;
		//reads in all of spots features
		int numofzom;
		reader >> numofzom;
		//gets the amount of zombies
		for (int i = 0; i != numofzom; i++)
		{
			zombie a = { ZOMBIE };
			reader >> a.baseobject.x;
			reader >> a.baseobject.y;
			reader >> a.imobalized;
			reader >> a.startx;
			reader >> a.starty;
			reader >> a.alive;
			reader >> a.hidden;
			zombies.push_back(a);
			//adds all the zombies to the list
		}
		reader >> numofzom;
		for (int i = 0; i != numofzom; i++)
		{
			pill a = { PILL };
			reader >> a.baseobject.x;
			reader >> a.baseobject.y;
			reader >> a.eaten;
			pills.push_back(a);
			//adds all the pills to the list
		}
		reader >> numofzom;
		for (int i = 0; i != numofzom; i++)
		{
			Item a = { HOLE };
			reader >> a.x;
			reader >> a.y;
			holes.push_back(a);
			//adds all the zombies to the list
		}
		reader.close();
		//closes the reader
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
	//all the functions going to be used in this part of the code
	string name = "";
	char key = ' ';
	//sets up some variables
	while (toupper(key) != PLAY)						//whilst they havent clicked play yet
	{
		showTitle();
		showgametitle();
		showOptions();
		showtime();
		showmenu();
		//displays all the menu items
		if (_kbhit())						
		{
			key = getKeyPress();
			if (toupper(key) == INFO)
				showDescription();
			//if the hit info it displays the game descriptions
			else if (toupper(key) == LEADERBOARD)
				displayhighscores();
			//if they click leaderboard it opens the leaderboard
			else if (toupper(key) == QUIT)
				return 0;
			//if they click quit it closes the game
			else if (toupper(key) != PLAY)
			{
				SelectBackColour(clRed);
				SelectTextColour(clYellow);
				Gotoxy(40, 13);
				cout << "INVALID KEY!  ";
				//if a different key is pressed an error message appears
			}
		}
	}
	requestname();
	cin >> name;
	//this calls the function to request the name and then gets it as an input
	clearMessage();
	while(levelSelection < 1 || levelSelection > 3)	//code runs until the user enters a correct level
	{
		getLevel();
		cin >> levelSelection;
		if (levelSelection < 0 || levelSelection > 3)
			cout << "Error: please enter a correct level";
		//this runs until the user enters a correct value
	}
	
	//this calls the function to ask for the level chosen an
	clearMessage();
	int previousscore = getscore(name); //this gets the score for the user
	showscore(previousscore);//this shows the score
	return name;
}

void savescore(const string &name, const int score)
{
	ofstream out(name + ".scr"); //creates a stream
	if (!out.fail())
		out << score; //returns the score based on the name
	out.close();
}

bool readsavedcore(const string &name, const int score)
{
	ifstream in(name + ".scr");//creates a stream
	if (!in.fail())// the file may not be found
	{
		int storedscore;
		in >> storedscore;
		if (storedscore > score) //if the score they got is greater than the one atm it returns true
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
		in >> storedscore;	//gets the score
		return storedscore;
	}
	in.close();
	return -1; //returns -1 if not previous score
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
	//two functions used
	for (int i = 0; i < zombies.size(); i++)
	{
		if (zombies[i].imobalized == false && zombies[i].hidden == false && zombies[i].alive == true)
		{
			//calculate direction of movement required by key - if any
			int dx(zombies[i].baseobject.x), dy(zombies[i].baseobject.y);
			if (!spot.isProtected) //if spot is protected the zombies move differently
			{
				getrandommove(spot, dx, dy); //gets the move as regular
			}
			else
			{
				retreat(spot, dx, dy); //moves the zombie away from spot if is protected
			}
			const int targetY(zombies[i].baseobject.y + dy);
			const int targetX(zombies[i].baseobject.x + dx);
			//gets the target x and y
			switch (g[targetY][targetX])
			{		//...depending on what's on the target position in grid...
			case PILL:
			case TUNNEL:      //can move
				zombies[i].baseobject.y += dy;   //go in that Y direction
				zombies[i].baseobject.x += dx;   //go in that X direction
				break;
			case SPOT:// zombie hits spot
				spot.lives--;
				zombies[i].baseobject.x = zombies[i].startx;
				zombies[i].baseobject.y = zombies[i].starty;
				//decreases lives and respawns
				break;
			case ZOMBIE:
				zombies[i].baseobject.x = zombies[i].startx;
				zombies[i].baseobject.y = zombies[i].starty;
				//respawns zombie
				for (zombie& item : zombies)
				{
					if (item.baseobject.x == targetX && item.baseobject.y == targetY)
					{
						item.baseobject.x = item.startx;
						item.baseobject.y = item.starty;
						//determines what zombie was hit and respawns it
					}
				}
				break;
			case HOLE://remove the zombie from map
				zombies[i].alive = false;
				//sets zombie to dead
			}
		}
	}
}

void ApplyCheat(const int key, player& spot, vector<zombie>& zombies, vector<pill>& pills)
{
	if (toupper(key) == EAT)//remove all pils from the grid
	{
		int livesGained = 0;
		for (int i = 0; i < pills.size(); i++)
		{
			if (pills[i].eaten == true);
				livesGained++; //calculates the lives gained from all the bills 
		}
		spot.lives = spot.lives + livesGained; //adds this number to the total life
		pills.clear(); //clears all the pills from the game
	}
	else if (toupper(key) == EXTERMINATE)//hides all the zombies on the board
	{
		for (int i = 0; i != zombies.size(); i++)
		{
			zombies[i].hidden = !zombies[i].hidden; //sets to hidden if not and vice versa
			zombies[i].baseobject.x = zombies[i].startx;
			zombies[i].baseobject.y = zombies[i].starty;
			//sets all zombies to their spawn points
		}
	}
	else if (toupper(key) == FREEZ)// do nothing when it is the zombies turn to move
		for (int i = 0; i != zombies.size(); i++)
			zombies[i].imobalized = !zombies[i].imobalized;
		//imobalizes all the zombies if they aren't and vice versa
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
	//gets the move of the zombie based on where spot is
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
	//gets the move of the zombie based on where spot is
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
	//all functions used to set up game

	Seed();                            //seed reandom number generator
	setGrid(grid);                     //reset empty grid
	placewallonmap(grid);
	placezombiesonmap(grid, zombies);  // place the zombies on the map
	setSpotInitialCoordinates(grid, spot.baseobject);//initialise spot position
	placeSpot(grid, spot.baseobject);  //set spot in grid
	switch (spot.levelChoice) //decides which level is picked
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
	//this variates the amount of lives based on level chosen
	placepillonmap(grid, pills, spot);	   // place pills on the map
	placeholeonmap(grid, holes, spot);       // place holes on the map
}
void getLevel()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 15);
	cout << "please select a level: ";
	//prints out this message
}

void placepillonmap(char grid[][SIZEX], vector<pill>& pills, player& spot)
{
	void occupyPills(int numberOfPills, char grid[][SIZEX], vector<pill>& pills);
	int selection;
	//creates a new function to occupy pills on the map based on an int value
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
	//calls occupy pills and passes what number of pills to add to grid
}

void occupyPills(int numberOfPills, char grid[][SIZEX], vector<pill>& pills)
{
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y);
	for (int i = 0; i < numberOfPills; i++) // places pills on the map
	{
		int x = Random(SIZEX - 2); // gets the first coordinates
		int y = Random(SIZEY - 2); 
		while (ocupiedpeace(grid, x, y))//checks if the space is free
		{
			Seed();
			x = Random(SIZEX - 2); // get new coordinates
			y = Random(SIZEY - 2); // 
		}
		pill pilla = { PILL, x, y };
		pills.push_back(pilla); //creates the pill in the list
		grid[y][x] = PILL; // place it on the map	
	}
}

void placeholeonmap(char grid[][SIZEX], vector<Item>& holes, player& spot)
{
	void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles);
	int selection;
	//creates a new function to occupy holes on the map based on an int value
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
	//calls occupy holes and passes what number of holes to add to grid
}

void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles)
{
	void occupyHoles(char grid[][SIZEX], vector<Item>& holes, int numberOfHoles);
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y);
	for (int i = 0; i < numberOfHoles; i++) // places holes on the map
	{
		int x = Random(SIZEX - 2); //original coordinates
		int y = Random(SIZEY - 2); // 
		while (ocupiedpeace(grid, x, y)) //checks if the space is free
		{
			Seed();
			x = Random(SIZEX - 2); // get new coordinates
			y = Random(SIZEY - 2); // 
		}
		Item hole = { HOLE, x, y };
		grid[y][x] = HOLE; //places hole on the grid
		holes.push_back(hole);//adds hole to the list
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
	//this is hard code to set the inner walls inside the game
}

void placezombiesonmap(char grid[][SIZEX], vector<zombie>& zombies)
{
	const zombie zom1 = { ZOMBIE, 1, 1, 1, 1, false, true }; // {{item}, startx, starty, imobilzed, alive}
	const zombie zom2 = { ZOMBIE, SIZEX - 2, 1, SIZEX - 2, 1, false, true };
	const zombie zom3 = { ZOMBIE, 1, SIZEY - 2, 1, SIZEY - 2, false, true };
	const zombie zom4 = { ZOMBIE, SIZEX - 2, SIZEY - 2, SIZEX - 2, SIZEY - 2, false, true };
	//declares all zombies
	zombies.push_back(zom1);
	zombies.push_back(zom2);
	zombies.push_back(zom3);
	zombies.push_back(zom4);
	//adds all zombies to the list
	grid[1][1] = ZOMBIE; 
	grid[SIZEY - 2][1] = ZOMBIE;
	grid[1][SIZEX - 2] = ZOMBIE;
	grid[SIZEY - 2][SIZEX - 2] = ZOMBIE;
	//places all zombies on the map
}

void setSpotInitialCoordinates(char grid[][SIZEX], Item& spot)
{
	bool ocupiedpeace(const char gd[][SIZEX], const int x, const int y); 
	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
	while (ocupiedpeace(grid, spot.x, spot.y))//checks if space is free
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
	//all functions needed

	setGrid(grid);	         //reset empty grid
	placewallonmap(grid); //places all the walls
	placezombies(grid, zombies); //set zombies on map
	placepill(grid, pills);      //set pills on map
	placeitem(grid, holes); // set the holes on the grid
	placeSpot(grid, spot);	 //set spot in grid
}

void placepill(char g[][SIZEX], const vector<pill> &pills)
{
	for (const pill& item : pills)
		if (!item.eaten) // places the pills if they havent been eaten
			g[item.baseobject.y][item.baseobject.x] = item.baseobject.symbol;
}

void placeitem(char g[][SIZEX], const vector<Item> &holes)
{
	for (const Item& it : holes)
		g[it.y][it.x] = it.symbol;	//places the hole on the map
}

void placezombies(char g[][SIZEX], const vector<zombie> &zombies)
{
	for (const zombie& item : zombies)
		if (item.alive == true && item.hidden == false) //if the zombie isn't hidden or dead place it on the grid
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
		if (sp.isProtected)      //if its protected and moves decrement the count
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
				if (zombies[i].baseobject.x == sp.baseobject.x && zombies[i].baseobject.y == sp.baseobject.y) 
					zombies[i].alive = false; 
			//when the player is protected this loop is called to see what zombie was hit and then that zombie is killed
			break;
		}
		else
			sp.lives--;
		//otherwise the player loses a live
		for (zombie& it : zombies)
		{
			if (sp.baseobject.x == it.baseobject.x && sp.baseobject.y == it.baseobject.y)
			{
				it.baseobject.x = it.startx;
				it.baseobject.y = it.starty;
				//this resets the position of the zombie
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
			if (pills[i].baseobject.x == sp.baseobject.x && pills[i].baseobject.y == sp.baseobject.y) 
				pills[i].eaten = true; 
		//this loop goes through all the pills and finds what one got hit and then declares it as eaten
		break;
	}
	if (sp.protectedCount == 0)
		sp.isProtected = false;
	//if the user is no longer protected the boolean is set to false
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
	//if the user quits send a message and return true
}

bool haswon(vector<zombie>& zombies, string& message, const player& spot)
{		
	if (zombies[0].alive == true || zombies[1].alive == true || zombies[2].alive == true || zombies[3].alive == true)
	{
		return false;
	}
	// if any zombies are alive return false
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 16);
	cout << "Congratulations, you have finished this level!";
	Gotoxy(40, 17);
	cout << "Your score is: " << spot.lives;
	return true;
	//displays a message if not
}


bool haslost(const player &spot, string& message)
{
	if (spot.lives == 0)
	{
		message = "you have no lives";
		return true;
	//if the user has no lives the message is shown and true is returned back
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
	//this functions determines if there's a space free on the grid
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
	void showrempill(const int pils);
	void showTitle();
	void showOptions();
	void showtime();
	void showSaveLoad();
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
	showSaveLoad();
	showLives(spot);
	showname(spot.name);
	int previousscore = getscore(spot.name);
	showscore(previousscore);
	//show number of zombie lives
	//show number of remaing pills
	showrempill(remainingpill);
	//display menu options available
	showOptions();
	//display message if any
	showMessage(mess);
}

void paintGrid(const char g[][SIZEX])
{
	SelectBackColour(clRed);
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
			//adds all the pills and their colours
			cout << g[row][col];              //output cell content
		} //end of col-loop
		cout << endl;
	} //end of row-loop
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
	SelectBackColour(clRed);
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

void showSaveLoad()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 11);
	cout << "Press S to save your game";
	Gotoxy(40, 12);
	cout << "Press L to load your game";
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
	Gotoxy(40, 19);
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
	Gotoxy(40, 20);
	cout << "player score: " << score;

}

void showtime()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 14);
	cout << GetDate();
	Gotoxy(40, 15);
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
		//declares all variables
		in.close();
		ofstream out("best.scr"); //creates a stream
		if (score1 < score)
		{
			score3 = score2;
			score2 = score1;
			name3 = name2;
			name2 = name1;
			score1 = score;
			name1 = name;
			//if the score is greater than the first entry shift all the scores
		}
		else if (score2 < score)
		{
			score3 = score2;
			name3 = name2;
			score2 = score;
			name2 = name;
			//if the score is greater than the second entry shift all the scores
		}
		else if (score3 < score)
		{
			score3 = score;
			name3 = name;
			//if the score is greater than the third entry set the scores
		}
		out << name1 << endl;

		out << score1 << endl;
		out << name2 << endl;
		out << score2 << endl;
		out << name3 << endl;
		out << score3 << endl;
		//output all of the scores
		out.close();
	}

}
	void showdiff(int diff)
	{
		SelectBackColour(clRed);
		SelectTextColour(clYellow);
		Gotoxy(40, 16);
		cout << "time spent in game: " << diff;
		//shows the time spent
	}

