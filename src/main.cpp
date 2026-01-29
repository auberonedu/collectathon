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

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 3;

// Width and height of the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// STARTING POSITIONS (new static constexpr)
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;
static constexpr int TREASURE_START_X = 50;
static constexpr int TREASURE_START_Y = -20;

// Number of characters required to show the longest number possible in an int
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Boost settings
static constexpr int BOOST_MAX_USES = 3;
static constexpr int BOOST_DURATION_FRAMES = 60;
static constexpr bn::fixed BOOST_SPEED = 6;

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

    // grab the player's palette 
    bn::sprite_palette_ptr player_palette = player.palette();

    // speed boost
    int boost_left = BOOST_MAX_USES;
    int boost_frames_left = 0;

    while(true)
    {
        if(bn::keypad::start_pressed())
        {
            player.set_position(PLAYER_START_X, PLAYER_START_Y);
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            score = 0;
            boost_left = BOOST_MAX_USES;
            boost_frames_left = 0;

            player.set_visible(true);
            bn::sprite_palette_fade_manager::set_intensity(0);
        }

        // boost when press A
        if(bn::keypad::a_pressed() && boost_left > 0 && boost_frames_left == 0)
        {
            --boost_left;
            boost_frames_left = BOOST_DURATION_FRAMES;
        }

        // choose base speed
        bn::fixed current_speed = SPEED;

        if(boost_frames_left > 0)
        {
            current_speed = BOOST_SPEED;

            // blink effect
            player.set_visible((boost_frames_left / 5) % 2 == 0);

            // add a palette effect
            bn::sprite_palettes::set_fade_intensity(0.3);   
            --boost_frames_left;
        }
        else
        {
            player.set_visible(true);
            // reset palette effect when not boosted
            bn::sprite_palettes::set_fade_intensity(0);
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

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location and increase score
        if(player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            ++score;
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
