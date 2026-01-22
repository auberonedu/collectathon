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

6. For the speed post we need to add a `duration` variable and a `boosts_left` variable. We need to add a `if` a is pressed, then drop the `boosts_left` by 1 and increase `duration` to the desired boost length.  then add `if(duration > 0)` then we have the boost applied and we drop the duration by 1 for the frame that just went by. `else` set the speed back to normal. 
## Brainstorming game ideas

## Plan for implementing game

