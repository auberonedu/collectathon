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
#include <bn_color.h>
#include "bn_sprite_items_treasure.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

#include "bn_audio.h"       // KFG
#include "bn_sound_items.h" //KFG
#include "bn_sprite_items_face.h" // KFG Wave 6.5

#include "bn_sprite_items_minusone.h" // Wave 7

// Pixels / Frame player moves at
// Change 1 - Changed the speed from 1 to 2
// KFG - Slightly changed the speed of the sprite
static constexpr bn::fixed SPEED = 1.8;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Wave 7 - Width and height of the minusone
static constexpr bn::size MINUSONE_SIZE = {16, 16};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Wave 7 - Keeps minusone fully on screen
static constexpr int MINUSONE_MARGIN = 8;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
// Slight change - KFG
static constexpr int SCORE_X = 80;
static constexpr int SCORE_Y = -60;

// Wave 6 - Boost indicator location and information
static constexpr int BOOST_X = 80;
static constexpr int BOOST_Y = -50;
static constexpr int BOOST_WIDTH = 8;
static constexpr int START_BOOST = 3;

// Change 3 - Changing the starting position of the player and the dot
static constexpr int PLAYER_START_X = -25;
static constexpr int PLAYER_START_Y = 25;
static constexpr int TREASURE_START_X = 0;
static constexpr int TREASURE_START_Y = 0;

// Location and information of life display
static constexpr int LIFE_X = 80;
static constexpr int LIFE_Y = -40;
static constexpr int START_LIVES = 5;

int main()
{
    bn::core::init();

    // Change 2 - Changed the backdrop color to a light blue color
    // KFG - changed the last parameter
    bn::bg_palettes::set_transparent_color(bn::color(15, 26, 25));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> boost_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    // timer used for speed boost functionality (measured in frames)
    int boostTimer = 0;
    // charges of boost ability
    int boostCount = START_BOOST;

    int score = 0;

    bn::sprite_ptr player = bn::sprite_items::face.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::treasure.create_sprite(TREASURE_START_X, TREASURE_START_Y);

    // Wave 7 - minusone starts at a random spot
    bn::sprite_ptr minusone = bn::sprite_items::minusone.create_sprite(
        rng.get_int(MIN_X + MINUSONE_MARGIN, MAX_X - MINUSONE_MARGIN),
        rng.get_int(MIN_Y + MINUSONE_MARGIN, MAX_Y - MINUSONE_MARGIN));

    while (true)
    {

        // Change 4 - When the player hits start, the game restarts
        if (bn::keypad::start_pressed())
        {
            score = 0;
            player.set_position(PLAYER_START_X, PLAYER_START_Y);
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            boostTimer = 0;
            boostCount = START_BOOST;

            // Wave 7 - send minusone to a new random spot on restart
            minusone.set_position(rng.get_int(MIN_X + MINUSONE_MARGIN, MAX_X - MINUSONE_MARGIN),
                                  rng.get_int(MIN_Y + MINUSONE_MARGIN, MAX_Y - MINUSONE_MARGIN));
        }

        // Wave 6 - Adds a barrier to the edge of the screen
        if (player.x() < MIN_X + 4)
        {
            player.set_x(MIN_X + 4);
        }
        if (player.x() > MAX_X - 4)
        {
            player.set_x(MAX_X - 4);
        }
        if (player.y() < MIN_Y + 4)
        {
            player.set_y(MIN_Y + 4);
        }
        if (player.y() > MAX_Y - 4)
        {
            player.set_y(MAX_Y - 4);
        }

        // Move player with d-pad
        if (boostTimer > 0)
        {
            // if there is time remaining on a boost, double movement speed
            if (bn::keypad::left_held())
            {
                player.set_x(player.x() - (SPEED * 3));
            }
            if (bn::keypad::right_held())
            {
                player.set_x(player.x() + (SPEED * 3));
            }
            if (bn::keypad::up_held())
            {
                player.set_y(player.y() - (SPEED * 3));
            }
            if (bn::keypad::down_held())
            {
                player.set_y(player.y() + (SPEED * 3));
            }
            boostTimer--; // subtract time from boost each frame
        }
        else
        { // if no boost time remaining: move at normal speed
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
        }
        // when player presses A and boost is not active, add ~5 seconds to boost timer
        if (bn::keypad::a_pressed() && boostCount > 0 && boostTimer == 0)
        {
            boostTimer = 300;
            boostCount--;
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

            // Wave 7 - minusone also jumps to a new random spot when the treasure is collected
            minusone.set_position(rng.get_int(MIN_X + MINUSONE_MARGIN, MAX_X - MINUSONE_MARGIN),
                                  rng.get_int(MIN_Y + MINUSONE_MARGIN, MAX_Y - MINUSONE_MARGIN));

            score++;

            // Wave 6 - Gives player a bonus boost every 5 points
            // Capped extra boosts at starting value to encourage using them and prevent text overflow - Matthew
            if (score % 5 == 0 && boostCount < START_BOOST)
            {
                boostCount++;
            }
        }

        // Sound effect every time touch pad is pressed
        // This might be a bit much - maybe play when the player picks up the treasure? - Matthew
        if (bn::keypad::left_pressed() || bn::keypad::right_pressed() || bn::keypad::up_pressed() || bn::keypad::down_pressed())
        {
            bn::sound_items::alert.play();
        }

        // Wave 7 - Bounding box of minusone (checked after the treasure, so it uses minusone's newest position)
        bn::rect minusone_rect = bn::rect(minusone.x().round_integer(),
                                          minusone.y().round_integer(),
                                          MINUSONE_SIZE.width(),
                                          MINUSONE_SIZE.height());

        // Wave 7 - Touching minusone subtracts a point and sends it to a new random spot
        if (player_rect.intersects(minusone_rect))
        {
            minusone.set_position(rng.get_int(MIN_X + MINUSONE_MARGIN, MAX_X - MINUSONE_MARGIN),
                                  rng.get_int(MIN_Y + MINUSONE_MARGIN, MAX_Y - MINUSONE_MARGIN));

            score--;
        }

        // Update boost display
        bn::string<MAX_SCORE_CHARS> boost_string = "B: " + bn::to_string<MAX_SCORE_CHARS>(boostCount);

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update boost display
        boost_sprites.clear();
        text_generator.generate(BOOST_X, BOOST_Y,
                                boost_string,
                                boost_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}