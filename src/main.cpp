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
#include <bn_math.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"
#include <bn_backdrop.h>
#include <bn_color.h>

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 1.25;
static constexpr bn::fixed DIAG_SPEED = SPEED * bn::degrees_cos(45);

// Boosted speed 

static constexpr bn:fixed BOOST_SPEED = SPEED * 2;

//Boost boundaries

static constexpr int MAX_BOOSTS = 3;
static constexpr int BOOST_DURATION_FRAMES = 120;

//Variables for speed boost 
int player_speed = SPEED;

bn_fixed(payer_speed);

int boosts_left = MAX_BOOSTS;

bn_fixed(boosts_left);

int boost_timer = 0;
bool boost_active = false;


// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Starting position of player and treasure
static constexpr bn::fixed PLAYER_X = -25;
static constexpr bn::fixed PLAYER_Y = -25;
static constexpr bn::fixed TREASURE_X = 25;
static constexpr bn::fixed TREASURE_Y = 25;

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

    bn::backdrop::set_color(bn::color(20, 0, 0));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_X, PLAYER_Y);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(TREASURE_X, TREASURE_Y);

    while (true)
    {
        // Move player with d-pad

        // Multikey (nerf to diagonal movement because its faster)
        if (bn::keypad::left_held() && bn::keypad::up_held()) {
            player.set_x(player.x() - DIAG_SPEED);
            player.set_y(player.y() - DIAG_SPEED);
        }
        else if (bn::keypad::right_held() && bn::keypad::up_held()) {
            player.set_x(player.x() + DIAG_SPEED);
            player.set_y(player.y() - DIAG_SPEED);
        }
        else if (bn::keypad::left_held() && bn::keypad::down_held()) {
            player.set_x(player.x() - DIAG_SPEED);
            player.set_y(player.y() + DIAG_SPEED);
        }
        else if (bn::keypad::right_held() && bn::keypad::down_held()) {
            player.set_x(player.x() + DIAG_SPEED);
            player.set_y(player.y() + DIAG_SPEED);
        }

        // Single key presses
        else if (bn::keypad::left_held())
        {
            player.set_x(player.x() - SPEED);
        }
        else if (bn::keypad::right_held())
        {
            player.set_x(player.x() + SPEED);
        }
        else if (bn::keypad::up_held())
        {
            player.set_y(player.y() - SPEED);
        }
        else if (bn::keypad::down_held())
        {
            player.set_y(player.y() + SPEED);
        }
        //Boost activation 
        if(bn::keypad::a_pressed() && boosts_left > 0 && !boost_active)
        {
            boost_active = true;
            boost_timer = BOOST_DURATION_FRAMES;
            player_speed = BOOST_SPEED;
            boosts_left--;
        }
        //When boost is active 
        if(boost_active)
        {
            --boost_timer;

            if(boost_timer <= 0)
            {
                boost_active = false;
                player_speed = SPEED;
            }
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
        }

        if(bn::keypad::start_pressed())
        {
            // Reset positions
            player.set_position(PLAYER_X, PLAYER_Y);
            treasure.set_position(TREASURE_X, TREASURE_Y);

            // Reset score
            score = 0;

            //Reset boost 
            boosts_left = MAX_BOOSTS;
            boost_active = false;
            boost_timer = 0;
            player_speed = SPEED;
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