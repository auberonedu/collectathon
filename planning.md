A place to write your findings and plans

## Understanding
- the include section is like a library for our games needs

- "static constexpr bn::fixed SPEED = 1;" is a constant for the speed. you can change the speed here. 

- "bn::core::init();" Initializing the Butano Engine 

- I understand the part with the note about the width and height of the player and treasure bounding boxes. 

- The bounds of the screen is a little confusing but i get the idea i just wouldnt feel confident explaining it.

- the while loop is stating if the left button is held the speed of the game decreases and if the right button is held the speed increased. 

- What I dont understand: Text generator 

## Planning required changes
1.Change the speed of the player
change speed to 3 instead of 1

2.Change the backdrop color
Changed it to pink

3.Change the starting position of the player and dot, making new static constexpr for starting X and Y of each
changed player to (-60,-50)
changed treasure to (25,0)

4.Make it so when the player hits start, the game restarts (the player and treasure are sent back to their initial positions and the score is reset to zero)
created another if statement in the while true loop and when start is pressed. score gets set to 0 and player and treasure gets set to original positions.


5.Make it so that the player loops around the screen (if they go off the left of the screen, they show up on the right, if they go off the bottom of the screen they show up at the top, etc.)
I changed the code from  stop the player at the screen edges to “teleport the player to the opposite side when they leave the screen.”


6.Make a speed boost. When the player presses 'A', their speed is increased for a short amount of time. They can only use the speed boost 3 times. They get all speed boosts back when the game is restarted by pressing start.

Created two variables for speed boost and timer. Then created a while true loop and if statement to pick the speed. Then added boost and timer to apply when the game is restarted as well



## Brainstorming game ideas

## Plan for implementing game

