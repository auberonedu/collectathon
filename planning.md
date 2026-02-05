A place to write your findings and plans

## Understanding
<<<<<<< HEAD
understood about size, random place for the sprites to located at rando place in the screen.
i dont really understand the score display.git p
=======
I understand what bn::keypad means. 
I understand what the int new_x and int new_y mean when the treasure is 'caught' it gets spawned to a different point on the screen. 
>>>>>>> f4d9309ccd4acb49ae22a642bef8dae8165e1f04

## Planning required changes
<<<<<<< HEAD
=======
Change the speed of the player (B)
Change the backdrop of color (A)
Change the starting position of the player and dot (B)
Make it so when the player hits start, the game restarts (A)
Make it so that the player loops around the screen (B)
Make a speed boost (A)
>>>>>>> b0d2cbe258d816764434bd05b2b80b55951ad451
## Brainstorming game ideas
I'm thinking add a boost count that will display the number of boost we have
prob gonna be st like 3. // 2 // 1 // 0
How about the screen shake when using boost  // not work! so hard
make the player blink when using boost?


Change the player sprite color (or palette) while boosted
Add a faster speed when going vertical. 
Even adding another sprite that is against the player, maybe slows the character down?



## Plan for implementing game
boost count + blink when using boost
- show 3 begining. maybe top left? or top right hmm.
- go down 1 for each time using A
- show 0 when out
- player blink when usingboost

change sprite to orange when boosted
add another sprite that slows character down, maybe color blue?
try to see if i can add faster speed.


-------- 
I still wanna try the shaking when boost but I may do it afterward since I need to do some homework, so ... to be continue



## wave 7 -- improvements/planning:
Add a timer (15 secs) 
Slow down the regular speed of the player sprite
Add sound effects when dying using jsfxr
Add another sprite (could be designed) that chases the player

EDIT: 2/3/2026 - Kelley:
I added a timer, from 15 seconds to 30 seconds. 
I also slowed down the speed from 4 to 2 for the player.
I added sound effects when dying/getting the treasure.
I added another sprite called 'dragon' that chases the player.
I changed the graphics of both the treasure and player to look like a chest and a knight. 
EDIT: 2/4/2026 - Kate
I added a combo effect where it show combo, x2,x3,etc
planning to add some background decor, alr set up, still testing the code.
I added some grass using libresprite for background.