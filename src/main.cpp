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

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "bn_sprite_items_chicken_front.h"
#include "bn_sprite_items_chicken_right.h"
#include "bn_sprite_items_chicken_left.h"
#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_cornupd.h"
#include "bn_sprite_items_cornbag.h"


//github update test
// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 1;

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

//NEW: sprite starting location

static constexpr int PLAYER_X = 50;
static constexpr int PLAYER_Y = -70;

static constexpr int TREASURE_X = 20;
static constexpr int TREASURE_Y = -10;

static constexpr int BAG_X = -112;
static constexpr int BAG_Y = 72;

static constexpr int BOOST_X = -70;
static constexpr int BOOST_Y = -70;

int main() {
    bn::core::init();

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    //Displays boost number left
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> boost_sprites = {};

    //add text for game explanation
    bn::vector<bn::sprite_ptr, 64> intro_sprites; //new intro sprite
    
    //defining values that update as the game goes on
    //score, speed, celebration & treasure timers
    int score = 0;
    int speed_boosts_left = 3;
    int boost_timer = 0;
    int celebration_countdown = 0;
    int treasure_timer = 0;

    //timer for intro text
    int intro_timer = 300; //60 frames for 5 seconds


    //generate the intro text
        text_generator.generate(-85, 0, "Oh no! Corn fell out!!", intro_sprites);
        text_generator.generate(-75, 20, "Find it and eat it!", intro_sprites);


    int boost_speed = 2;

    //Spawn location for chicken and treasure
    int direction = 0; //0 is for the front facing direction 1 is for left, 2 is for right 
    bn::sprite_ptr player = bn::sprite_items::chicken_front.create_sprite(PLAYER_X, PLAYER_Y); //player spawn location..?
    bn::sprite_ptr treasure = bn::sprite_items::cornupd.create_sprite(TREASURE_X, TREASURE_Y);

    bn::sprite_ptr cornbag = bn::sprite_items::cornbag.create_sprite(BAG_X, BAG_Y);

    while (true) {
        bn::backdrop::set_color(bn::color(12,5,28));
        //if player presses the START input, reset starting position of sprite and treasure
        if(bn::keypad::start_pressed()){
            player = bn::sprite_items::chicken_front.create_sprite(PLAYER_X,PLAYER_Y);
            treasure = bn::sprite_items::cornupd.create_sprite(TREASURE_X, TREASURE_Y);
            score = 0;
            speed_boosts_left = 3;

            intro_timer = 300; //reset it back to 5 seconds

            intro_sprites.clear();

        //generate the intro text
            text_generator.generate(-85, 0, "Oh no! Corn fell out!!", intro_sprites);
            text_generator.generate(-75, 20, "Find it and eat it!", intro_sprites);

            
            
        }

//intro timer
if(intro_timer > 0){
        intro_timer--;
    }

if(intro_timer == 0){
    intro_sprites.clear(); //text goes away
}

//Speed boost count
    bn::string<MAX_SCORE_CHARS> speed_boosts_left_string = bn::to_string<MAX_SCORE_CHARS>(speed_boosts_left);

    //Update the boost display
    boost_sprites.clear();
    text_generator.generate(BOOST_X, BOOST_Y, speed_boosts_left_string, boost_sprites);


        //Moving mechanics for player
        //LEFT input
        if (bn::keypad::left_held())
        {

            //add direction checker
            if (direction != 1){
                player = bn::sprite_items::chicken_left.create_sprite(player.x(), player.y());
                direction = 1;
            }


            player.set_x(player.x() - SPEED);

            if(bn::keypad::a_pressed() && speed_boosts_left > 0) {
                boost_timer = 180;
                speed_boosts_left--;
                }

                if(boost_timer > 0) {
                player.set_x(player.x() - SPEED - boost_speed);
                }  
        }

        //RIGHT input
        if (bn::keypad::right_held())
        {
            //add direction checker
            if (direction != 2){
                player = bn::sprite_items::chicken_right.create_sprite(player.x(), player.y());
                direction = 2;
            }

            player.set_x(player.x() + SPEED);

            if(bn::keypad::a_pressed() && speed_boosts_left > 0) {
                boost_timer = 180;
                speed_boosts_left --;               
            
            }

            if(boost_timer > 0){
                player.set_x(player.x() + SPEED + boost_speed);

            }
        }

        //UP input
        if (bn::keypad::up_held())
        {
            //add direction checker
            if (direction != 0){
                player = bn::sprite_items::chicken_front.create_sprite(player.x(), player.y());
                direction = 0;
            }


            player.set_y(player.y() - SPEED);

            if(bn::keypad::a_pressed() && speed_boosts_left > 0) {
                boost_timer = 180;                
                speed_boosts_left --;               
            
            }

            if(boost_timer > 0){
                player.set_y(player.y() - SPEED - boost_speed);

            }
        }

        //DOWN input
        if (bn::keypad::down_held())
        {
            
            //add direction checker
            if (direction != 0){
                player = bn::sprite_items::chicken_front.create_sprite(player.x(), player.y());
                direction = 0;
            }


            player.set_y(player.y() + SPEED);

            if(bn::keypad::a_pressed() && speed_boosts_left > 0) {
                    boost_timer = 180;
                    speed_boosts_left --;               
            
            }

            if(boost_timer > 0){
                player.set_y(player.y() + SPEED + boost_speed);
            }

        }
        
        //Starts boost timer if it equals 0
        if(boost_timer > 0){
            boost_timer--;
        }


        //If player goes out of bounds, loop back to opposite side
        if(player.x() < MIN_X){
            player.set_x(MAX_X);
        }

        if(player.x() > MAX_X){
            player.set_x(MIN_X);
        }

        if(player.y() < MIN_Y){
            player.set_y(MAX_Y);
        }

        if(player.y() > MAX_Y){
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

        // variables for new locations for treasure 
        int new_x = rng.get_int(MIN_X, MAX_X);
        int new_y = rng.get_int(MIN_Y, MAX_Y);

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect)) {
            // Jump to any random point in the screen
            treasure.set_position(new_x, new_y);

            score++;

            //If chicken intersects with the treasure, change the color of the screen 
            //for celebration effect
            celebration_countdown = 30;
            treasure_timer = 80;
        }

        //screen changes color if chicken intersects with treasure
        if(celebration_countdown > 0) {
                bn::backdrop::set_color(bn::color(14, 20, 9));
                celebration_countdown--;
            }

        //treasure changes position after 100 frames has passed
        if(treasure_timer > 0) {
            treasure_timer--;
        } 
        if(treasure_timer == 0) {
            treasure_timer = 80;
            treasure.set_position(new_x, new_y);
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