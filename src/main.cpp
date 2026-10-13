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

#include "bn_sprite_items_weirdface.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "bn_sprite_items_sob.h"
#include "common_fixed_8x16_font.h"

//Local variables ---------------------------------------------------------------------
static constexpr int START_X = 5;
static constexpr int START_Y = 10;

static constexpr bn::size PLAYER_SIZE = {16, 16};
static constexpr bn::size TREASURE_SIZE = {8, 8};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

static constexpr int MAX_TEXT_CHARS = 40;

static constexpr int SCORE_X = 100;
static constexpr int SCORE_Y = 70;
static constexpr int Timer_X = 0;
static constexpr int Timer_Y = -70;

int main()
{
    bn::core::init();
    
    //Initial values and statements (run off start of game)--------------------------------
    bn::backdrop::set_color(bn::color(15, 5, 15));
    bn::random rng = bn::random();
    bn::vector<bn::sprite_ptr, MAX_TEXT_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    //Variables----------------------------------------------------------------------------
    int score = 0;
    int boosts = 3;
    int speed = 1;

    int matchTime = 3600;
    bool start = true;

    int count = 0;
    bool counting = false;

    static constexpr int P_START_X = 20;
    static constexpr int P_START_Y = -50;

    //Sprite creation ----------------------------------------------------------------------
    bn::sprite_ptr player = bn::sprite_items::weirdface.create_sprite(P_START_X, P_START_Y);
    bn::sprite_ptr sob = bn::sprite_items::sob.create_sprite(
        (rng.get_int(0, 2) == 0) ? 50 : -50,
        (rng.get_int(0, 2) == 0) ? 50 : -50
    );
    bn::sprite_ptr sob2 = bn::sprite_items::sob.create_sprite(
        (rng.get_int(0, 2) == 0) ? 50 : -50,
        (rng.get_int(0, 2) == 0) ? 50 : -50
    );
    bn::sprite_ptr sob3 = bn::sprite_items::sob.create_sprite(
        (rng.get_int(0, 2) == 0) ? 50 : -50,
        (rng.get_int(0, 2) == 0) ? 50 : -50
    );

    //All game logic (must run inside of the loop in order to update per frame of running the game)
    while (true)
    {
        //Player position variables
        bn::fixed x = player.x();
        bn::fixed y = player.y();

        //Player screen looping logic
        if (x > MAX_X) {
            x = MIN_X;
        }
        else if (x < MIN_X) {
            x = MAX_X;
        }
        if (y > MAX_Y) {
            y = MIN_Y;
        }
        else if (y < MIN_Y) {
            y = MAX_Y;
        }

        player.set_x(x);
        player.set_y(y);

        //Player movement logic
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

        //Match timer, and end of match logic
        if (start == true) {
            matchTime--;
            if(matchTime <= 0) {
                start = false;
                matchTime = 0;
                boosts = 0;
                speed = 0;
                bn::backdrop::set_color(bn::color(15, 15, 15));
            }
        }

        //Replay system
        if (bn::keypad::start_pressed()) {
            score = 0;
            boosts = 3;
            speed = 1;
            start = true;
            matchTime = 3600;
            count = 0;
            counting = false;
            bn::backdrop::set_color(bn::color(15, 5, 15));
            player.set_position(P_START_X, P_START_Y);
            sob.set_position((rng.get_int(0, 2) == 0) ? 50 : -50, (rng.get_int(0, 2) == 0) ? 50 : -50);
            sob2.set_position((rng.get_int(0, 2) == 0) ? 50 : -50, (rng.get_int(0, 2) == 0) ? 50 : -50);
            sob3.set_position((rng.get_int(0, 2) == 0) ? 50 : -50, (rng.get_int(0, 2) == 0) ? 50 : -50);
        }

        //Player speed boost mechanic
        if (start && bn::keypad::a_pressed()) {
            if(boosts >= 1) {
                speed = 3;
                boosts = boosts - 1; 

                count = 0;
                counting = true;
            }
        }
        if(counting) {
            count++;
            if(count >= 300) {
                speed = 1;
                counting = false;
            }
        }

        //Sprite(s) dimensions
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());

        bn::rect sob_rect = bn::rect(sob.x().round_integer(),
                                          sob.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        bn::rect sob2_rect = bn::rect(sob2.x().round_integer(),
                                           sob2.y().round_integer(),
                                           TREASURE_SIZE.width(),
                                           TREASURE_SIZE.height());

        bn::rect sob3_rect = bn::rect(sob3.x().round_integer(),
                                           sob3.y().round_integer(),
                                           TREASURE_SIZE.width(),
                                           TREASURE_SIZE.height());

        //Treasure rng spawn locations
        if (start && player_rect.intersects(sob_rect))
        {
            sob.set_position(rng.get_int(MIN_X, MAX_X), rng.get_int(MIN_Y, MAX_Y));
            score++;
        }

        if (start && player_rect.intersects(sob2_rect))
        {
            sob2.set_position(rng.get_int(MIN_X, MAX_X), rng.get_int(MIN_Y, MAX_Y));
            score++;
        }

        if (start && player_rect.intersects(sob3_rect))
        {
            sob3.set_position(rng.get_int(MIN_X, MAX_X), rng.get_int(MIN_Y, MAX_Y));
            score++;
        }

        //All in-game visual display logic
        int timer = matchTime / 60;
        bn::string<MAX_TEXT_CHARS> score_string = bn::to_string<MAX_TEXT_CHARS>(score);
        bn::string<3> timer_string = bn::to_string<3>(timer);

        score_sprites.clear();

        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        text_generator.generate(Timer_X, Timer_Y,
                                timer_string, 
                                score_sprites);

        if (start == false) {
            text_generator.generate(-85, 0, 
                                    "Press Start to Replay", 
                                    score_sprites);
        }

        if (boosts == 3) {
            text_generator.generate(-110, 70, 
                                    ">>>", 
                                    score_sprites);
        }
        else if (boosts == 2) {
            text_generator.generate(-110, 70, 
                                    ">>", 
                                    score_sprites);
        }
        else if (boosts == 1) {
            text_generator.generate(-110, 70, 
                                    ">", 
                                    score_sprites);
        }

        rng.update();

        bn::core::update();
    }
}