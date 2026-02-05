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

#include "bn_timer.h"
#include "bn_timers.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_coin.h"
#include "bn_sprite_items_charactersprite.h"
#include "bn_sprite_items_hourglass.h"

#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {10, 10};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = -85;
static constexpr int SCORE_Y = -50;

// Start location of player sprite
static constexpr int startPosX = 0;
static constexpr int startPosY = 0;
// boost speed values
int boostSpeed = 0;
static constexpr int boostSpeedMax = 10;
// Amount of boosts values
static constexpr int useableBoostsMax = 3;
int availableBoosts = useableBoostsMax;

int main()
{
    bn::core::init();

    // change backdrop color
    bn::backdrop::set_color(bn::color(20, 20, 31));
    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, 32> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    // timer variables
    bn::vector<bn::sprite_ptr, 32> time_sprites = {};
    int start_time = 15;
    int time = start_time;
    bn::timer timer;
    uint64_t ticks = 0;

    bn::vector<bn::sprite_ptr, 128> text_sprites;
    text_generator.set_center_alignment();

    int score = 0;

    bn::sprite_ptr player = bn::sprite_items::charactersprite.create_sprite(startPosX, startPosY); // KJeans Changed

    bn::sprite_ptr timeBoost = bn::sprite_items::hourglass.create_sprite(1000, 1000);
    bool isSpawnedTB = false;
    int timeBoostChance = 0;

    int newCoinX = rng.get_int(MIN_X, MAX_X);
    int newCoinY = rng.get_int(MIN_Y, MAX_Y);
    bn::sprite_ptr treasure = bn::sprite_items::coin.create_sprite(newCoinX, newCoinY);

    // indicator variables
    int currentTime = 0;
    bn::vector<bn::sprite_ptr, 32> indicator_sprites = {};

    // high score variables
    int high_score = 0;

    while (true)

    {
        // add timer
        ticks += timer.elapsed_ticks_with_restart();
        int64_t seconds = time - (ticks / bn::timers::ticks_per_second());
        if (seconds < 0)
            seconds = 0;
        bn::string<32> time_string("Time Remaining:");
        time_string.append(bn::to_string<MAX_SCORE_CHARS>(seconds));
        time_sprites.clear();
        text_generator.generate(-50, -70,
                                time_string,
                                time_sprites);

        // -KJeans added speed boostSpeed;
        if (bn::keypad::a_pressed())
        {
            if (boostSpeed == 0 && availableBoosts > 0)
            {
                boostSpeed = boostSpeedMax;
            }
        }
        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - (SPEED + boostSpeed));
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + (SPEED + boostSpeed));
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - (SPEED + boostSpeed));
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + (SPEED + boostSpeed));
        }

        // KJeans- boostSpeed must adjust value after movement event occurs
        if (boostSpeed > 0)
        {
            boostSpeed--; // KJeans- decrement boostSpeed per frame

            if (boostSpeed == 0) // KJeans- this means 1 boostSpeed has finished its cycle and should decrement available boostd
            {
                availableBoosts--;
            }
        }

        // loop the plaer if they go off screen

        // x-axis
        if (player.x() >= 120)
        {
            player.set_x(-120);
        }
        else if (player.x() <= -120)
        {
            player.set_x(120);
        }

        // y-axis
        if (player.y() >= 80)
        {
            player.set_y(-80);
        }
        else if (player.y() <= -80)
        {
            player.set_y(80);
        }

        // add restart button
        if (bn::keypad::start_pressed())
        {
            // Kjeans- creates rng values to spawn treasure on restart
            int newCoin_x = rng.get_int(MIN_X, MAX_X);
            int newCoin_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(newCoin_x, newCoin_y);

            player.set_position(startPosX, startPosY);
            score = 0;
            availableBoosts = useableBoostsMax; // reset boosts
            ticks = 0;
            timer.restart();
            indicator_sprites.clear();

            bn::backdrop::set_color(bn::color(20, 20, 31));
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
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;

            // KJeans- added funtionality so that every 2 points the player gets another available boost
            if (score % 2 == 0 && score != 0)
            {
                availableBoosts++;
            }

            // point indicator
            currentTime = seconds;
            bn::string<32> indicator_string("+1");
            indicator_sprites.clear();
            text_generator.generate(player.x(), player.y(),
                                    indicator_string,
                                    indicator_sprites);
        }

        // clare point indicator after 1 second
        if (currentTime - seconds > 1)
        {
            indicator_sprites.clear();
        }

        /// KJeans  TIME BOOST LOGIC BEGIN///////////////////////////////////////////////////////////////////////
        if (!isSpawnedTB) // verify a boost is not already spawned
        {
            timeBoostChance = rng.get_int(1, 100000);
        }
        if (timeBoostChance > 99700 && !isSpawnedTB) // if valid spawnchance value, spawn in a timeBoost and set isSpawnedTB bool true to prevent more spawning
        {

            int newX = rng.get_int(MIN_X, MAX_X);
            int newY = rng.get_int(MIN_Y, MAX_Y);
            timeBoost.set_position(newX, newY);

            isSpawnedTB = true;
        }
        // Collision logic
        bn::rect timeBoost_rect = bn::rect(timeBoost.x().round_integer(), timeBoost.y().round_integer(), TREASURE_SIZE.width(), TREASURE_SIZE.height());

        if (player_rect.intersects(timeBoost_rect))
        {
            seconds += 3;
            timeBoost.set_position(1000, 1000); // Spawns offscreen because i dont know how to delete
            isSpawnedTB = false;
        }
        /// TIME BOOST LOGIC END////////////////////////////////////////////////////////////////////

        
        // KJeans - boost text
        text_sprites.clear();
        bn::string<32> boost_string("Boosts:");
        boost_string += bn::to_string<MAX_SCORE_CHARS>(availableBoosts);
        text_generator.generate(SCORE_X + 3, SCORE_Y + 12, boost_string, text_sprites);

        // Update score display
        bn::string<32> score_string("Score:");
        score_string.append(bn::to_string<MAX_SCORE_CHARS>(score));
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // add end game screen and restart button
        while (seconds == 0)
        {
            bn::backdrop::set_color(bn::color(0, 0, 0));
            indicator_sprites.clear();
            time_sprites.clear();
            score_sprites.clear();
            text_sprites.clear();
            // update high score
            if (score > high_score)
            {
                high_score = score;
            }
            if (bn::keypad::start_pressed())
            {
                // Kjeans- creates rng values to spawn treasure on restart
                int newCoin_x = rng.get_int(MIN_X, MAX_X);
                int newCoin_y = rng.get_int(MIN_Y, MAX_Y);
                treasure.set_position(newCoin_x, newCoin_y);

                player.set_position(startPosX, startPosY);
                score = 0;
                availableBoosts = useableBoostsMax; // reset boosts
                ticks = 0;
                timer.restart();
                seconds = start_time;

                bn::backdrop::set_color(bn::color(20, 20, 31));
            }
            //display score
            bn::string<32> end_score_string("Score: ");
            end_score_string.append(bn::to_string<MAX_SCORE_CHARS>(score));
            text_generator.generate(0, 0, end_score_string, text_sprites);
            text_generator.generate(0, -16, "Press START to play again", text_sprites);
            //display high score
            bn::string<32> end_highscore_string("High Score: ");
            end_highscore_string.append(bn::to_string<MAX_SCORE_CHARS>(high_score));
            text_generator.generate(0, 16, end_highscore_string, text_sprites);
  
            bn::core::update();
            
        }

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        // logs player position each update
        // BN_LOG("(", player.x(), ",", player.y(), ")");
        // BN_LOG(currentTime - seconds);
        BN_LOG(start_time);

        bn::core::update();
    }
}

// commit comment
//  added random treasure spawn on restart
//  changed boost related value names to represent more accurately what they do
//  reset backdrop and set backdrop