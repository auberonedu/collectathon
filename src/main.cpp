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
#include "bn_sprite_items_brick.h"
#include "bn_sprite_items_coin.h"

// imports/includes  all the core functions of gameboy which includes, keypad, core, sprite
// as well as the random

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size BRICK_SIZE = {16, 16};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 90;
static constexpr int SCORE_Y = -70;

// Speed Boost Location
static constexpr int SPEED_BOOST_COUNT_X = -20;
static constexpr int SPEED_BOOST_COUNT_Y = -70;
static constexpr int SPEED_BOOST_COUNT_CHARS = 11;

// Starting points for player and treasure
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;
static constexpr int TREASURE_START_X = 0;
static constexpr int TREASURE_START_Y = 0;

// High Score
static constexpr int HIGH_X = 90;
static constexpr int HIGH_Y = -60;

int main()
{
    int score = 0;
    int highScore = 0;
    // Pixels / Frame player moves at
    int SPEED = 1;
    int duration = 0;
    int boostCount = 3;

    bn::core::init();
    // #1
    bn::backdrop::set_color(bn::color(0, 0, 31));

    bn::random rng = bn::random();

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::coin.create_sprite(TREASURE_START_X, TREASURE_START_Y);

    // create wall vector
    bn::vector<bn::sprite_ptr, bn::display::width() / 16> wall = {};
    bn::vector<bn::rect, bn::display::width() / 16> wall_rect = {};

    int wall_Y = MIN_Y;

    while (true)
    {

        // Game Reset
        if (bn::keypad::start_pressed())
        {
            boostCount = 3;
            duration = 0;
            score = 0;
            player.set_x(PLAYER_START_X);
            player.set_y(PLAYER_START_Y);
            treasure.set_x(TREASURE_START_X);
            treasure.set_y(TREASURE_START_Y);
            wall_Y = MAX_Y;
        }

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - SPEED);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + SPEED);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - SPEED);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + SPEED);
        }

        // wall functionallity
        if (wall_Y == MIN_Y)
        { // if the wall at is the top
            for (int i = 0; i < bn::display::width(); i += 16)
            {
                if (score > 0 && rng.get_int(0, 10) < score / 5 + 1)
                {

                    wall.push_back(bn::sprite_items::brick.create_sprite(MIN_X + i + 8, MIN_Y + 1));
                }
            }
            for (int i = 0; i < wall.size(); i++)
            {
                wall_rect.push_back(bn::rect(wall[i].x().round_integer(),
                                             wall[i].y().round_integer(),
                                             BRICK_SIZE.width(),
                                             BRICK_SIZE.height()));
            }
            wall_Y++;
        }
        else if (wall_Y == MAX_Y)
        { // if the wall is at the bottom
            wall.clear();
            wall_rect.clear();
            wall_Y = MIN_Y;
        }
        else
        { // Move the wall
            for (int i = 0; i < wall.size(); i++)
            {
                wall[i].set_y(wall[i].y() + 1);
            }
            wall_Y++;
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

        for (int i = 0; i < wall.size(); i++)
        {
            wall_rect[i] = bn::rect(wall[i].x().round_integer(),
                                    wall[i].y().round_integer(),
                                    BRICK_SIZE.width(),
                                    BRICK_SIZE.height());
        }

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y + 20, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
            if (score > highScore) {
                highScore = score;
            }
        }

        for (int i = 0; i < wall.size(); i++)
        {
            if (player_rect.intersects(wall_rect[i]))
            {
                boostCount = 3;
                duration = 0;
                score = 0;
                player.set_x(PLAYER_START_X);
                player.set_y(PLAYER_START_Y);
                treasure.set_x(TREASURE_START_X);
                treasure.set_y(TREASURE_START_Y);
                wall_Y = MAX_Y;
            }
        }

        if (player.x() > MAX_X)
        {
            player.set_x(MIN_X);
        }
        if (player.x() < MIN_X)
        {
            player.set_x(MAX_X);
        }
        if (player.y() > MAX_Y)
        {
            player.set_y(MIN_Y);
        }
        if (player.y() < MIN_Y)
        {
            player.set_y(MAX_Y);
        }

        if (duration > 0)
        {
            SPEED = 2;
            duration--;
        }
        else
        {
            SPEED = 1;
        }

        if (bn::keypad::a_pressed() && boostCount > 0)
        {
            duration = 180;
            boostCount--;
        }

        // Will hold the sprites for the score
        bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
        bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

        bn::vector<bn::sprite_ptr, 5> score_label_sprites;
        text_generator.generate(SCORE_X - 50, SCORE_Y, "SCORE:", score_label_sprites);

        // This will hold the sprites for the speed boost count
        bn::vector<bn::sprite_ptr, 11> boost_label_sprites = {};
        bn::vector<bn::sprite_ptr, SPEED_BOOST_COUNT_CHARS> boost_sprites = {};

        text_generator.generate(SPEED_BOOST_COUNT_X - 90, SPEED_BOOST_COUNT_Y,
                                "BOOST LEFT:", boost_label_sprites);

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // added the boost display
        bn::string<SPEED_BOOST_COUNT_CHARS> boost_string = bn::to_string<SPEED_BOOST_COUNT_CHARS>(boostCount);
        boost_sprites.clear();
        // added Boost count text
        text_generator.generate(SPEED_BOOST_COUNT_X, SPEED_BOOST_COUNT_Y,
                                boost_string,
                                boost_sprites);
        bn::vector<bn::sprite_ptr, 5> high_label_sprites;
        text_generator.generate(HIGH_X - 50, HIGH_Y, "HIGH:", high_label_sprites);

        bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> high_sprites;
        bn::string<MAX_SCORE_CHARS> high_string = bn::to_string<MAX_SCORE_CHARS>(highScore);
        high_sprites.clear();
        text_generator.generate(HIGH_X, HIGH_Y, high_string, high_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();
        bn::core::update();
    }
}