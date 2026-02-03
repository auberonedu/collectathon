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
#include <bn_sound_items.h>

#include "bn_sprite_items_square.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_dragon.h"
#include "bn_sprite_items_treasure.h"
#include "common_fixed_8x16_font.h"

// change player color while boosted
#include <bn_sprite_palette_ptr.h>
#include <bn_sprite_palettes.h>

// Width and height of the player, treasure, hazard, and dragon bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {16, 16};
static constexpr bn::size HAZARD_SIZE = {8, 8};
static constexpr bn::size DRAGON_SIZE = {16, 16}; // Dragon size

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
static constexpr int HAZARD_START_X = 0;
static constexpr int HAZARD_START_Y = 0;
static constexpr int DRAGON_START_X = -80;
static constexpr int DRAGON_START_Y = -60;

static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Timer location
static constexpr int TIMER_X = 0;
static constexpr int TIMER_Y = -70;

// Boost settings
static constexpr int BOOST_MAX_USES = 3;
static constexpr int BOOST_DURATION_FRAMES = 60;
static constexpr bn::fixed BOOST_SPEED = 8;

// Slow speed
static constexpr int SLOW_DURATION_FRAMES = 90;
static constexpr bn::fixed SLOW_SPEED = 1.5;

// Timer settings
static constexpr int TIMER_MAX_FRAMES = 900;

// Dragon chase speed
static constexpr bn::fixed DRAGON_SPEED = 1.2;

// Base speed - changed from static constexpr to regular variable
static bn::fixed SPEED = 2;

int main()
{
    bn::core::init();

    // bg color
    bn::bg_palettes::set_transparent_color(bn::color(7, 2, 8)); // dark purple

    bn::random rng;

    // Will hold the sprites for the score and timer
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites;
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> timer_sprites;
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;
    int timer_frames = TIMER_MAX_FRAMES;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::treasure.create_sprite(TREASURE_START_X, TREASURE_START_Y);
    bn::sprite_ptr hazard = bn::sprite_items::dot.create_sprite(HAZARD_START_X, HAZARD_START_Y);
    bn::sprite_ptr dragon = bn::sprite_items::dragon.create_sprite(DRAGON_START_X, DRAGON_START_Y);

    bool paused = true;

    bn::vector<bn::sprite_ptr, 8> score_text_sprites;
    bn::vector<bn::sprite_ptr, 32> paused_sprites;
    bn::vector<bn::sprite_ptr, 24> instruction_sprites;

    // bg color when paused
    bn::bg_palettes::set_transparent_color(bn::color(31, 0, 0));

    text_generator.generate(50, -70, "Score:", score_text_sprites);
    text_generator.generate(-75, 40, "Press START to play!", paused_sprites);
    text_generator.generate(-50, 60, "A - Boost (x3)", instruction_sprites);

    // grab the player's palette
    bn::sprite_palette_ptr player_palette = player.palette();

    // speed boost
    int boost_left = BOOST_MAX_USES;
    int boost_frames_left = 0;

    // slow effect
    int slow_frames_left = 0;

    // game over flag
    bool game_over = false;

    while (true)
    {
        // Pauses the game
        if (paused == true)
        {
            paused_sprites.clear();
            instruction_sprites.clear();
            text_generator.generate(-75, 40, "Press START to play!", paused_sprites);
            text_generator.generate(-50, 60, "A - Boost (x3)", instruction_sprites);
        }
        if (bn::keypad::start_pressed())
        {
            paused = false;
            
            player.set_position(PLAYER_START_X, PLAYER_START_Y);
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            hazard.set_position(HAZARD_START_X, HAZARD_START_Y);
            dragon.set_position(DRAGON_START_X, DRAGON_START_Y);

            score = 0;
            timer_frames = TIMER_MAX_FRAMES;
            boost_left = BOOST_MAX_USES;
            boost_frames_left = 0;
            slow_frames_left = 0;
            game_over = false;

            player.set_visible(true);
            dragon.set_visible(true);
            bn::sprite_palettes::set_fade(bn::color(31, 31, 31), 0);
            
            // Clear pause screen
            paused_sprites.clear();
            instruction_sprites.clear();
        }

        // Only update game if not paused and not game over
        if (!paused && !game_over)
        {
            // Decrease timer
            if (timer_frames > 0)
            {
                --timer_frames;
            }
            else
            {
                // Timer reached 0 - game over!
                game_over = true;
                bn::sound_items::death.play();                           // Play death sound
                bn::sprite_palettes::set_fade(bn::color(31, 0, 0), 0.7); // Red fade
            }

            // boost when press A
            if (bn::keypad::a_pressed() && boost_left > 0 && boost_frames_left == 0)
            {
                --boost_left;
                boost_frames_left = BOOST_DURATION_FRAMES;
            }

            // choose base speed
            bn::fixed current_speed = SPEED;

            // Check if slow
            if (slow_frames_left > 0)
            {
                current_speed = SLOW_SPEED;

                // Blue fade
                bn::sprite_palettes::set_fade(bn::color(0, 0, 31), 0.4);

                --slow_frames_left;
            }

            // Boost
            if (boost_frames_left > 0)
            {
                current_speed = BOOST_SPEED;

                // blink effect
                player.set_visible((boost_frames_left / 5) % 2 == 0);

                // Orange fade
                bn::sprite_palettes::set_fade(bn::color(31, 16, 0), 0.5);

                --boost_frames_left;
            }
            else if (slow_frames_left == 0)
            {
                player.set_visible(true);
                // reset palette effect when not boosted or slowed
                bn::sprite_palettes::set_fade(bn::color(31, 31, 31), 0);
            }

            // Horizontal move: use current_speed
            if (bn::keypad::left_held())
            {
                player.set_x(player.x() - current_speed);
            }
            if (bn::keypad::right_held())
            {
                player.set_x(player.x() + current_speed);
            }

            // Vertical move
            bn::fixed vertical_speed = current_speed * 1.5;

            if (bn::keypad::up_held())
            {
                player.set_y(player.y() - vertical_speed);
            }
            if (bn::keypad::down_held())
            {
                player.set_y(player.y() + vertical_speed);
            }

            // loop for min
            if (player.x() < MIN_X)
            {
                player.set_x(MAX_X);
            }
            else if (player.x() > MAX_X)
            {
                player.set_x(MIN_X);
            }

            if (player.y() < MIN_Y)
            {
                player.set_y(MAX_Y);
            }
            else if (player.y() > MAX_Y)
            {
                player.set_y(MIN_Y);
            }

            // DRAGON CHASING LOGIC
            // Calculate direction from dragon to player
            bn::fixed dx = player.x() - dragon.x();
            bn::fixed dy = player.y() - dragon.y();

            // Normalize and move dragon toward player
            bn::fixed distance = bn::sqrt(dx * dx + dy * dy);
            if (distance > 0)
            {
                dragon.set_x(dragon.x() + (dx / distance) * DRAGON_SPEED);
                dragon.set_y(dragon.y() + (dy / distance) * DRAGON_SPEED);
            }

            // The bounding boxes of the player, treasure, hazard, and dragon
            bn::rect player_rect = bn::rect(player.x().round_integer(),
                                            player.y().round_integer(),
                                            PLAYER_SIZE.width(),
                                            PLAYER_SIZE.height());
            bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                              treasure.y().round_integer(),
                                              TREASURE_SIZE.width(),
                                              TREASURE_SIZE.height());

            bn::rect hazard_rect = bn::rect(hazard.x().round_integer(),
                                            hazard.y().round_integer(),
                                            HAZARD_SIZE.width(),
                                            HAZARD_SIZE.height());

            bn::rect dragon_rect = bn::rect(dragon.x().round_integer(),
                                            dragon.y().round_integer(),
                                            DRAGON_SIZE.width(),
                                            DRAGON_SIZE.height());

            // If the bounding boxes overlap with treasure, set the treasure to a new location and increase score
            if (player_rect.intersects(treasure_rect))
            {
                // Jump to any random point in the screen
                int new_x = rng.get_int(MIN_X, MAX_X);
                int new_y = rng.get_int(MIN_Y, MAX_Y);
                treasure.set_position(new_x, new_y);

                ++score;
            }

            if (player_rect.intersects(hazard_rect))
            {
                // slow affect applies
                slow_frames_left = SLOW_DURATION_FRAMES;

                int new_x = rng.get_int(MIN_X, MAX_X);
                int new_y = rng.get_int(MIN_Y, MAX_Y);
                hazard.set_position(new_x, new_y);
            }

            // Dragon catches player - GAME OVER!
            if (player_rect.intersects(dragon_rect))
            {
                game_over = true;
                bn::sound_items::death.play();                           // Play death sound
                bn::sprite_palettes::set_fade(bn::color(31, 0, 0), 0.7); // Red fade
            }
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y, score_string, score_sprites);

        // Update timer display (convert frames to seconds)
        int seconds = timer_frames / 60;
        bn::string<MAX_SCORE_CHARS> timer_string = bn::to_string<MAX_SCORE_CHARS>(seconds);
        timer_sprites.clear();
        text_generator.generate(TIMER_X, TIMER_Y, timer_string, timer_sprites);

        // Display boost left
        bn::string<2> boost_string = bn::to_string<1>(boost_left);
        bn::vector<bn::sprite_ptr, 2> boost_sprites;
        text_generator.generate(-80, -70, boost_string, boost_sprites);

        // Update RNG seed so no more than one game is the same
        rng.update();

        bn::core::update();
    }
}