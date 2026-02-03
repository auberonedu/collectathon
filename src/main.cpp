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
#include <bn_backdrop.h>
#include <bn_color.h>
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"
#include <bn_music.h>
#include <bn_music_items.h>
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_car.h"




// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 3;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size FOX_SIZE = {32, 32};
static constexpr bn::size CAR_SIZE = {32, 16};

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

int main()
{
    bn::core::init();

    //Start background music
    bn::music_items::background_music.play();

    // background color
    bn::backdrop::set_color(bn::color(31, 20, 25));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    // Color change variables
    int current_color_index = 0;
    bn::color level_colors[5] = {
        bn::color(31, 20, 25),  // Pink (starting color)
        bn::color(15, 25, 31),  // Blue
        bn::color(20, 31, 15),  // Green
        bn::color(31, 25, 10),  // Yellow
        bn::color(25, 15, 31)   // Purple
    };

    int boosts_left = 3;
    int boost_timer = 0;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(-60, -50);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(25, 0);
    bn::sprite_ptr fox = bn::sprite_items::fox.create_sprite(0, 40);
    bn::sprite_ptr car = bn::sprite_items::car.create_sprite(0, -40);

    // Fox chase speed (slower than player so it's beatable)
    bn::fixed fox_speed = 1.5;
    
    // Car random movement
    bn::fixed car_vx = 1;
    bn::fixed car_vy = 1;
    int car_direction_timer = 0;

    // Hide fox initially (it appears at level 15)
    fox.set_visible(false);

    while (true)
    {
        if(bn::keypad::a_pressed() && boosts_left > 0 && boost_timer == 0)
        {
            boost_timer = 60;
            boosts_left--;
        }

        bn::fixed current_speed = SPEED;

        if(boost_timer > 0)
        {
            current_speed = 6;
            boost_timer--;
        }

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - current_speed);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + current_speed);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - current_speed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + current_speed);
        }

        if(player.x() < MIN_X)
        {
            player.set_x(MAX_X);
        }
        else if(player.x() > MAX_X)
        {
            player.set_x(MIN_X);
        }
        if(player.y() < MIN_Y)
        {
            player.set_y(MAX_Y);
        }
        else if(player.y() > MAX_Y)
        {
            player.set_y(MIN_Y);
        }

        // Fox chasing behavior (only at level 15+)
        if (score >= 15)
        {
            fox.set_visible(true);
            
            // Calculate direction from fox to player
            bn::fixed dx = player.x() - fox.x();
            bn::fixed dy = player.y() - fox.y();
            
            // Normalize and move fox towards player
            bn::fixed distance = bn::sqrt(dx * dx + dy * dy);
            if (distance > 0)
            {
                fox.set_x(fox.x() + (dx / distance) * fox_speed);
                fox.set_y(fox.y() + (dy / distance) * fox_speed);
            }
        }
        else
        {
            fox.set_visible(false);
        }

        // Car random movement
        car.set_x(car.x() + car_vx);
        car.set_y(car.y() + car_vy);
        
        // Wrap car around screen
        if(car.x() > MAX_X)
        {
            car.set_x(MIN_X);
        }
        else if(car.x() < MIN_X)
        {
            car.set_x(MAX_X);
        }
        if(car.y() > MAX_Y)
        {
            car.set_y(MIN_Y);
        }
        else if(car.y() < MIN_Y)
        {
            car.set_y(MAX_Y);
        }
        
        // Change car direction randomly every 60-120 frames
        car_direction_timer++;
        if(car_direction_timer > 60 + rng.get_int(60))
        {
            car_direction_timer = 0;
            // Random velocities between -2 and 2
            car_vx = rng.get_int(-2, 2);
            car_vy = rng.get_int(-2, 2);
            
            // Make sure car is always moving
            if(car_vx == 0 && car_vy == 0)
            {
                car_vx = 1;
            }
        }

        // Reset game if start is pressed
        if (bn::keypad::start_pressed())
        {            
            score = 0;
            boosts_left = 3;
            boost_timer = 0;
            current_color_index = 0;
            player.set_position(-60, -50);
            treasure.set_position(25, 0);
            fox.set_position(0, 40);
            fox.set_visible(false);
            car.set_position(0, -40);
            car_vx = 1;
            car_vy = 1;
            car_direction_timer = 0;
            player.set_scale(1);  // Reset size
            bn::backdrop::set_color(level_colors[0]);  // Reset color
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

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
        }

        // Fox collision (only if visible)
        if (fox.visible())
        {
            bn::rect fox_rect = bn::rect(fox.x().round_integer(),
                                          fox.y().round_integer(),
                                          FOX_SIZE.width(),
                                          FOX_SIZE.height());
            
            if (player_rect.intersects(fox_rect))
            {
                score = bn::max(0, score - 2);  // Lose 2 points
                player.set_position(-60, -50);  // Reset player position
                fox.set_position(0, 40);  // Reset fox position
            }
        }

        // Car collision
        bn::rect car_rect = bn::rect(car.x().round_integer(),
                                      car.y().round_integer(),
                                      CAR_SIZE.width(),
                                      CAR_SIZE.height());
        
        if (player_rect.intersects(car_rect))
        {
            score = bn::max(0, score - 2);  // Lose 2 points
            player.set_position(-60, -50);  // Reset player position
        }

        // Color change every 5 levels
        if (score > 0 && score % 5 == 0)
        {
            int new_color_index = (score / 5) % 5;
            if (new_color_index != current_color_index)
            {
                current_color_index = new_color_index;
                bn::backdrop::set_color(level_colors[current_color_index]);
            }
        }

        // Increase player size at level 10
        if (score == 10)
        {
            player.set_scale(1.5);
        }
        else if (score == 20)
        {
            player.set_scale(2);
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}