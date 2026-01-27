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
#include <bn_vector.h>
#include <bn_sound_item.h>
#include <bn_audio.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_enemy.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;
static constexpr bn::fixed ENEMY_SPEED = 1;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {16, 16};

// Player starting location
static constexpr int PLAYER_X = 0;
static constexpr int PLAYER_Y = 0;
// Enemy start locatin
static constexpr int ENEMY_X = -40;
static constexpr int ENEMY_Y = -40;

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

    // Backdrop color
    bn::backdrop::set_color(bn::color(31, 0, 0));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    // BOST VERIABLES
    int Speed_boost = 3;   // AMOUNT OF BOOSTS
    int BOOST = 0;         // INCREASE IN SPEED
    int boost_counter = 0; // TIME BEFORE BOOST ENDS

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_X, PLAYER_Y);

    bn::sprite_ptr enemy = bn::sprite_items::enemy.create_sprite(ENEMY_X, ENEMY_Y);

    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(0, 0);

    while (true)
    {
        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - SPEED - BOOST);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + SPEED + BOOST);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - SPEED - BOOST);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + SPEED + BOOST);
        }

        // // Places the player on the other side of the screen
        // if (player.x() <= MIN_X || player.x() >= MAX_X)
        // {
        //     player.set_x(-player.x());
        // }

        // if(player.y() <= MIN_Y || player.y() >= MAX_Y){
        //     player.set_y(-player.y());
        // }

        // Touching border resets game
        if (player.x() == MIN_X || player.x() == MAX_X || player.y() == MIN_Y || player.y() == MAX_Y)
        {
            score = 0;
            player.set_x(PLAYER_X);
            player.set_y(PLAYER_Y);
            enemy.set_x(ENEMY_X);
            enemy.set_y(ENEMY_Y);

            // places orbs in random spots
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            // Resets boosts
            Speed_boost = 3;
        }
        // Restart the game when pressed START
        if (bn::keypad::start_pressed())
        {
            score = 0;
            player.set_x(PLAYER_X);
            player.set_y(PLAYER_Y);

            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            // resets boosts
            Speed_boost = 3;
        }

        // BOOST
        if (bn::keypad::a_pressed() && Speed_boost > 0)
        {
            Speed_boost--;      // HOW MANY BOOST FOR GAME
            boost_counter = 30; // HOW MANY FRAMES BEFORE BOOST ENDS
            BOOST = 2;
        }
        if (boost_counter > 0)
        {
            boost_counter--;
        }
        else
        {
            BOOST = 0;
        }

        // ENEMY MOVEMENT
        if (enemy.y() < player.y())
        {
            enemy.set_y(enemy.y() + ENEMY_SPEED);
        }
        else
        {
            enemy.set_y(enemy.y() - ENEMY_SPEED);
        }
        if (enemy.x() < player.x())
        {
            enemy.set_x(enemy.x() + ENEMY_SPEED);
        }
        else
        {
            enemy.set_x(enemy.x() - ENEMY_SPEED);
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
        bn::rect enemy_rect = bn::rect(enemy.y().round_integer(), enemy.y().round_integer(), ENEMY_SIZE.width(), ENEMY_SIZE.height());

        // ENEMY
        if (player_rect.intersects(enemy_rect))
        {
            score = 0;
            player.set_x(PLAYER_X);
            player.set_y(PLAYER_Y);
            enemy.set_x(ENEMY_X);
            enemy.set_y(ENEMY_Y);

            // places orbs in random spots
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            // Resets boosts
            Speed_boost = 3;
        }

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            // bn::sound_item::alert.play();
            score++;
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