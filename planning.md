A place to write your findings and plans

## Understanding
(Buay) The main.cpp has the main includes such as core, keypad, size(for player), random for the dot
when it hits the player and some sprites for the game. This does not include the backdrop to set the
color background. There's a score count feature to where when player touches the sqaure it adds + 1 
to the score.

Player speed is controlled by `SPEED`.

Player and treasure sizes are conrolled by `PLAYER_SIZE` and `TREASURE_SIZE`.

Display boundaries are recored in `MIN_Y`,`MAX_Y`,`MIN_X`,and `MAX_X`.

`player` is at `(-50, 50)` and `treasure` is at `(0, 0)` for the starting positions.

## Planning required changes
1. To change the speed of the player change `static constexpr bn::fixed SPEED = 1;` to `static constexpr bn::fixed SPEED = 1.5;` to make the player move faster.

2. To change the backdrop color, `include` the `backdrop` and `color`, then in main set color to blue

3. Change the starting position of the player and dot, making new static constexpr for starting X and Y of each

4. To reset the game we just have to put an `if` start pressed, then set `score` to 0, set `boosts_left` to 3 and set `player` x and y to starting and set `treasure` x and y to starting aswell.

5. For the player to loop around the screen when it gets past the edge we would need to make an `if` in the `while` loop so that if the x or y of the player is greater or less than the max or min then change the x/y to the opposite wall.

6. For the speed boost we need to add a `duration` variable and a `boosts_left` variable. We need to add a `if` a is pressed, then drop the `boosts_left` by 1 and increase `duration` to the desired boost length.  then add `if(duration > 0)` then we have the boost applied and we drop the duration by 1 for the frame that just went by. `else` set the speed back to normal. 
## Brainstorming game ideas
Add a speed boost count
Create a vector of sprites outside the top border of the game. The player must avoid these sprites. If the player touches an obstacle, the game restarts.
Add text to score and boost count
add waves
restart the boost count after a couple of wave

## Plan for implementing game

