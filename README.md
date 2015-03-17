[![Build Status](https://magnum.travis-ci.com/hillliam/zombiegame.svg?token=Y1339iZFApz4EjDASsTe&branch=master)](https://magnum.travis-ci.com/hillliam/zombiegame)


# ZombieGame
FoP Group Project
Alex Ogden, Liam Hill, Oliver Parker
------------------------------------
In-progress.

current progress

- [ ] basic version
  - [ ] spot & holes #1
    - [x] position 12 holes on map
    - [x] add spot movment
    - [x] prevent sop from leaving map
    - [x] remove life when faling in hole
    - [ ] display number of lives
    - [x] allow player to exit with 'q' and 'Q'
    - [x] allow player to exit when lost all lives
  - [ ] Power pills #2
    - [x] position 8 pills on the map
    - [x] remove the pill and reward the player when they land on it
    - [ ] display number of remaing pills
    - [x] exit when all pills have been eaten and all zombies have died
  - [ ] Zombies
    - [x] position 4 zombies in the corners of the map
    - [x] move the zombies eatch turn
    - [ ] detect when zombie has hit other item on map
  - [x] cheat codes
    - [x] f stops the zombies from moving
    - [x] x removes all zombies from map
    - [x] e removes all pills from map
  - [ ] Game entry
    - [ ] displaying the title of the game
    - [ ] displaying the information about the group members
    - [ ] displaying the current date (as DD/MM/YYYY) and time (as HH:MM::SS), in the same place, on the entry screen and while the game is being played
    - [ ] showing a brief, friendly description of the game, including a description of how the score is calculated and the various commands available when the ’I’ key (for ‘Information'), with the <Enter> key allowing the user to return to the entry screen.
    - [ ] starting the game when the user presses the 'P' key
    - [x] quitting the application when the user presses the 'Q' key
	  - [ ] ignoring any other key.
  - [ ] Player name and score
    - [ ] ask the player for their name
    - [ ] displaying the player's name (20 letters max)
    - [ ] displaying the player's best score so far (or -1 if no previous score has been recorded) on the screen while the game is being played
    - [ ] storing the player's best score (-1 if the player plays for the first time) into a file named after the player's name with extension .scr at the end of a game.
- [ ] precedure dependency diagram of basic version
- [ ] Extended version
  - [ ] make a copy of the game (have to demo both versions)
  - [ ] Magic protection
    - [ ] add new varible to player structure
    - [ ] set a random number of the 12 to be magic pills
    - [ ] dont allow zombie attacks if protected turns > 0
    - [ ] remove lives from zombie when it hits protected player
    - [ ] zombies run awway from player when protected
  - [ ] Inner walls
    - [ ] create a vector<item> for walls
    - [ ] read the location of the walls from file or c++
    - [ ] place walls before pills and holes
  - [ ] Save/Load Game option
    - [ ] allow user to press s to save the game
    - [ ] allow user to press l to load the game
    - [ ] write the entire map and the number of zombie lives to the file
    - [ ] read the file and set the map to what is stored in the file
    - [ ] allow user to load save in main menu if avalible
  - [ ] Game levels
    - [ ] read new level from files (new wall positions) e.g 1.lvl
    - [ ] when the play has won the game load the next level in main
  - [ ] Best scores record
    - [ ] write the best score to file
  - [ ] Replay option
    - [ ] allow the user to press r to show all moves that have being made during the game
    - [ ] save the state of the board eatch move allowing it to be replayed
  - [ ] A ‘Timed’ Game Option
    - [ ] find an async version of getch()
