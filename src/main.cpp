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
#include <bn_bg_palettes.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

// change player color while boosted
#include <bn_sprite_palette_ptr.h>
#include <bn_sprite_palettes.h>

// Pixels / Frame player moves at - INCREASED SPEED
static constexpr bn::fixed SPEED = 4;  // Changed from 3 to 4 for faster movement

// Width and height of the player, treasure, and hazard bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size HAZARD_SIZE = {8, 8};  // New: size for slow hazard

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// STARTING POSITIONS
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;
static constexpr int TREASURE_START_X = 50;
static constexpr int TREASURE_START_Y = -20;


static constexpr int HAZARD_START_X = 0;  //  hazard starting position - searched on yt
static constexpr int HAZARD_START_Y = 0;


static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Boost settings
static constexpr int BOOST_MAX_USES = 3;
static constexpr int BOOST_DURATION_FRAMES = 60;
static constexpr bn::fixed BOOST_SPEED = 8;  // Increased from 6 to 8

// Slow speed
static constexpr int SLOW_DURATION_FRAMES = 90;  // How long slow effect lasts
static constexpr bn::fixed SLOW_SPEED = 1.5;  // Speed when slowed down

int main()
{
    bn::core::init();

    // bg color
    bn::bg_palettes::set_transparent_color(bn::color(7, 2, 8)); // dark purple

    bn::random rng;

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites;
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(TREASURE_START_X, TREASURE_START_Y);
    bn::sprite_ptr hazard = bn::sprite_items::dot.create_sprite(HAZARD_START_X, HAZARD_START_Y);  // New: slow hazard sprite

    // grab the player's palette 
    bn::sprite_palette_ptr player_palette = player.palette();

    // speed boost
    int boost_left = BOOST_MAX_USES;
    int boost_frames_left = 0;
    
    // slow effect 
    int slow_frames_left = 0;

    while(true)
    {
        if(bn::keypad::start_pressed())
        {
            player.set_position(PLAYER_START_X, PLAYER_START_Y);
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);

            hazard.set_position(HAZARD_START_X, HAZARD_START_Y);  // Resets the hazard position
            score = 0;
            boost_left = BOOST_MAX_USES;
            boost_frames_left = 0;
            slow_frames_left = 0;  // Reset slow effect

            player.set_visible(true);
            bn::sprite_palettes::set_fade(bn::color(31, 31, 31), 0);
        }

        // boost when press A
        if(bn::keypad::a_pressed() && boost_left > 0 && boost_frames_left == 0)
        {
            --boost_left;
            boost_frames_left = BOOST_DURATION_FRAMES;
        }

        // choose base speed 
        bn::fixed current_speed = SPEED;

        // Check if slow
        if(slow_frames_left > 0)
        {
            current_speed = SLOW_SPEED;
            
            // Blue fade 
            bn::sprite_palettes::set_fade(bn::color(0, 0, 31), 0.4);  // Blue fade
            
            --slow_frames_left;
        }

        // Boost
        if(boost_frames_left > 0)
        {
            current_speed = BOOST_SPEED;

            // blink effect
            player.set_visible((boost_frames_left / 5) % 2 == 0);

            // Orange fade
            bn::sprite_palettes::set_fade(bn::color(31, 16, 0), 0.5);  // Orange fade
            
            --boost_frames_left;
        }
        else if(slow_frames_left == 0)  
        {
            player.set_visible(true);
            // reset palette effect when not boosted or slowed
            bn::sprite_palettes::set_fade(bn::color(31, 31, 31), 0);
        }

        // Horizontal move: use current_speed
        if(bn::keypad::left_held())
        {
            player.set_x(player.x() - current_speed);
        }
        if(bn::keypad::right_held())
        {
            player.set_x(player.x() + current_speed);
        }

        // Vertical move
        bn::fixed vertical_speed = current_speed * 1.5; 

        if(bn::keypad::up_held())
        {
            player.set_y(player.y() - vertical_speed);
        }
        if(bn::keypad::down_held())
        {
            player.set_y(player.y() + vertical_speed);
        }

        // loop for min
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

        // The bounding boxes of the player, treasure, and hazard
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        bn::rect hazard_rect = bn::rect(hazard.x().round_integer(),  //hazard bounding box
                                        hazard.y().round_integer(),
                                        HAZARD_SIZE.width(),
                                        HAZARD_SIZE.height());

        // If the bounding boxes overlap with treasure, set the treasure to a new location and increase score
        if(player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            ++score;
        }


        if(player_rect.intersects(hazard_rect))
        {
            // slow affect applies
            slow_frames_left = SLOW_DURATION_FRAMES;
            
    
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            hazard.set_position(new_x, new_y);
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y, score_string, score_sprites);

        // Display boost left
        bn::string<2> boost_string = bn::to_string<1>(boost_left);
        bn::vector<bn::sprite_ptr, 2> boost_sprites;
        text_generator.generate(-80, -70, boost_string, boost_sprites);

        // Update RNG seed so no more than one game is the same
        rng.update();

        bn::core::update();
    }
}