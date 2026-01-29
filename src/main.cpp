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
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;

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
static constexpr int SCORE_X = -85;
static constexpr int SCORE_Y = -50;

// Start location of player sprite
static constexpr int startPosX = 0;
static constexpr int startPosY = 0;
int boost = 0;
static constexpr int boostSpeedMax = 10;
static constexpr int useableBoostsMax = 3;
int curBoosts = useableBoostsMax;

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
    int start_time = 30; 
    int time = start_time;
    bn::timer timer;
    uint64_t ticks = 0;

    bn::vector<bn::sprite_ptr, 64> text_sprites;
    text_generator.set_center_alignment();

    int score = 0;

    // bn::sprite_ptr player = bn::sprite_items::square.create_sprite(-50, 50);
    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(startPosX, startPosY); // KJeans Changed

    bn::sprite_ptr treasure = bn::sprite_items::coin.create_sprite(0, 0);

    while (true)

    {
        // KJans added speed boost;
        if (bn::keypad::a_pressed())
        {
            if (boost == 0 && curBoosts > 0)
            {
                boost = boostSpeedMax;
            }
        }
        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - (SPEED + boost));
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + (SPEED + boost));
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - (SPEED + boost));
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + (SPEED + boost));
        }

        // Boost must adjust value after movement event occurs
        if (boost > 0)
        {
            boost--; // decrement boost per frame
            if (boost == 0)
            {
                curBoosts--;
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
            treasure.set_position(0, 0);
            player.set_position(startPosX, startPosY);
            score = 0;
            curBoosts = useableBoostsMax; // reset boosts
            ticks = 0;
            timer.restart();
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

            // kjeans added funtionality so that every 2 points the player gets another boost
            if (score % 2 == 0 && score != 0)
            {
                curBoosts++;
            }
        }
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
            if (bn::keypad::start_pressed())
            {
                treasure.set_position(0, 0);
                player.set_position(startPosX, startPosY);
                score = 0;
                curBoosts = useableBoostsMax; // reset boosts
                ticks = 0;
                timer.restart();
                seconds = start_time;
            }
            text_sprites.clear();
            text_generator.generate(0, 0, "Score: " + bn::to_string<MAX_SCORE_CHARS>(score), text_sprites);
            text_generator.generate(0,-16, "Press START to play again", text_sprites);
            bn::core::update();
            text_sprites.clear();
        }

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        // logs player position each update
        // BN_LOG("(", player.x(), ",", player.y(), ")");

        bn::core::update();
    }
}