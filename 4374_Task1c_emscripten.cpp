#include <iostream>
#include <iomanip>
//#include <conio.h>
#include <string>
#include <vector>
#include <fstream>
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

//#include "RandomUtils.h"
//#include "ConsoleUtils.h"
//#include "TimeUtils.h"

using namespace std;

const int SIZEY(12);         //vertical dimension
const int SIZEX(20);         //horizontal dimension

const int screenhight(500);
const int screenwith(500);
const int pixels(32);

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
	int totalscore;			 // keeps track of the total score for the leaderboard
	bool hascheated;		 // set true if the user has cheated
	bool isProtected;		 // boolean variable to check if the player is protected by magic pill
	int protectedCount;		 // sets the amount moves the player can make before their protection goes
	int levelChoice;		 // sets the level choice 
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

void Seed()
{
}

int Random(int max) {
	return (rand() % max) + 1;
}

string GetDate()
{

}

string GetTime()
{

}

void GetSystemTime(int& hrs, int& mins, int& secs)
{

}

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
	void renderGame(const char g[][SIZEX], const string &mess, const player &spot, const int zomlives, const int remaingpills, const int diff, SDL_Surface* display, TTF_Font *font);
	void endProgram(const string &message);
	string mainloop(int& levelSelection, SDL_Surface* display, TTF_Font *font);
	void savescore(const string &name, const int score);
	bool readsavedcore(const string &name, const int score);
	bool haswon(const vector<zombie>& zombies, const player &spot);
	bool haslost(const player &spot, string& message);
	bool wantToQuit(const int k, string& message);
	void updatescore(const string &name, const int score);
	bool issaveKey(const int k);
	bool isloadKey(const int k);
	bool isreplayKey(const int k);
	void displayallmoves(const vector<replay> &replayer, SDL_Surface* display, TTF_Font *font);
	void savegame(const player& spot, const vector<zombie>& zombies, const vector<pill>& pills, const vector<Item>& holes);
	void loadgame(player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes);
	void saveboard(vector<replay>& replayer, const char grid[][SIZEX]);
	bool canload(const string& name);
	void setGrid(char[][SIZEX]);
	bool setupdisplay(SDL_Surface* display);
	TTF_Font * setuptext();
	//These are all the functions we call in our main body of code, they all pass different paramters
	SDL_Surface* display;						// the screen
	setupdisplay(display);						// setup screen
	TTF_Font *font = setuptext();				// text system
	char grid[SIZEY][SIZEX];					//grid for display
	string message("LET'S START...      ");		//current message to player
	int key(' ');								//declares the input key
	vector<replay> replayer;					//creates a list of moves to replay
	player spot = { SPOT, 0, 0, mainloop(levelSelection, display, font), 5 };     //creates the player based on what level and name they choose
	bool loadgames = canload(spot.name);		//determines if that can load a current game or not
	spot.levelChoice = levelSelection;			//this sets the level that is selected in the main loop
	//Clrscr();									//this clears the screan
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
			initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot etc)
		int hours, min, seconds;					//sets up the current time
		GetSystemTime(hours, min, seconds);			//gets the current time on the system
		renderGame(grid, message, spot, zombies.size(), pills.size(), 0, display, font); //this renders all the information on the screen (lives, pills left etc)
		do {
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
					displayallmoves(replayer, display, font);	//replays all moves up till the point pressed
			int nhours, nmin, nseconds;			//declares the time to track
			GetSystemTime(nhours, nmin, nseconds); //gets the new time after this has passed
			const int diff = (((nhours - hours) * 3600) + ((nmin - min) * 60) + (nseconds - seconds)); //gets the time based on the difference
			//of the two times
			renderGame(grid, message, spot, zombies.size(), getsize(pills), diff, display, font);        //render game state on screen
		} while (!wantToQuit(key, message) && (!haswon(zombies, spot) && !haslost(spot, message)));      //while user does not want to quit
		spot.levelChoice++;
		spot.totalscore = spot.totalscore + spot.lives;
		spot.isProtected = false;
	} while (spot.levelChoice <= 3 && !wantToQuit(key, message) && !haslost(spot, message));
	//this while loop runs until they reach the end of the game or quit or lose
	if (!spot.hascheated)						//this is called if spot cheats
	{
		if (!readsavedcore(spot.name, spot.totalscore)) //checks if there is a current score
			savescore(spot.name, spot.totalscore);      //if there isn't this is called to save the score
		updatescore(spot.name, spot.totalscore);		   //if not this is called to update it if the new score is higher
	}
	endProgram(message);                             //display final message
}

bool canload(const string& name, SDL_Surface *image, TTF_Font *font)
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

void displayallmoves(const vector<replay> &replayer, SDL_Surface *image, TTF_Font *font)
{
	void paintGrid(const char g[][SIZEX], SDL_Surface *image, TTF_Font *font);
	int getKeyPress(SDL_Surface *image, TTF_Font *font);
	void showDescription(SDL_Surface *image, TTF_Font *font);
	void showTitle(SDL_Surface *image, TTF_Font *font);
	void showOptions(SDL_Surface *image, TTF_Font *font);
	void showmenu(SDL_Surface *image, TTF_Font *font);
	void showtime(SDL_Surface *image, TTF_Font *font);
	void showMessage(const string&, SDL_Surface *image, TTF_Font *font);
	//all the menu functions defined here
	int index = 0;
	char key = ' ';
	//sets up the index an key
	//Clrscr();										//clears the screen
	while (index != replayer.size())				//whilst there is still moves to show run this loop
	{
		showDescription(image, font);
		showTitle(image, font);
		showOptions(image, font);
		showmenu(image, font);
		showtime(image, font);
		//call all display functions
		stringstream a;								//creates a stringstream to display the amount of moves
		a << "displaying move " << index << " of " << replayer.size();
		showMessage(a.str(), image, font);						// displays the above message;
		paintGrid(replayer[index].grid, image, font);			//this calls the paint grid function with the grid at that point in the list
		index++;									//increments index
	}
	//Clrscr();
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
	writer << spot.totalscore << endl;
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
		reader >> spot.totalscore;
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

string mainloop(int& levelSelection, SDL_Surface *image, TTF_Font *font)
{
	void requestname(SDL_Surface *image, TTF_Font *font);
	void showTitle(SDL_Surface *image, TTF_Font *font);
	void showOptions(SDL_Surface *image, TTF_Font *font);
	void showmenu(SDL_Surface *image, TTF_Font *font);
	void showtime(SDL_Surface *image, TTF_Font *font);
	void showgametitle(SDL_Surface *image, TTF_Font *font);
	int getscore(const string&);
	int  getKeyPress(SDL_Surface *image, TTF_Font *font);
	void clearMessage(SDL_Surface *image, TTF_Font *font);
	void showscore(const int score, SDL_Surface *image, TTF_Font *font);
	void getLevel(SDL_Surface *image, TTF_Font *font);
	void displayhighscores(SDL_Surface *image, TTF_Font *font);
	void showDescription(SDL_Surface *image, TTF_Font *font);
	//all the functions going to be used in this part of the code
	string name = "";
	char key = ' ';
	name = "";
	levelSelection = 1;
	return name;
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
				getrandommove(spot, dx, dy); //gets the move as regular
			else
				retreat(spot, dx, dy); //moves the zombie away from spot if is protected
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
	else if (spot.baseobject.x < x)
		x = -1;
	else
		x = 0;
	if (spot.baseobject.y > y)
		y = 1;
	else if (spot.baseobject.y < y)
		y = -1;
	else
		y = 0;
	//gets the move of the zombie based on where spot is
}

void retreat(const player &spot, int& x, int& y)
{
	if (spot.baseobject.x > x)
		x = -1;
	else if (spot.baseobject.x < x)
		x = 1;
	else
		x = 0;
	if (spot.baseobject.y > y)
		y = -1;
	else if (spot.baseobject.y < y)
		y = 1;
	else
		y = 0;
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

void getLevel(SDL_Surface *image, TTF_Font *font)
{
	SDL_Color text_color = { 255, 0, 255 };
	SDL_Rect dstrect = { 40, 15 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, "please select a level: ", text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
	//prints out this message
}

void placepillonmap(char grid[][SIZEX], vector<pill>& pills, player& spot)
{
	void occupyPills(const int numberOfPills, char grid[][SIZEX], vector<pill>& pills);
	//creates a new function to occupy pills on the map based on an int value	
	switch (spot.levelChoice)
	{
	case 1:
		occupyPills(8, grid, pills);
		break;
	case 2:
		occupyPills(5, grid, pills);
		break;
	case 3:
		occupyPills(3, grid, pills);
		break;
	}
	//calls occupy pills and passes what number of pills to add to grid
}

void occupyPills(const int numberOfPills, char grid[][SIZEX], vector<pill>& pills)
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
	void occupyHoles(char grid[][SIZEX], vector<Item>& holes, const int numberOfHoles);
	//creates a new function to occupy holes on the map based on an int value
	switch (spot.levelChoice)
	{
	case 1:
		occupyHoles(grid, holes, 12);
		break;
	case 2:
		occupyHoles(grid, holes, 5);
		break;
	case 3:
		occupyHoles(grid, holes, 2);
		break;
	}
	//calls occupy holes and passes what number of holes to add to grid
}

void occupyHoles(char grid[][SIZEX], vector<Item>& holes,const int numberOfHoles)
{
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
{ // gets a single key press
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_KEYDOWN:
		return event.key.keysym.sym;
	}
	return 0;
}

bool isArrowKey(const int key)
{// checks if the key pressed is a direction key 
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
}

bool isCheatKey(const int key)
{// checks if the key pressed is a cheat key
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

bool haswon(const vector<zombie>& zombies, const player& spot, SDL_Surface *image, TTF_Font *font)
{		
	if (zombies[0].alive == true || zombies[1].alive == true || zombies[2].alive == true || zombies[3].alive == true)
	{
		return false;
	}
	// if any zombies are alive return false
	SDL_Color text_color = { 255, 0, 255 };
	SDL_Rect dstrect = { 40, 17 };
	stringstream a;
    SDL_Surface *text;
	if (spot.levelChoice < 3)
		a << "Congratulations, you have finished this level!";
	else
		a << "Congratulations, you have finished the game!" << endl << "Your score is: " << spot.totalscore;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
	return false;
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
{// removes a message that has being drawn to the screen
}

void renderGame(const char gd[][SIZEX], const string &mess, const player &spot, const int zombielives, const int remainingpill, const int diff, TTF_Font *font, SDL_Surface *image)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(const char g[][SIZEX],SDL_Surface *text, TTF_Font *font);
	void showLives(const player &spot,SDL_Surface *text, TTF_Font *font);
	void showDescription(SDL_Surface *text, TTF_Font *font);
	void showrempill(const int pils, SDL_Surface *text, TTF_Font *font);
	void showTitle(SDL_Surface *text, TTF_Font *font);
	void showOptions(SDL_Surface *text, TTF_Font *font);
	void showtime(SDL_Surface *text, TTF_Font *font);
	void showSaveLoad(SDL_Surface *text, TTF_Font *font);
	void showMessage(const string&, SDL_Surface *text, TTF_Font *font);
	void showname(const string &name, SDL_Surface *text, TTF_Font *font);
	void showdiff(const int diff, SDL_Surface *text, TTF_Font *font);

	//display grid contents
	paintGrid(gd, image, font);
	//display game title
	showTitle(image, font);
	showDescription(image, font);
	showdiff(diff, image, font);
	showtime(image, font);
	showSaveLoad(image, font);
	showLives(spot, image, font);
	showname(spot.name, image, font);
	//show number of remaing pills
	showrempill(remainingpill, image, font);
	//display menu options available
	showOptions(image, font);
	//display message if any
	showMessage(mess, image, font);
}

void paintGrid(const char g[][SIZEX], SDL_Surface *image, TTF_Font *font)
{
	SDL_Color text_color = { 255, 255, 255 }; // R,G,B
	SDL_Rect dstrect = { 0, 2 };
	stringstream a;
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			//adds all the pills and their colours
			a << g[row][col];              //output cell content
		} //end of col-loop
		a << endl;
	} //end of row-loop
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showrempill(const int pils, SDL_Surface *image, TTF_Font *font)
{// display the number of pills left on the board 
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 10 };
	stringstream a;
	a << "pills left: " << pils;
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showDescription(SDL_Surface *image, TTF_Font *font)
{// displays a description of the game during the main menu
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 2 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, "This is a game where you must escape/nthe zombies and survive. Pills mean/na life is gained./nContact with a hole(0) or zombie(Z)/nmeans a life is lost ", text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showTitle(SDL_Surface *image, TTF_Font *font)
{ //display game title
	SDL_Color text_color1 = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect1 = { 0, 0 };
	SDL_Surface *text1;
	text1 = TTF_RenderText_Solid(font, "___ZOMBIES GAME SKELETON___", text_color1);
	SDL_Color text_color2 = { 255, 0, 0 }; // R,G,B
	SDL_Rect dstrect = { 40, 0 };
	SDL_Surface *text2;
	text2 = TTF_RenderText_Solid(font, "Oliver Parker, Liam Hill, Alex Odgen/n1RR - COMPUTER SCIENCE", text_color2);
	SDL_BlitSurface(text2, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text2); // prevent mem leak
	SDL_BlitSurface(text1, NULL, image, dstrect1); // add text to framebuffer
	SDL_FreeSurface(text1); // prevent mem leak
}

void showSaveLoad(SDL_Surface *image, TTF_Font *font)
{//displays the save and load options during the game
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 11 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, "Press S to save your game/nPress L to load your game", text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showname(const string &name, SDL_Surface *image, TTF_Font *font)
{// display the players name during the game
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 13 };
	stringstream a;
	a << "your name: " << name;
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showOptions(SDL_Surface *image, TTF_Font *font)
{ //show game options
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect1 = { 40, 7 };
	SDL_Rect dstrect2 = { 40, 19 };
	SDL_Surface *text1;
	SDL_Surface *text2;
	text1 = TTF_RenderText_Solid(font, "TO MOVE USE KEYBOARD ARROWS  ", text_color);
	text2 = TTF_RenderText_Solid(font, "TO QUIT ENTER 'Q'   ", text_color);
	SDL_BlitSurface(text1, NULL, image, dstrect1); // add text to framebuffer
	SDL_FreeSurface(text1); // prevent mem leak
	SDL_BlitSurface(text2, NULL, image, dstrect2); // add text to framebuffer
	SDL_FreeSurface(text2); // prevent mem leak
}

void showLives(const player &spot, SDL_Surface *image, TTF_Font *font)
{ //show game options
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 9 };
	stringstream a;
	a << spot.lives << " lives left";
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showMessage(const string &m, SDL_Surface *image, TTF_Font *font)
{ //print auxiliary messages if any
	SDL_Color text_color = { 255, 255, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 8 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, m, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void endProgram(const string &message, SDL_Surface *image, TTF_Font *font)
{ //end program with appropriate message
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 8 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, message, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showmenu(SDL_Surface *image, TTF_Font *font)
{//shows the buttions the user can press
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 10 };
	SDL_Surface *text;
	stringstream a;
	a << "press p to play" << endl << "press i to get infomation" << endl << "press b to display leaderboard";
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showtime(SDL_Surface *image, TTF_Font *font)
{// shows the current date and time
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 40, 14 };
	stringstream a;
	SDL_Surface *text;
	a << GetDate()<< endl << GetTime();
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showgametitle(SDL_Surface *image, TTF_Font *font)
{// displays the title of the game before the board is drawn
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Rect dstrect = { 2, 4 };
	SDL_Surface *text;
	text = TTF_RenderText_Solid(font, "------------------------/n| SPOT AND ZOMBIE GAME |/n------------------------", text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
}

void showdiff(const int diff, SDL_Surface *image, TTF_Font *font)
{
	SDL_Rect dstrect = { 40, 16 };
	SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	SDL_Surface *text;
	stringstream a;
	a << "time spent in game: " << diff << endl;
	text = TTF_RenderText_Solid(font, a, text_color);
	SDL_BlitSurface(text, NULL, image, dstrect); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
	//shows the time spent
}

bool setupdisplay(SDL_Surface* display)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		cerr << "SDL_Init() Failed: " <<
			SDL_GetError() << endl;
		return false;
	}
	// Set the video mode
	display = SDL_SetVideoMode(screenhight, screenwith, pixels, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (display == NULL)
	{
		cerr << "SDL_SetVideoMode() Failed: " <<
			SDL_GetError() << endl;
		return false;
	}
	return true;
}

TTF_Font * setuptext()
{
	if (TTF_Init() != 0)
	{
		cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
		SDL_Quit();
	}
	TTF_Font *font;
	font = TTF_OpenFont("FreeSans.ttf", 24);
	if (font == NULL)
	{
		cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}
}