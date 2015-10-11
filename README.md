﻿[![Build Status](https://magnum.travis-ci.com/hillliam/zombiegame.svg?token=Y1339iZFApz4EjDASsTe&branch=master)](https://magnum.travis-ci.com/hillliam/zombiegame)


# ZombieGame
 FoP Group Project
Alex Ogden, Liam Hill, Oliver Parker
------------------------------------
## http://hillliam.github.io/zombiegame/

In-progress.

current progress

- [x] basic version
  - [x] spot & holes #1 @done
    - [x] position 12 holes on map
    - [x] add spot movment
    - [x] prevent sop from leaving map
    - [x] remove life when faling in hole
    - [x] player enters hole when in contact
    - [x] display number of lives
    - [x] allow player to exit with 'q' and 'Q'
    - [x] allow player to exit when lost all lives
  - [x] Power pills #2 @done
    - [x] position 8 pills on the map
    - [x] remove the pill and reward the player when they land on it
    - [x] display number of remaing pills
    - [x] exit when all pills have been eaten and all zombies have died
  - [x] Zombies #3 @done
    - [x] position 4 zombies in the corners of the map
    - [x] move the zombies eatch turn
    - [x] detect when zombie has hit other item on map
  - [x] cheat codes  #4 @done
    - [x] f stops the zombies from moving
    - [x] x removes all zombies from map
    - [x] e removes all pills from map
  - [x] Game entry  #5
    - [x] displaying the title of the game
    - [x] displaying the information about the group members
    - [x] displaying the current date (as DD/MM/YYYY) and time (as HH:MM::SS), in the same place, on the entry screen and while the game is being played
    - [x] showing a brief, friendly description of the game, including a description of how the score is calculated and the various commands available when the ’I’ key (for ‘Information'), with the <Enter> key allowing the user to return to the entry screen.
    - [x] starting the game when the user presses the 'P' key
    - [x] quitting the application when the user presses the 'Q' key
	  - [x] ignoring any other key.
  - [x] Player name and score  #4 @done
    - [x] ask the player for their name
    - [x] displaying the player's name (20 letters max)
    - [x] displaying the player's best score so far (or -1 if no previous score has been recorded) on the screen while the game is being played
    - [x] storing the player's best score (-1 if the player plays for the first time) into a file named after the player's name with extension .scr at the end of a game.
- [x] precedure dependency diagram of basic version  #5
- [x] Extended version   #6
  - [x] make a copy of the game (have to demo both versions)   #7
  - [x] Magic protection   #8
    - [x] add new varible to player structure
    - [x] set a random number of the 12 to be magic pills
    - [x] dont allow zombie attacks if protected turns > 0
    - [x] remove lives from zombie when it hits protected player
    - [x] zombies run awway from player when protected
  - [x] Inner walls   #9
    - [x] create a vector<item> for walls
    - [x] read the location of the walls from file or c++
    - [x] place walls before pills and holes
  - [x] Save/Load Game option   #10
    - [x] allow user to press s to save the game
    - [x] allow user to press l to load the game
    - [x] write the entire map and the number of zombie lives to the file
    - [x] read the file and set the map to what is stored in the file
    - [x] allow user to load save in main menu if avalible
  - [x] Game levels   #11
    - [x] read new level from files (new wall positions) e.g 1.lvl
    - [x] when the play has won the game load the next level in main needs some work
  - [x] Best scores record   #12
    - [x] write the best score to file
  - [x] Replay option   #13
    - [x] allow the user to press r to show all moves that have being made during the game
    - [x] save the state of the board eatch move allowing it to be replayed
  - [x] A ‘Timed’ Game Option #14
    - [x] only check for input and updategame when _kbhit() is true
    - [X] show the time in seconds since the game started and stoping the timer when it is done e.g GetSystemTime( int& hrs, int& mins, int& secs) to get the start time then displaying the diffrence between them in seconds

---
# improved check list
- [x] game rules
	- [x] When Spot eats a power pill (i.e. moves onto a cell that contains one) he gains one life and the power pill that has just been eaten disappears from the grid.
	- [x] When Spot falls into a hole, he loses one life but, providing he still has at least one life left, he will move out of the hole again on his next move. The hole remains in its place.
	- [x] When Spot moves into a cell that contains a zombie or when a zombie catches up with Spot, Spot loses one life and that particular zombie moves back onto its corner, ready to resume the chase as soon as it is the zombies' turn to move.
	- [x] Zombies cannot eat power pills. When a zombie moves onto a cell that contains one pill, the zombie is unaffected and the pill remains where it was after the zombie has left the cell.
	- [x] When Spot moves into a cell that contains both a zombie and a pill, Spot neither loses nor gains a life, the pill disappears and the zombie moves back onto its corner, ready to resume the chase as soon as it is the zombies' turn to move.
	- [x] Zombies must remain within the grid's boundaries. If they bump into a wall they remain in the same cell or slide along the wall in a blind attempt to remain close to Spot.
	- [x] When (if) two zombies bump into each other, they are both sent back to their corner, ready to resume the chase as soon as it is the zombies' turn to move.
	- [x] A zombie dies and disappears forever when it falls into a hole.
- [x] basic version
	- [ ] spot and zombie
		- [ ] positioning Spot and 12 holes at random on the grid, making sure they are not placed on the same cells
		- [ ] allowing the player to control Spot's movements on the grid
		- [ ] ensuring that Spot stays within the grid's boundaries, and remains in same place if the player tries to move him onto that wall
		- [ ] detecting when Spot has fallen into a hole and reacting as specified (see rules)
		- [ ] indicating at any time the number of remaining lives
		- [ ] terminating the game, with an appropriate message, (a) when the player wants to quit (e.g., "YOU QUIT!") or (b) when Spot has lost all his allocated lives (e.g., "YOU LOST!").
	- [ ] power pills
		- [ ] positioning 8 power pills at random on some of the grid free cells (i.e. they should not be placed where there is a hole, where Spot is, or where other power pills are)
		- [ ] detecting when Spot has eaten one power pills and reacting as specified (see rules)
		- [ ] indicating at any time the number of remaining pills (not eaten yet)
		- [ ] terminating the game, with an appropriate message, (a) when the player wants to quit (e.g., "YOU QUIT!"), (b) when Spot has lost all his lives (e.g., "YOU LOSE!") or (c) when all pills have been eaten and all zombies have died (e.g., "YOU WON WITH 5 PILLS LEFT!").
	- [ ] zombies
		- [ ] positioning 4 zombies on the grid, one on each corner, making sure not to place a hole, a pill or Spot on those corner cells
		- [ ] automatically moving the four zombies, after each of Spot's moves, so that they all (blindly) get closer to Spot (within the grid's boundaries)
		- [ ] detecting when a zombie has fallen into a hole, bumped into another zombie or caught Spot and reacting 		- [ ] terminating the game, with an appropriate message, (i) when the player wants to quit (e.g., "YOU QUIT!"), (ii) when Spot has lost all his allocated lives (e.g., "YOU LOSE!") or (iii) when the zombies are all dead and all pills have been eaten (e.g., "YOU WON WITH 5 PILLS LEFT!").
	- [ ] cheat codes
		- [ ] When the user presses the 'F' key (for 'Freeze') the zombies should stop moving. When this key is pressed again they should resume their chase when it is their turn to move.
		- [ ] When the user presses the 'X' key (for 'eXterminate') all zombies still alive on the board should be eliminated. When this key is pressed again they should reappear in their respective corners and resume the chase.
		- [ ] When the user presses the 'E' key (for 'Eat') all pills are automatically eaten, they disappear from the board and the numbers of remaining pills is set to 0.
	- [ ] game entry
		- [ ] displaying the title of the game
		- [ ] displaying the information about the group members (e.g., GROUP X7 – Tom Jones, Andrew Bloggs and Ann Smith)
		- [ ] displaying the current date (as DD/MM/YYYY) and time (as HH:MM::SS), in the same place, on the entry screen and while the game is being played (e.g., 26/03/2015  13:33:02).
		- [ ] showing a brief, friendly description of the game, including a description of how the score is calculated and the various commands available when the ’I’ key (for ‘Information'), with the <Enter> key allowing the user to return to the entry screen.
		- [ ] starting the game when the user presses the 'P' key (for ‘Play Game')
		- [ ] quitting the application when the user presses the 'Q' key (for ‘Quit Game')
		- [ ] ignoring any other key.
	- [ ] player name and score
		- [ ] asking the player for their name (a single word) when the program starts
		- [ ] displaying the player's name (20 letters max) on the screen while the game is being played
		- [ ] displaying the player's best score so far (or -1 if no previous score has been recorded) on the screen while the game is being played
		- [ ] storing the player's best score (-1 if the player plays for the first time) into a file named after the player's name with extension .scr (e.g., john.scr) at the end of a game.
	- [ ] enstended version
	- [ ] magic protection
	- [ ] iner walls
	- [ ] save and load
		- [ ] keeping saved versions available during current game and for future program executions.
		- [ ] ensuring that a player can only save one game (i.e., each time a player saves a game any previously saved data for that player is lost and replaced by the current state of the game)
		- [ ] describing the 'S' and 'L' options on the screen at all times
		- [ ] checking when the program starts whether some "saved" data is available for the player and, if it is, offering that player the choice to reload that (last) saved version or start a new game.
		- [ ] displaying an error message on the screen when the player presses ‘L’ if no “saved” version exists for the current player and allowing the game to resume.
	- [ ] game levels
	- [ ] best score
		- [ ] calculating and displaying the current player's score at all time,
		- [ ] keeping track of the scores of the six best players (together with the name of the players and the time and date at which a 'best' score were obtained) and recording this information in a text file called bestScores.txt
		- [ ] offering an additional option on the entry screen menu to display the high scores table (in descending order)when the player presses ‘B’ (for ‘Best Scores’),
		- [ ] detecting at the end of a game when a personal or overall record as been exceeded, updating them accordingly and displaying any appropriate congratulation message.
	- [ ] replay option
		- [ ] this only replays the moves from the start of the current game or level and stops after the last move on that game (i.e., just before the ‘R’ key was pressed),
		- [ ] the player can carry on playing after that and possibly ask for another replay that will again include all moves from the beginning to the current point in the game.
	- [ ] a timed option
		- [ ] displaying the time (in seconds) from the start of the game (player’s first move). The time should refresh each second, even if nothing else happens in the game (i.e., no key press).
		- [ ] detecting when the game has emend (i.e., player quits, wins or loses), stopping the timer and displaying the final time/score.
	- [ ] input error detection
		- [ ] if a key pressed is not valid (i.e., different from keyboard arrows or commend keys such as 'Q', 'S' or 'L'), display an error message on screen and ignore.
		- [ ] if the game level selected is not valid (i.e., not 1, 2 or 3), display an error message and ask for another one until you get a valid input.
		- [ ] only accept one word for the player's name (i.e., no digit or whitespace or punctuation character),
		- [ ] if no saved version can be found when the player press 'L', display an error message and continue game.

---
# files to upload
| files	        	   | 4374_ConsoleBasic_RR4.zip | 4374_ ConsoleExtended_RR4.zip |
| ------------------------ |:-------------------------:|:-----------------------------:|
| 4374_Task1c_Skeleton.cpp | yes           	       | yes 			       |
| ConsoleUtils.cpp 	   | yes               	       | yes  	 		       |
| ConsoleUtils.h 	   | yes               	       | yes    		       |
| Project_VS13.sln 	   | yes               	       | yes    		       |
| Project_VS13.vcxproj 	   | yes               	       | yes    		       |
| RandomUtils.cpp 	   | yes               	       | yes    		       |
| RandomUtils.h 	   | yes               	       | yes   			       |
| TimeUtils.cpp 	   | yes               	       | yes   			       |
| TimeUtils.h 		   | yes              	       | yes			       |

----
# building with emscriptem
## debug
```
emcc 4374_Task1c_emscripten.cpp -o project.js -g4 --preload-file hole.png --preload-file pill.png --preload-file player.png --preload-file wall.png --preload-file zombie.png
```
## release
```
emcc 4374_Task1c_emscripten.cpp -o project.js -O3 --js-opts 1 --closure 2 --preload-file pill.png --preload-file player.png --preload-file wall.png --preload-file zombie.png
```
