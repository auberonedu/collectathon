A place to write your findings and plans

## Understanding

player is moved by adding/subtracting speed from current position

This code section seems to make the "collectable" jump to a random x and y position on the screen when the player touches it: 
if (player_rect.intersects(treasure_rect)){
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);
            score++;
        }

This code sectoin seems to update the score of the player when he interacts with the collectable updating it by 1 point.
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y, score_string, score_sprites);

Kevin - 
bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);   
This piece of code updates the score. I see there is a clear method which I think erases the current score before its updated with the newer score. 

KFG - 
// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;
This code displays the score on the screen.

KFG - 
// charges of boost ability
    int boostCount = 3;
I think this code allows the player to increase speed of the sprite.

## Planning required changes

1. Change the speed of the player
    should be simple, just changing speed value
2. Change the backdrop color
    can copy code from previous assignments to set bg color
3. Change the starting position of the player and dot, making new static constexpr for starting X and Y of each
4. Make it so when the player hits start, the game restarts (the player and treasure are sent back to their initial positions and the score is reset to zero)
5. Make it so that the player loops around the screen (if they go off the left of the screen, they show up on the right, if they go off the bottom of the screen they show up at the top, etc.)
6. Make a speed boost. When the player presses 'A', their speed is increased for a short amount of time. They can only use the speed boost 3 times. They get all speed boosts back when the game is restarted by pressing start.
    maybe add a timer and a conditional that checks if the timer is > 0 and applies speed to pos twice if so
    timer could tick down every frame (if > 0) and increase on A press

## Brainstorming game ideas
KFG ideas
- Add some enemies like bugs or space monsters 
 - I do think something like this or hazards of some kind would be good, we could have them subtract score if touched, or add a health bar/life system - Matthew
- Have three rockets under the score to represent boosts left
- Have a background like in space or forest
- Add a barrier to the edge of the map
- Gives player bonus boost every 5 points they earn

- Could add acceleration on movement instead of having player go from 0 to top speed instantly - Matthew

## Plan for implementing game

