A place to write your findings and plans

## Understanding
There is a player size and treasure size with a player speed (changable)

There is a score that starts at 0 (counter)

There is basic controls to move the player

There is a spring player and treasure that are set at different locations

There is a variable called rng that is used to give the treasure a random spawn point

Code for if the player intersects the treasure, give it a random location and up the score by one.


## Planning required changes

1. Find the variable that increases player speed

Change the value to a higher one to increase it.

2. find the variable that is for the background, then insert a hex color and remake the game 
to change the backdrop color.

    Variable was not found. I had to import files in order to change color of background to light blue.

3. Change starting pos of the player and dot using a variable for each cord.

    Made variable of static constexpr for x and y of each character.
    Speed was also changed to a lower amount 

4. Create a if statement for the reset keybind, just create another if statement and just reset the 
x position and the y position, along with the treasure and the score!

5. Probably use the already made variables (MIN_X, MIN_Y, etc) and check to see if the player if above or below those variables. If so, put them opposite side so the player loops


6. add a if statement and say when the player presses the A button on the gameboard, the speed
is increased by a lot, and create a int variable that starts at 3 and whenever they click the 
keybind it would get subtracted by one.

7. We need a variable that represents the speed boost duriation (float) and a variable to keep how much turns of the speed boost (int) the user has. Also need a variable that represents the actual speed boost (int) and a variable (int) that can change between the normal and boosted speed of the player. Also need a variable to represent the player being in the speed boost mode (bool). We can do if the user pressed the a button, we have turns and if the speed boost mode is not active, turn it on with the timer started and a speed boost taken away. We need another if statement stating if the speed boost mode is active, decrement the timer variable by a certain value with the players speed at that boosted speed. Also need an if statement for if the timer reaches 0, turn the speed back to normal and the speed boost mode off.

## Brainstorming game ideas

1. We can add text to the top of the screen indicating that the speed boost is currently in use.

2. Whenever the movement keybinds are pressed, the character can spin around while the user is playing

3. We can add a red square indicated as a enemy, and it can follow the player around. If the enemy
touches the player the game automatically restarts back to 0.

4. Add a timer for the player when the game starts and every time the player gets the treasure, add 5 seconds to the timer.

5. Add a sprite (human) to indicate the player and use the square for 5 points and circle for one points. Make the square come out randomly

6. Try to add sound when the player gets the circle, square, lose and when they are in the speed boost.

7. After getting a point, change the background color randomly.


## Plan for implementing game

2. for the player rotation, the plan is to make a new int variable for the rotation, and within the while loop make another if statement that selects the keybind and it has another if statement that if rotation is less than 360 or equal to it, it resets to 0.


1. i will initiate a string variable for the boost in use, and then i will implement a if statement using the speedbooston boolean. If the speed boost is in use i will add a text on the top of the screen indicating that the user is currently using the speed boost.

7. Try to see how to get a random number every time the user gets a point. Found in doc that there is a random class and teacher already made a variable from it. Use it to get a random num every time player gets point.






## 10/6/26

Change the enemies to actual sprites. - Alex

Change the main user to an actual sprite - Alex

Add background music - Ali

Make the background colors easily visible on the GMA device - Alex

Make a weapon (Do this last) - (RAY GUN)

Enemies will have health bars - Ali


Add a nuke point that kills all the enemies when it's collected - Ali / Alex










## IMPROVEMENTS
i will make the sprite spin faster when the boost is in use as an improvement.

I will make the speed boost button to held instead of pressed, that way you can easily toggle it off when needed.

Added some comments to the code. Needed to remove duplicate if statement

Added the enemy player as discussed before. Needed to figure out its position relative to the player and make the enemy
go to the player. Figured it out by checking its x and y pos against the player and see where it was relative to the player and acted accordingly.