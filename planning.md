A place to write your findings and plans

## Understanding
Cristian's understanding:
-variables that were initiated like the speed, player size, treasure size.
-while loop that constantly updates the game by frame.
-conditional statements that determine movemement of the player. 
-conditional statement if the player touches the yellow ball, the score increases

Brook's understanding:
Player size
Speed
Boundaries of the game
While loop that goes through the actual game to keep it updated and running
if statements that repeat the boundary set outside of the main
conditional statements that rely on the movement of the user
if statements that repeat loop and add score

## Planning required changes
- speed of the player to 1 for greater speed with boosts
- add a bn for backdrop and change the color
- find the pixel size of the screen and divide it to find another location for the sprite
- use the a pressed value to loop back to initial game start
- use the same pixel size of the screen and use that for the minimum and maximum for the x and y coordinates. Then loop back to opposite pixel size on the screen when out of bounds.
- make if statement when a button is pressed it activates a new speed value for the sprite
    - make a boost counter that is inside of another if statement that will not work when the count becomes 0


## Brainstorming game ideas
- already implemented the boost count on the screen (unintentionally)
- change the color of the background/make a color affect when the player collects the treasure
- change the sprite to another picture
-change the treasure picture


## Plan for implementing game

- edit the original boost counter if statement with a new int that is displayed on the screen to show the counter
- (refrence the implemented treasure counter for the boost counter)
- make an if statement when the sprite collects the treasure, the background color changes
- edit the graphics file for the sprite picture
        - refrence the bn actions to help with sprite (I only needed the basic structure because the example used one whole bmp image)
        - use a png to bmp converter and manipulate the image to be 8-bit coloration
        - use procreate to draw the left, right, and front sprite images
        - put the original square.bmp and square.json into a seperate file in case it is neccessary

        -use a new int for direction that changes for different inputs (instead of constantly creating a sprite)
        - if statement for each dpad direction input that checks what number the direction is and changes it accordingly


## Make one or more improvements to the game itself. 
Improved graphics: added new sprites for better graphic/decoration
New features: added a timer for the treasure to reset to different locations after a certain amount of time

## Make one or more improvements to the code.
Removed dead code / outdated comments
Added new, better comments


New ideas for sprites:
make the treasure chicken food for the chicken to eat
add a background sprite that is an open bag of food to imply that chicken food keeps falling out
add text to explain how to play the game