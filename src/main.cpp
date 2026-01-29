#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_sprite_tiles_ptr.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_backdrop.h>
#include <bn_color.h>

#include "bn_sprite_items_oyster.h"
#include "bn_sprite_items_crab.h"
#include "bn_sound_items.h"
#include "bn_sound.h"
#include "common_fixed_8x16_font.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 1.5;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

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

// Speed boost location
static constexpr int BOOST_X = -70;
static constexpr int BOOST_Y = -70;

// Speed boost parameters
static int BOOSTS_LEFT = 3;
static int BOOST_DURATION_FRAMES = 0;

// Player location
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;

// Treasure location
static constexpr int TREASURE_START_X = 0;
static constexpr int TREASURE_START_Y = 0;

int main()
{
    bn::core::init();

    bn::backdrop::set_color(bn::color(0, 0, 15));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> boost_sprites = {};
    bn::sprite_text_generator boost_generator(common::fixed_8x16_sprite_font);

    bn::sprite_ptr player = bn::sprite_items::crab.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::oyster.create_sprite(TREASURE_START_X,
                                                                  TREASURE_START_Y);

    while (true)
    {
        bn::fixed speed = SPEED;

        // Speed boost set to A button
        if (bn::keypad::a_pressed() && BOOSTS_LEFT > 0 && BOOST_DURATION_FRAMES <= 0)
        {
            bn::sound_items::zoom.play(); //play zoom sound
            BOOST_DURATION_FRAMES = 180;
            BOOSTS_LEFT--;
        }

        if (BOOST_DURATION_FRAMES > 0)
        {
            speed = SPEED * 3;
            BOOST_DURATION_FRAMES--;
        }

        else
        {
            speed = SPEED;
        }

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - speed);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + speed);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - speed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + speed);
        }

        // Start of animation
        if(bn::keypad::right_held())
        {
            player.set_tiles(bn::sprite_items::crab.tiles_item().create_tiles(0));
        }
        else if(bn::keypad::left_held())
        {
            player.set_tiles(bn::sprite_items::crab.tiles_item().create_tiles(2));
        }
        else if(bn::keypad::up_held())
        {
            player.set_tiles(bn::sprite_items::crab.tiles_item().create_tiles(4));
        }
        else if(bn::keypad::down_held())
        {
            player.set_tiles(bn::sprite_items::crab.tiles_item().create_tiles(6));
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
            bn::sound_items::collect.play(); // play collect sound

            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
        }

        // On start press, the game resets and puts everything back to initial state
        if (bn::keypad::start_pressed())
        {
            score = 0;
            BOOSTS_LEFT = 3;
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            player.set_position(PLAYER_START_X, PLAYER_START_Y);
        }

        // Implement loop behavior on screen
        if (player.x() <= MIN_X && bn::keypad::left_held())
        {
            player.set_x(MAX_X);
        }

        if (player.x() >= MAX_X && bn::keypad::right_held())
        {
            player.set_x(MIN_X);
        }

        if (player.y() <= MIN_Y && bn::keypad::up_held())
        {
            player.set_y(MAX_Y);
        }

        if (player.y() >= MAX_Y && bn::keypad::down_held())
        {
            player.set_y(MIN_Y);
        }

        //Update boost amount
        bn::string<MAX_SCORE_CHARS> boost_string = bn::to_string<MAX_SCORE_CHARS>(BOOSTS_LEFT);
        boost_sprites.clear();
        text_generator.generate(BOOST_X, BOOST_Y,
                                boost_string,
                                boost_sprites);

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