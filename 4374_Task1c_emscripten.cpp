#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

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
	srand(static_cast<unsigned>(time(0)));
}

int Random(int max) {
	return (rand() % max) + 1;
}

void GetSystemDate(int& day, int& month, int& year)
{ //get date from system
	time_t ltime;
	struct tm *now;
	// Set time zone from TZ environment variable. If TZ is not set, the operating
	// system is queried to obtain the default value for the variable. 
	tzset();
	// Get UNIX-style time. 
	time(&ltime);
	// Convert to time structure. 
	now = localtime(&ltime);
	// Set Time objects members. 
	day = now->tm_mday;
	month = now->tm_mon + 1;
	year = now->tm_year + 1900;
}

void GetSystemTime(int& hrs, int& mins, int& secs)
{
	time_t now = time(0);
	struct tm& t = *localtime(&now);
	hrs = t.tm_hour;
	mins = t.tm_min;
	secs = t.tm_sec;
}

string DateToString(int day, int month, int year)
{ //convert the date to a string in format (dd/mm/yyyy)
	ostringstream os;
	const char prev(os.fill('0'));
	os << setw(2) << day << "/"
		<< setw(2) << month << "/"
		<< setw(4) << year;
	os.fill(prev);
	return (os.str());
}

string TimeToString(int h, int m, int s)
{ //convert the time to a string in 24-h digital clock format (00:00:00)
	ostringstream os;
	const char prev(os.fill('0'));
	os << setw(2) << h << ":"
		<< setw(2) << m << ":"
		<< setw(2) << s;
	os.fill(prev);
	return (os.str());
}

string GetDate()
{
	int day, month, year;		//hold the current date
	GetSystemDate(day, month, year);
	return (DateToString(day, month, year));
}

string GetTime()
{
	int hrs, mins, secs;		//hold the current time
	GetSystemTime(hrs, mins, secs);
	return (TimeToString(hrs, mins, secs));
}

int main()
{
	bool setupdisplay(SDL_Surface* display);
	void setuptext(TTF_Font *font);
	void gameloop(SDL_Surface* display, TTF_Font *font, player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes, char grid[SIZEY][SIZEX], vector<replay>& replayer, int& hours, int& amin, int& seconds);
	string mainloop(int& levelSelection, SDL_Surface *image, TTF_Font *font);
	void initialiseGame(char grid[][SIZEX], player& spot, vector<zombie>& zombies, vector<Item>& holes, vector<pill>& pills);
	int levelSelection;
	SDL_Surface* display;						// the screen
	vector<zombie> zombies;					//initalize the zombies
	vector<pill> pills; 					//initalize pills
	vector<Item> holes; 					//initalize holes
	TTF_Font *font;
	char grid[SIZEY][SIZEX];					//grid for display
	vector<replay> replayer;					//creates a list of moves to replay
	int hours, amin, seconds;					//sets up the current time

	//These are all the functions we call in our main body of code, they all pass different paramters
	setupdisplay(display);						// setup screen
	setuptext(font);							// text system
	player spot = { SPOT, 0, 0, mainloop(levelSelection, display, font), 5 };     //creates the player based on what level and name they choose
	spot.levelChoice = levelSelection;			//this sets the level that is selected in the main loop
	initialiseGame(grid, spot, zombies, holes, pills);  //initialise grid (incl. walls and spot etc)
	GetSystemTime(hours, amin, seconds);			//gets the current time on the system
	emscripten_set_main_loop_arg((em_arg_callback_func)gameloop, &(display, font, spot, zombies, pills, holes, grid, replayer, hours, amin, seconds), 60, 0);
}

void gameloop(SDL_Surface* display, TTF_Font *font, player& spot, vector<zombie>& zombies, vector<pill>& pills, vector<Item>& holes, char grid[SIZEY][SIZEX], vector<replay>& replayer, int& hours, int& amin, int& seconds)
{
	bool isArrowKey(const int k);
	bool isCheatKey(const int k);
	int getsize(const vector<pill>& pills);
	void ApplyCheat(const int key, player& spot, vector<zombie>& zombies, vector<pill>& pills);
	void updateGame(char grid[][SIZEX], player& spot, const int key, string& message, vector<zombie>& zombies, vector<pill>& pills, const vector<Item>& holes);
	void renderGame(const char g[][SIZEX], const string &mess, const player &spot, const int zomlives, const int remaingpills, const int diff, SDL_Surface* display, TTF_Font *font);
	bool haswon(const vector<zombie>& zombies, const player &spot, SDL_Surface* display, TTF_Font *font);
	bool haslost(const player &spot, string& message);
	bool wantToQuit(const int k, string& message);
	bool isreplayKey(const int k);
	void displayallmoves(const vector<replay> &replayer, SDL_Surface* display, TTF_Font *font);
	void saveboard(vector<replay>& replayer, const char grid[][SIZEX]);
	int  getKeyPress();
	string message("");				//current message to player
	int key(' ');					//declares the input key
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
	else if (isreplayKey(key))		//if its the replay key
		displayallmoves(replayer, display, font);	//replays all moves up till the point pressed
	int nhours, nmin, nseconds;			//declares the time to track
	GetSystemTime(nhours, nmin, nseconds); //gets the new time after this has passed
	const int diff = (((nhours - hours) * 3600) + ((nmin - amin) * 60) + (nseconds - seconds)); //gets the time based on the difference
	//of the two times
	renderGame(grid, message, spot, zombies.size(), getsize(pills), diff, display, font);        //render game state on screen
	if (!wantToQuit(key, message) && (!haswon(zombies, spot, display, font) && !haslost(spot, message)))
	{
		spot.levelChoice++;
		spot.totalscore = spot.totalscore + spot.lives;
		spot.isProtected = false;
	}
}

bool isreplayKey(const int key)
{
	return (toupper(key) == REPLAY);				//returns true if replay key
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

int getsize(const vector<pill>& pills)
{
	int pils = 0;
	for (const pill& item : pills)
		if (!item.eaten)
			++pils;
	return pils;
	//gets the size of the pills based on what is eaten
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

string mainloop(int& levelSelection, SDL_Surface *image, TTF_Font *font)
{
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
			if (pills[i].eaten == true)
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
	void placepillonmap(char grid[][SIZEX], vector<pill>& pills,const player& spot);
	void placeholeonmap(char grid[][SIZEX], vector<Item>& holes,const player& spot);
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
	SDL_Color backgroundColor = { 0, 0, 255 };
	SDL_Rect dstrect = { 40, 15, 0, 0 };
	SDL_Surface *text;
	text = TTF_RenderText_Shaded(font, "please select a level: ", text_color, backgroundColor);
	SDL_BlitSurface(text, NULL, image, NULL); // add text to framebuffer
	SDL_FreeSurface(text); // prevent mem leak
	//prints out this message
}

void placepillonmap(char grid[][SIZEX], vector<pill>& pills,const player& spot)
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

void placeholeonmap(char grid[][SIZEX], vector<Item>& holes,const player& spot)
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
	for (const zombie& zom : zombies)
		if (zom.alive == true)
			return false;
	// if any zombies are alive return false
	const SDL_Color text_color = { 255, 0, 255 };
	const SDL_Color backgroundColor = { 0, 0, 255 };
	SDL_Rect dstrect = { 40, 17 };
	stringstream a;
	if (spot.levelChoice < 3)
		a << "Congratulations, you have finished this level!";
	else
		a << "Congratulations, you have finished the game!" << endl << "Your score is: " << spot.totalscore;
	SDL_Surface *text = TTF_RenderText_Shaded(font, a.str().c_str(), text_color, backgroundColor);
	SDL_UnlockSurface(image);
	SDL_BlitSurface(text, NULL, image, NULL); // add text to framebuffer
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

void renderGame(const char gd[][SIZEX], const string &mess, const player &spot, const int zombielives, const int remainingpill, const int diff, SDL_Surface *image, TTF_Font *font)
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

	SDL_FillRect(image, NULL, SDL_MapRGB(image->format, 0, 0, 0));
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
	// display screen to user
	SDL_Flip(image);
}

void paintGrid(const char g[][SIZEX], SDL_Surface *image, TTF_Font *font)
{
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 255, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	SDL_Rect dstrect = { 0, 2, 0,0 };
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
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showrempill(const int pils, SDL_Surface *image, TTF_Font *font)
{// display the number of pills left on the board 
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 10, 0, 0 };
	stringstream a;
	a << "pills left: " << pils;
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showDescription(SDL_Surface *image, TTF_Font *font)
{// displays a description of the game during the main menu
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 2, 0, 0 };
	drawtext("This is a game where you must escape/nthe zombies and survive. Pills mean/na life is gained./nContact with a hole(0) or zombie(Z)/nmeans a life is lost ", image, font, text_color, backgroundColor, dstrect);
}

void showTitle(SDL_Surface *image, TTF_Font *font)
{ //display game title
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color1 = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Color text_color2 = { 255, 0, 0 }; // R,G,B
	const SDL_Rect dstrect = { 40, 0, 0, 0 };
	const SDL_Rect dstrec2 = { 0, 0, 0, 0 };
	drawtext("___ZOMBIES GAME SKELETON___", image, font, text_color1, backgroundColor, dstrec2);
	drawtext("Oliver Parker, Liam Hill, Alex Odgen/n1RR - COMPUTER SCIENCE", image, font, text_color2, backgroundColor, dstrect);
}

void showSaveLoad(SDL_Surface *image, TTF_Font *font)
{//displays the save and load options during the game
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 11, 40, 11 };
	drawtext("Press S to save your game/nPress L to load your game", image, font, text_color, backgroundColor, dstrect);
}

void showname(const string &name, SDL_Surface *image, TTF_Font *font)
{// display the players name during the game
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 13, 0, 0 };
	stringstream a;
	a << "your name: " << name;
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showOptions(SDL_Surface *image, TTF_Font *font)
{ //show game options
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect1 = { 40, 7, 0, 0 };
	const SDL_Rect dstrect2 = { 40, 19, 0, 0 };
	drawtext("TO MOVE USE KEYBOARD ARROWS  ", image, font, text_color, backgroundColor, dstrect1);
	drawtext("TO QUIT ENTER 'Q'   ", image, font, text_color, backgroundColor, dstrect2);
}

void showLives(const player &spot, SDL_Surface *image, TTF_Font *font)
{ //show game options
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0,  255 };
	const SDL_Rect dstrect = { 40, 9, 0, 0 };
	stringstream a;
	a << spot.lives << " lives left";
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showMessage(const string &m, SDL_Surface *image, TTF_Font *font)
{ //print auxiliary messages if any
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 255, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 8, 0, 0 };
	drawtext(m.c_str(), image, font, text_color, backgroundColor, dstrect);
}

void endProgram(const string &message, SDL_Surface *image, TTF_Font *font)
{ //end program with appropriate message
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 8, 0, 0 };
	drawtext(message.c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showmenu(SDL_Surface *image, TTF_Font *font)
{//shows the buttions the user can press
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 10, 0, 0 };
	drawtext("press p to play/npress i to get infomation/npress b to display leaderboard", image, font, text_color, backgroundColor, dstrect);
}

void showtime(SDL_Surface *image, TTF_Font *font)
{// shows the current date and time
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect); 
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 40, 14, 0, 0 };
	stringstream a;
	a << GetDate()<< endl << GetTime();
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
}

void showgametitle(SDL_Surface *image, TTF_Font *font)
{// displays the title of the game before the board is drawn
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect);
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	const SDL_Rect dstrect = { 2, 4, 0, 0 };
	drawtext("------------------------/n| SPOT AND ZOMBIE GAME |/n------------------------", image, font, text_color, backgroundColor, dstrect);
}

void showdiff(const int diff, SDL_Surface *image, TTF_Font *font)
{
	void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect);
	const SDL_Rect dstrect = { 40, 16, 0, 0 }; // x,y,w,h
	const SDL_Color text_color = { 255, 0, 255 }; // R,G,B
	const SDL_Color backgroundColor = { 0, 0, 255 };
	stringstream a;
	a << "time spent in game: " << diff << endl;
	drawtext(a.str().c_str(), image, font, text_color, backgroundColor, dstrect);
	//shows the time spent
}

bool setupdisplay(SDL_Surface* display)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		cout << "SDL_Init() Failed: " <<
			SDL_GetError() << endl;
		return false;
	}
	// Set the video mode
	display = SDL_SetVideoMode(screenhight, screenwith, pixels, SDL_SWSURFACE);
	if (display == NULL)
	{
		cout << "SDL_SetVideoMode() Failed: " <<
			SDL_GetError() << endl;
		return false;
	}
	return true;
}

void setuptext(TTF_Font *font)
{
	if (TTF_Init() != 0)
	{
		cout << "TTF_Init() Failed: " << TTF_GetError() << endl;
	}
	font = TTF_OpenFont("ARIAL.TTF", 12);
	if (font == NULL)
	{
		cout << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
	}
}

void drawtext(const char* string, SDL_Surface *image, TTF_Font *font, const SDL_Color& text_color, const SDL_Color& backgroundColor, SDL_Rect dstrect)
{
	cout << "drawing string " << string << " at location x: " << dstrect.x << " y: " << dstrect.y << endl;
	SDL_Surface *text = TTF_RenderText_Shaded(font, string, text_color, backgroundColor);
	SDL_UnlockSurface(image);
	SDL_BlitSurface(text, NULL, image, &dstrect); // add text to framebuffer
	SDL_FreeSurface(text);
}

void DrawImage(SDL_Surface *surface, const char *image_path, const int x_pos, const int y_pos)
{
	SDL_Surface *image = IMG_Load(image_path);
	if (!image)
		printf("IMG_Load: %s\n", IMG_GetError());
	SDL_Rect rcDest = { x_pos, y_pos, 0, 0 };
	SDL_UnlockSurface(surface);
	SDL_BlitSurface(image, NULL, surface, &rcDest);
	SDL_FreeSurface(image);
}