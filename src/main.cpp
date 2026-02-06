#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_size.h>
#include <bn_string.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"
#include <bn_backdrop.h>
#include "bn_sprite_items_obstacle.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;
static constexpr int BOOST_DURATION = 60;
static constexpr int MAX_BOOSTS = 3;
// variables for timer
static constexpr int GAME_TIME_FRAMES = 60 * 30; // for 30 seconds
static constexpr int TIMER_X = -70;
static constexpr int TIMER_Y = -70;
// some colors for changing backdeop
static constexpr bn::color BG_COLORS[] = {
    bn::color(4, 4, 6),  // blue-grey
    bn::color(16, 0, 16), // purple
    bn::color(7, 10, 7),  // forest green
    bn::color(18, 9, 3),  // brown
    bn::color(3, 3, 8), // dark blue
    bn::color(9, 7, 5), // dusty brow
};
static constexpr int BG_COLOR_COUNT = 6;

// for obstacle
static constexpr int OBSTACLE_X = 0;
static constexpr int OBSTACLE_Y = 0;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size OBSTACLE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// player location
static constexpr int PLAYER_X = -50;
static constexpr int PLAYER_Y = -50;

// treasure location
static constexpr int TREASURE_X = 50;
static constexpr int TREASURE_Y = -50;

int main()
{
    bn::core::init();

    // adding backdrop color
    bn::backdrop::set_color(bn::color(31, 0, 0));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::vector<bn::sprite_ptr, 6> score_label_sprites;

    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);
    text_generator.generate(
        SCORE_X - 50,
        SCORE_Y,
        "Score:",
        score_label_sprites);

    int score = 0;

    int boost_timer = 0;
    int boosts_left = MAX_BOOSTS;
    int speed_multiplier = 1;
    int bg_color_index = 0;
    int game_timer = GAME_TIME_FRAMES;

    bool game_over = false;

    // will hold the sprite for time
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> timer_sprites = {};
    bn::vector<bn::sprite_ptr, 5> timer_label_sprites;
    text_generator.generate(
        TIMER_X - 43,
        TIMER_Y,
        "Time:",
        timer_label_sprites);

    // will hold the sprite for game over
    bn::vector<bn::sprite_ptr, 10> game_over_sprites;
    bn::vector<bn::sprite_ptr, 16> final_score_sprites;
    bn::vector<bn::sprite_ptr, 12> restart_sprites;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_X, PLAYER_Y);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(TREASURE_X, TREASURE_Y);
    bn::sprite_ptr obstacle = bn::sprite_items::obstacle.create_sprite(OBSTACLE_X, OBSTACLE_Y);

    while (true) {
        
        if (game_over){
            // Clear old gameplay text
            score_sprites.clear();
            timer_sprites.clear();

            // Show GAME OVER text
            bn::vector<bn::sprite_ptr, 10> game_over_text;
            bn::vector<bn::sprite_ptr, 16> final_score_text;
            bn::vector<bn::sprite_ptr, 12> restart_text;

            text_generator.generate(-100, -10, "GAME OVER", game_over_text);

            bn::string<MAX_SCORE_CHARS> final_score =
                "Score: " + bn::to_string<MAX_SCORE_CHARS>(score);

            text_generator.generate(-100, 10, final_score, final_score_text);
            text_generator.generate(-100, 30, "Press START", restart_text);

            // Restart game
            if (bn::keypad::start_pressed())
            {
                game_over = false;
                score = 0;
                game_timer = GAME_TIME_FRAMES;

                player.set_position(PLAYER_X, PLAYER_Y);
                treasure.set_position(TREASURE_X, TREASURE_Y);
            }

            bn::core::update();
            continue;
        }

        // Activate speed boost
        if (bn::keypad::a_pressed() && boost_timer == 0 && boosts_left > 0)
        {
            speed_multiplier = 2;
            boost_timer = BOOST_DURATION;
            boosts_left--;
        }

        // calculate speed boost
        bn::fixed move_speed = SPEED * speed_multiplier;

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - move_speed);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + move_speed);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - move_speed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + move_speed);
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());
        bn::rect obstacle_rect = bn::rect(obstacle.x().round_integer(),
                                          obstacle.y().round_integer(),
                                          OBSTACLE_SIZE.width(),
                                          OBSTACLE_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;

            // change the backdrop color
            bg_color_index++;
            // Loop back to first color if we reach the end
            if (bg_color_index >= BG_COLOR_COUNT)
            {
                bg_color_index = 0;
            }
            // Apply new background color
            bn::backdrop::set_color(BG_COLORS[bg_color_index]);
        }

        // if player hit the obstacle, game will restart
        // if (player_rect.intersects(obstacle_rect))
        // {
        //     player.set_position(PLAYER_X, PLAYER_Y);
        //     treasure.set_position(TREASURE_X, TREASURE_Y);

        //     score = 0;
        //     boost_timer = 0;
        //     boosts_left = MAX_BOOSTS;
        //     speed_multiplier = 1;
        //     game_timer = GAME_TIME_FRAMES;
        // }

        if (player_rect.intersects(obstacle_rect)){
            game_over = true;
        }

        // using if statement so that the player loops around the screen.
        // When the player goes left and right it comes from both opposite screen
        if (player.x() < MIN_X)
        {
            player.set_x(MAX_X);
        }
        else if (player.x() > MAX_X)
        {
            player.set_x(MIN_X);
        }

        // When the player goes on top it will come from the bottom and if the player goes to the bottom it wil go from top

        if (player.y() < MIN_Y)
        {
            player.set_y(MAX_Y);
        }
        else if (player.y() > MAX_Y)
        {
            player.set_y(MIN_Y);
        }

        // Update boost timer
        if (boost_timer > 0)
        {
            boost_timer--;

            if (boost_timer == 0)
            {
                speed_multiplier = 1;
            }
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // decresing timer every second
        if (game_timer > 0)
        {
            game_timer--;
        }

        // display timer
        int seconds_left = game_timer / 60;

        bn::string<MAX_SCORE_CHARS> timer_string = bn::to_string<MAX_SCORE_CHARS>(seconds_left);
        timer_sprites.clear();
        text_generator.generate(TIMER_X, TIMER_Y,
                                timer_string,
                                timer_sprites);

        // restart the game by pressing start button
        if (bn::keypad::start_pressed())
        {

            // reset player's position
            player.set_position(PLAYER_X, PLAYER_Y);
            // reset treasure's position
            treasure.set_position(TREASURE_X, TREASURE_Y);
            // reset score
            score = 0;
            score_sprites.clear();

            boost_timer = 0;
            boosts_left = MAX_BOOSTS;
            speed_multiplier = 1;
        }

        // restart the game once times up
        // if (game_timer == 0)
        // {
        //     player.set_position(PLAYER_X, PLAYER_Y);
        //     treasure.set_position(TREASURE_X, TREASURE_Y);

        //     score = 0;
        //     boost_timer = 0;
        //     boosts_left = MAX_BOOSTS;
        //     speed_multiplier = 1;
        //     game_timer = GAME_TIME_FRAMES;
        // }

        if(game_timer == 0){
            game_over = true;
        }

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}