A place to write your findings and plans

## Understanding
I think,  the SPEED variable is constant and used for giving the constant pace for the sprites. PLAYER_SIZE & TREASURE_SIZE = {8,8} is being used to set how big/small creator wants the gaming elements to be. By using bn::display::height() & bn::display::width(), creator set the display size where the treasure will always be displayed. Currently there is no score limitation, player can make as many as score they want. For the score, creator set a specific place (70, -70) which is top right corner. Also for counting scores creator used a for loop.

## Planning required changes
I think, For our game we will create one of the changes which is adding a timer. It will be a 30 seconds timer and the player will have 30 seconds to capture the enemy and gain as many points as possible under 30 seconds. 

For our second change we added six different background colors red, purple, brown , blue, cyan and magenta. Whenever the player is playing it changes the background colors to those colors. 

For our third change we added a obstacle so everytime you hit that obstacle the game restarts. 

## Brainstorming game ideas
1. Different colors either changing the dot colors or background color.
2. Adding a timer or adding no timer.
3. Adding a obstacle so when you hit that obstacle the game restarts.  

## Plan for implementing game