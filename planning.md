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
How about the screen shake when using boost 


## Plan for implementing game
boost count + shaky shaky when using boost
- show 3 begining. maybe top left? or top right hmm.
- go down 1 for each time using A
- show 0 when out
- screen shake when using boost
shaky_x and y.   boost/frame_left using a_pressed()
