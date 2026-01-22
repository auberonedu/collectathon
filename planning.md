A place to write your findings and plans

## Understanding
* Speed Variable - How fast the player dot moves
* Size - pixel by pixel size of treasure cube
* Sprite variables hold scores, constant + holds player and treaure, also constant
* MIN_X/Y, MAX_X/Y: Sets bounds of where player can move
* SCORE_X/Y: Sets location of score
* rng: Random number generator, used to decide where treasure cubes will spawn
* player_rect/treasure_rect: The space each object occupies; determines if they are touching
* score: Self-explanatory; updates on score++
* Score is displayed as a string

## Planning required changes
* Change the speed of the player
* Change the backdrop color
* Change the starting position of the player and dot, making new static constexpr for starting X and Y of each
* Make it so when the player hits start, the game restarts (the player and treasure are sent back to their initial positions and the score is reset to zero)
* Make it so that the player loops around the screen (if they go off the left of the screen, they show up on the right, if they go off the bottom of the screen they show up at the top, etc.)
* Make a speed boost. When the player presses 'A', their speed is increased for a short amount of time. They can only use the speed boost 3 times. They get all speed boosts back when the game is restarted by pressing start.

## Brainstorming game ideas

## Plan for implementing game

