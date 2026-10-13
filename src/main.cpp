#include <bn_backdrop.h>
#include <bn_blending.h>
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

// butano's version of vector
#include <bn_vector.h>

#include "bn_sprite_items_treasure.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

#include "bn_sprite_items_hot.h"
#include "bn_sprite_items_badfrog.h"

// Pixels / Frame player moves at
static constexpr bn::fixed NORMAL_SPEED = 2;
static constexpr bn::fixed BOOST_SPEED = 4;
static constexpr bn::fixed SLOW_SPEED = 1;
static bn::fixed current_speed = NORMAL_SPEED;

// max number of frogs
static constexpr int MAX_FROGS = 16;

// Width and height of the the player and treasure bounding boxes and frog
static constexpr bn::size PLAYER_SIZE = {12, 12};
static constexpr bn::size TREASURE_SIZE = {12, 12};
static constexpr bn::size FROG_SIZE = {12, 12};

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

// Player start location
static constexpr int PLAYER_INITIAL_X = 0;
static constexpr int PLAYER_INITIAL_Y = 0;

// Treasure start location
static constexpr int TREASURE_INITIAL_X = 0;
static constexpr int TREASURE_INITIAL_Y = -25;

//% chance that a FROG will spawn when you hit a treasure while boosting
static constexpr int FROG_SPAWN_CHANCE = 75;

// minimum distance between spawned objects
static constexpr int SPAWN_DISTANCE = 30;

static constexpr int ROUND_LENGTH = 5400;

bn::rect get_sprite_bounding_box(bn::sprite_ptr sprite, bn::size size)
{
    return bn::rect(sprite.x().round_integer(), sprite.y().round_integer(), size.width(), size.height());
}

int main()
{
    bn::core::init();

    bn::random rng = bn::random();

    bn::backdrop::set_color(bn::color(5, 0, 8));

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    int boost_timer = 0; // create a counter variable

    int slow_timer = 0;

    // pressing _A will start a "counter" if boosts is greater than 0;
    // pressing _A will subtract 1 from the boosts if greater than 0;
    // plan to make two seperate paths for movement
    // make a new speed variable to hold boost

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_INITIAL_X, PLAYER_INITIAL_Y);
    bn::sprite_ptr treasure = bn::sprite_items::treasure.create_sprite(TREASURE_INITIAL_X, TREASURE_INITIAL_Y);

    // vector to hold our badfrogs (up to 5 currently)
    bn::vector<bn::sprite_ptr, MAX_FROGS> frogs;

    // make 3 boost squares
    bn::sprite_ptr boost_1 = bn::sprite_items::hot.create_sprite(-110, -70);
    bn::sprite_ptr boost_2 = bn::sprite_items::hot.create_sprite(-100, -70);
    bn::sprite_ptr boost_3 = bn::sprite_items::hot.create_sprite(-90, -70);

    // These sprites are static so we only need to get the bounding box once
    bn::rect boost_1_rect = get_sprite_bounding_box(boost_1, PLAYER_SIZE);
    bn::rect boost_2_rect = get_sprite_bounding_box(boost_2, PLAYER_SIZE);
    bn::rect boost_3_rect = get_sprite_bounding_box(boost_3, PLAYER_SIZE);

    bn::sprite_ptr boost_sprites[3] = {boost_1, boost_2, boost_3};
    bn::rect boost_sprite_rects[3] = {boost_1_rect, boost_2_rect, boost_3_rect};

    int boost_count = 3;
    int boost_cap = 360;

    int round_timer = ROUND_LENGTH;

    while (true)
    {
        if (round_timer > 0)
        {
            bool decrease_round_timer = !(round_timer == 3600);
            // Move player with d-pad
            if (bn::keypad::left_held())
            {
                player.set_x(player.x() - current_speed);
                decrease_round_timer = true;
            }
            if (bn::keypad::right_held())
            {
                player.set_x(player.x() + current_speed);
                decrease_round_timer = true;
            }
            if (bn::keypad::up_held())
            {
                player.set_y(player.y() - current_speed);
                decrease_round_timer = true;
            }
            if (bn::keypad::down_held())
            {
                player.set_y(player.y() + current_speed);
                decrease_round_timer = true;
            }
            if (decrease_round_timer)
            {
                round_timer--;
            }
        }
        else
        {
            // Recycle score display for game over text
            bn::string_view game_over_text[3] = {"GAME OVER", "Final score: " + bn::to_string<17>(score), "START: Play again"};
            int start_x = 0;
            int start_y = -20;
            score_sprites.clear();
            text_generator.set_center_alignment();
            for (int i = 0; i < 3; i++)
            {
                text_generator.generate(start_x, start_y + (i * 16), game_over_text[i], score_sprites);
            }
        }

        if (bn::keypad::a_pressed())
        {
            if (boost_count > 0 && boost_timer == 0)
            {
                boost_count--;
                boost_cap = 360;
                boost_timer += 360; // (add 6 seconds of boost)
                current_speed = BOOST_SPEED;
                boost_sprites[boost_count].set_blending_enabled(true);
                // set player sprite to hot sprite
                player.set_item(bn::sprite_items::hot);
            }
        }
        // restart game button
        if (bn::keypad::start_pressed())
        {
            player.set_position(PLAYER_INITIAL_X, PLAYER_INITIAL_Y);
            treasure.set_position(TREASURE_INITIAL_X, TREASURE_INITIAL_Y);
            score = 0;
            current_speed = NORMAL_SPEED;
            boost_timer = 0;
            boost_count = 3;
            round_timer = ROUND_LENGTH;
            bn::blending::set_transparency_alpha(1);
            for (int i = 0; i < 3; i++)
            {
                boost_sprites[i].set_visible(true);
                boost_sprites[i].set_blending_enabled(false);
            }
            player.set_item(bn::sprite_items::square);
            text_generator.set_left_alignment();
            score_sprites.clear();
            bn::string<1> score_string = "0";
            text_generator.generate(SCORE_X, SCORE_Y, score_string, score_sprites);
        }

        // logic for crossing outside of screen bounds
        // move player to other side if above / below max / min
        if (player.x() > MAX_X || player.x() < MIN_X)
        {
            player.set_x(player.x().round_integer() * -1);
        }
        if (player.y() > MAX_Y || player.y() < MIN_Y)
        {
            player.set_y(player.y().round_integer() * -1);
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = get_sprite_bounding_box(player, PLAYER_SIZE);
        bn::rect treasure_rect = get_sprite_bounding_box(treasure, TREASURE_SIZE);

        // since there are going to be multiple frogs we have to loop through them to check if theres collision
        // frog collision
        for (int i = 0; i < frogs.size(); i++)
        {
            bn::rect frog_rect = get_sprite_bounding_box(frogs[i], FROG_SIZE);

            // check if the player has touched this frog
            if (player_rect.intersects(frog_rect))
            {
                // Cancel the boost
                boost_timer = 0;
                slow_timer = 180;
                current_speed = SLOW_SPEED;
                if (boost_count < 3)
                {
                    boost_sprites[boost_count].set_visible(false);
                    boost_sprites[boost_count].set_blending_enabled(false);
                }
                // Reset transparency
                bn::blending::set_transparency_alpha(1);
                player.set_visible(true);
                player.set_item(bn::sprite_items::square);

                // get rid of all the frogs
                frogs.clear();
                // end the loop
                break;
            }
        }

        // If player collects treasure, increase score, spawn new treasure and potentially a frog
        if (player_rect.intersects(treasure_rect))
        {
            // If there's room for frogs (5 max) // if boosting // and we pass 75% spawn chance
            if (frogs.size() < MAX_FROGS && rng.get_int(0, 100) < FROG_SPAWN_CHANCE && boost_timer > 0)
            {
                // Try up to 10 times to find a safe frog location (don't want to loop too long)
                for (int attempt = 0; attempt < 10; attempt++)
                {
                    // Pick a random location for the frog
                    int frog_x = rng.get_int(MIN_X, MAX_X);
                    int frog_y = rng.get_int(MIN_Y, MAX_Y);

                    // Check if frog is too close to player against coordinates, set true if clear
                    bool too_close = (bn::abs(frog_x - player.x()) < SPAWN_DISTANCE &&
                                      bn::abs(frog_y - player.y()) < SPAWN_DISTANCE);

                    // If location is safe, create frog
                    if (!too_close)
                    {
                        frogs.push_back(bn::sprite_items::badfrog.create_sprite(frog_x, frog_y));
                        break; // Stop looking once we've spawned a frog
                    }
                }
            }

            // Try up to 30 times to find a safe treasure location
            bool treasure_spawned = false;

            for (int attempt = 0; attempt < 30; attempt++)
            {
                int new_x = rng.get_int(MIN_X, MAX_X);
                int new_y = rng.get_int(MIN_Y, MAX_Y);

                bool too_close = false;

                // Check the new location against every frog
                for (int i = 0; i < frogs.size(); i++)
                {
                    if (bn::abs(new_x - frogs[i].x()) < SPAWN_DISTANCE &&
                        bn::abs(new_y - frogs[i].y()) < SPAWN_DISTANCE)
                    {
                        too_close = true;
                        break;
                    }
                }

                // Create a bounding box for the new treasure
                bn::rect new_treasure_bounding_box = bn::rect(new_x, new_y, TREASURE_SIZE.width(), TREASURE_SIZE.height());

                // And check if it's behind the boost squares
                for (int i = 0; i < 3; i++)
                {
                    if (new_treasure_bounding_box.intersects(boost_sprite_rects[i]) && boost_sprites[i].visible())
                    {
                        too_close = true;
                        break;
                    }
                }

                // Found a safe location!
                if (!too_close)
                {
                    treasure.set_position(new_x, new_y);
                    treasure_spawned = true;
                    break;
                }
            }

            // If all 20 attempts failed, remove frogs and spawn treasure anyway
            if (!treasure_spawned)
            {
                frogs.clear();

                int new_x = rng.get_int(MIN_X, MAX_X);
                int new_y = rng.get_int(MIN_Y, MAX_Y);

                treasure.set_position(new_x, new_y);
            }

            // If player hits treasure while boosted, add 2 seconds
            if (boost_timer > 0 && boost_timer < 360)
            {
                boost_timer += 120;

                if (boost_timer > boost_cap)
                {
                    boost_cap = boost_timer;
                }

                bn::blending::set_transparency_alpha(bn::min(bn::fixed(1), bn::fixed(boost_timer) / bn::fixed(boost_cap)));
            }

            score++;

            // Update score display
            bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
            score_sprites.clear();
            text_generator.generate(SCORE_X, SCORE_Y,
                                    score_string,
                                    score_sprites);
        }

        // reduce boost timer
        if (boost_timer > 0)
        {
            bn::sprite_ptr boost_sprite = boost_sprites[boost_count];
            bn::fixed current_alpha = bn::blending::transparency_alpha();
            bn::blending::set_transparency_alpha(bn::max(bn::fixed(0), current_alpha - (1 / bn::fixed(boost_timer)))); // Gradually fade out boost indicator as boost expires
            if (boost_timer <= 60)                                                                                     // 1 second or less of boost left
            {
                if (boost_timer % 30 == 0) // Make player sprite blink every quarter-second
                {
                    player.set_visible(false);
                }
                else if (boost_timer % 15 == 0)
                {
                    player.set_visible(true);
                }
            }
            boost_timer--;
            boost_cap--;
            if (boost_timer == 0)
            {
                boost_sprite.set_visible(false);
                boost_sprite.set_blending_enabled(false);
                bn::blending::set_transparency_alpha(1);
                current_speed = NORMAL_SPEED;
                player.set_item(bn::sprite_items::square);

                // when a boost expires go ahead and clear the frogs!
                frogs.clear();
            }
        }

        // slow timer countdown
        if (slow_timer > 0)
        {
            if (slow_timer % 30 == 0) // Make player sprite blink every quarter-second
            {
                player.set_visible(false);
            }
            else if (slow_timer % 15 == 0)
            {
                player.set_visible(true);
            }
            slow_timer--;
            if (slow_timer == 0)
            {
                current_speed = NORMAL_SPEED;
            }
        }

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}