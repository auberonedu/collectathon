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
#include <bn_color.h>
#include <bn_backdrop.h>
#include <bn_random.h>
#include <bn_sound_items.h>
#include <bn_sound.h>
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_skull.h"
#include "bn_music_items.h"

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_knight.h"
#include "common_fixed_8x16_font.h"
#include "bn_music.h"

#include "bn_sprite_items_pistol.h"
#include "bn_sprite_items_bullet.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 1;
static constexpr bn::fixed BOOSTEDSPEED = 6;

// Width and height of entities in game
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {16,16};
static constexpr bn::size BULLET_SIZE = {8,8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Starting pos of player and treasure
static constexpr int player_start_x = -50;
static constexpr int player_start_y = 50;
static constexpr int pistol_start_x = -45;
static constexpr int pistol_start_y = 50;
static constexpr int treasure_start_x = 0;
static constexpr int treasure_start_y = 0;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;
static bn::vector<bn::sound_item, 10> soundPool;


int main()
{
    bn::fixed soundAudio = 0.4;

    bn::core::init();

    bn::random rng = bn::random();

    bn::backdrop::set_color(bn::color(20, 31, 31));

    bn::sound_items::roundstart.play(soundAudio - 0.2);

    soundPool.push_back(bn::sound_items::zombiescream1);
    soundPool.push_back(bn::sound_items::zombiescream2);
    soundPool.push_back(bn::sound_items::zombiescream3);
    soundPool.push_back(bn::sound_items::zombiescream4);
    soundPool.push_back(bn::sound_items::zombiescream5);
    soundPool.push_back(bn::sound_items::zombiescream6);
    soundPool.push_back(bn::sound_items::zombiescream7);

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::vector<bn::sprite_ptr, 16> boost_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    // Variables for speed boost mode
    int score = 0;
    int speedBoostTurns = 3;
    int playersCurrentSpeed = (int)SPEED;
    float timer = 0;
    bool speedBoostOn = false;

    bn::sprite_ptr player = bn::sprite_items::knight.create_sprite(player_start_x, player_start_y);
    bn::sprite_ptr treasure = bn::sprite_items::skull.create_sprite(treasure_start_x, treasure_start_y);

    bn::sprite_ptr pistol = bn::sprite_items::pistol.create_sprite(player_start_x + 5, player_start_y);
    pistol.set_scale(0.5);
    
    bn::string<16> boosting = "";

    // Variables for bullets
    bn::vector<bn::sprite_ptr, 40> bulletPool = {};
    bn::fixed bulletSpeed = 3;
    /*
        This vector represents the bullets direction. 
        This goes with the bulletPool vector.
        If we store true, this means the bullet is going to the left side of the screen.
        If we store false, this means the bullet is going to the right side of the screen.
    */
    bn::vector<bool, 40> bulletDirection;

    // Vector for enemy related
    bn::vector<bn::sprite_ptr, 16> enemyPool = {};
    bn::vector<bn::fixed, 16> enemySpeed = {};
    bn::vector<int, 16> enemyHealth = {};

    // Rotation of player
    int rotation = 0;
    int rotationSpeed = 0;

    float timeDelay = 200;
    float timeMinus = 0;

    bn::sprite_text_generator gameEndedTextGenerator(common::fixed_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> textEndedV;


    bool isPlayerDead = false;

    bool isEndGameSongPlaying = false;

    int counterToSecretSong = 0;

    
    bn::sprite_text_generator restartGameTextGen(common::fixed_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> restartGameVec;

    bn::sprite_text_generator gunAmmoCount(common::fixed_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> gunAmmoCountV;

    bn::sprite_text_generator gunName(common::fixed_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> gunNameV;
    
    int pistolAmmo = 8;
    int pistolClips = 4;
    float reloadingTime = 60;

    while (true)
    {   

        bn::string<MAX_SCORE_CHARS> gunAmmoCountString = bn::to_string<MAX_SCORE_CHARS>(pistolAmmo) + bn::to_string<MAX_SCORE_CHARS>("/") + bn::to_string<MAX_SCORE_CHARS>(pistolClips);
        gunAmmoCountV.clear();
        gunAmmoCount.generate(88, 73, gunAmmoCountString, gunAmmoCountV);

        bn::string<MAX_SCORE_CHARS> gunNameString = bn::to_string<MAX_SCORE_CHARS>("M1911");
        gunNameV.clear();
        gunName.generate(45, 73, gunNameString, gunNameV);

        if (bn::keypad::a_pressed()) {
            
            // If we have ammo and we are not reloading
            if (pistolAmmo > 0 && reloadingTime == 60) {

                // Play the sound
                bn::sound_items::pistolshooting.play(soundAudio);

                // Make a bullet ptr and scale it down a little bit
                bn::sprite_ptr bullet = bn::sprite_items::bullet.create_sprite(pistol.x() + 1, pistol.y());
                bullet.set_scale(0.65);
                
                // If both vectors are not full (safety check)
                if (!bulletDirection.full() && !bulletPool.full()) {

                    // If the pistol is not flipped (looking right)
                    if (pistol.horizontal_flip()) {
                        bulletDirection.push_back(true);
                    }
                    
                    // If pistol is flipped (looking left)
                    if (!pistol.horizontal_flip()) {
                        bulletDirection.push_back(false);
                    }  

                    // Put that bullet in the list and decrement the ammo by one
                    bulletPool.push_back(bullet);
                
                }
  
                pistolAmmo -= 1;
                   
            }         
        }

        // Loop through the list of bullets
        for (int i = bulletPool.size() - 1; i >= 0; i--) {

            // If the bullet direction is false (meaning that we are shooting to the left side of the screen)
            if (bulletDirection[i]) {

                // Flip the bullet and make it go towards the left side of the screen
                bulletPool[i].set_horizontal_flip(true);
                bulletPool[i].set_x(bulletPool[i].x() - bulletSpeed);
            }

            // If the bullet direction is true (meaning that we are shooting to the right side of the screen)
            if (!bulletDirection[i]) {

                // Don't flip the bulletm
                bulletPool[i].set_horizontal_flip(false);

                // Get the current bullet and make it go towards the right side of the screen.
                bulletPool[i].set_x(bulletPool[i].x() + bulletSpeed);
            }

          

            // If the bullet is out of bounds, remove it from the list
            if (bulletPool[i].x() > MAX_X || bulletPool[i].x() < MIN_X) {
                bulletPool.erase(bulletPool.begin() + i);
                bulletDirection.erase(bulletDirection.begin() + i);
            }
            
        }

        // If the ammo on the pistol is empty
        if (pistolAmmo == 0 && pistolClips != 0) {

            // Play the reloading speed once
            if (reloadingTime == 60) {
                bn::sound_items::pistolreloading.play(soundAudio);
            }

            // Start the reloading
            reloadingTime -= 0.6;

            // If reloading is done, set the timer back to 60 and pistol ammo to 8
            if (reloadingTime <= 0) {
                reloadingTime = 60;
                pistolAmmo = 8;
                pistolClips -= 1;
            }
        }


        timeDelay -= timeMinus;

        // If the user pressed the b button 10 times, play the secret song
        if (bn::keypad::b_pressed()) {
            counterToSecretSong++;

            if (counterToSecretSong == 10) {
                bn::music_items::easteregg.play();
            }
        }

        // If the player is dead
        if (isPlayerDead) {

            // Display game over screen
            bn::string<MAX_SCORE_CHARS> gameEndedText = bn::to_string<MAX_SCORE_CHARS>("GAME OVER");
            textEndedV.clear();
            gameEndedTextGenerator.generate(-35, -20, gameEndedText, textEndedV);

            // Display restart screen
            bn::string<MAX_SCORE_CHARS> restartText = bn::to_string<MAX_SCORE_CHARS>("A - RESTART");
            restartGameVec.clear();
            restartGameTextGen.generate(-40, 30, restartText, restartGameVec);

            // Stop all music
            bn::music::stop();
            
            // If the endgame music is not playing, play it
            if (!isEndGameSongPlaying) {
                bn::sound_items::roundended.play();
                isEndGameSongPlaying = true;
            }

            // If the a button is pressed, stop all music and restart the game
            if (bn::keypad::a_pressed()) {

                // Stop both music and sounds
                bn::music::stop();
                bn::sound::stop_all();

                isPlayerDead = false;
                isEndGameSongPlaying = false;

                counterToSecretSong = 0;



                bn::sound_items::roundstart.play();
                

                // Set all entites back to position
                player.set_x(player_start_x);
                player.set_y(player_start_y);

                pistol.set_x(pistol_start_x);
                pistol.set_y(pistol_start_y);

                treasure.set_x(treasure_start_x);
                treasure.set_y(treasure_start_y);

                // Reset each list
                enemyPool.clear();
                enemySpeed.clear();
                bulletPool.clear();
                bulletDirection.clear();
                textEndedV.clear();
                restartGameVec.clear();

                // Reset the pistol stats
                pistolAmmo = 8;
                pistolClips = 4;

                score = 0;

                timeDelay = 200;
                timeMinus = 0;

                // Change users speed back to normal, put speed boost turns back to 3, reset timer and turn off speed boost mode
                playersCurrentSpeed = (int)SPEED;
                speedBoostTurns = 3;
                timer = 0;
                speedBoostOn = false;

                // Change background to how it was at the start of game
                bn::backdrop::set_color(bn::color(20, 31, 31));
            }

            
        } else { // Else, game is running
            if (timeDelay <= 0) {
            soundPool.at(rng.get_int() % soundPool.size()).play(soundAudio);
            timeDelay = 200;
        }

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - playersCurrentSpeed);
            pistol.set_x(pistol.x() - playersCurrentSpeed);

            player.set_horizontal_flip(true);
            pistol.set_horizontal_flip(true);

            pistol.set_x(player.x() - 6);
        }

        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + playersCurrentSpeed);
            pistol.set_x(pistol.x() + playersCurrentSpeed);

            player.set_horizontal_flip(false);
            pistol.set_horizontal_flip(false);

            pistol.set_x(player.x() + 5);

        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - playersCurrentSpeed);
            pistol.set_y(pistol.y() - playersCurrentSpeed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + playersCurrentSpeed);
            pistol.set_y(pistol.y() + playersCurrentSpeed);
        }

        // added a new improvement so when you enable speed
        // boost the sprite will spin even faster
        if (speedBoostOn)
        {
            rotationSpeed = 20;
        }
        else
        {
            rotationSpeed = 8;
        }

        // // added a rotation for the player, whenever movement is pressed, the player spins.
        // if (bn::keypad::left_held() || bn::keypad::right_held() ||
        //     bn::keypad::up_held() || bn::keypad::down_held())
        // {
        //     rotation = rotation + rotationSpeed;

        //     if (rotation >= 360)
        //     {
        //         rotation = 0;
        //     }

        //     player.set_rotation_angle(rotation);
        // }

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

        if (bn::keypad::start_pressed())
        {
            // Reset all entities positions
            player.set_x(player_start_x);
            player.set_y(player_start_y);

            treasure.set_x(treasure_start_x);
            treasure.set_y(treasure_start_y);

            score = 0;

            // Change users speed back to normal, put speed boost turns back to 3, reset timer and turn off speed boost mode
            playersCurrentSpeed = (int)SPEED;
            speedBoostTurns = 3;
            timer = 0;
            speedBoostOn = false;

            bn::backdrop::set_color(bn::color(20, 31, 31));
        }

        boost_sprites.clear();

        text_generator.generate(-30, SCORE_Y,
                                boosting,
                                boost_sprites);

        // If the speed boost is on, we decrement the timer variable by 0.5 and keeping the players current speed at the boosted speed
        if (speedBoostOn)
        {
            timer -= .5;
            playersCurrentSpeed = (int)BOOSTEDSPEED;

            // add the boosting text so it displays on top of the screen when toggled
            boosting = "boost in use";
        }

        // If the timer is up, we turn off speed boost and revert the players speed back to it's normal speed
        if (timer <= 0)
        {
            speedBoostOn = false;
            playersCurrentSpeed = (int)SPEED;
            boosting = "";
        }

        // If the player pressed the a button, the speedBoostTurns is not 0 and speed boost mode is not on,
        // Turn on the speed boost mode, start the timer and decrement the about of speed boost turns
        // if (bn::keypad::a_pressed() && speedBoostTurns > 0 && !speedBoostOn)
        // {
        //     speedBoostOn = true;
        //     timer = 60;
        //     speedBoostTurns--;
        // }

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

            // the background color that is randomly being set whenever a point is being taken
            bn::backdrop::set_color(bn::color(rng.get_int(15, 31), rng.get_int(15, 31), rng.get_int(15, 31)));

            // If the list is less than 10, add a new enemy and make their speed to 1
            
            // We added our first zombie
            if (enemyPool.size() < 1) {
                soundPool.at(rng.get_int() % soundPool.size()).play(soundAudio);    
            }
            
            // If the vector is not full (safety check), add an ememy
            if (!enemyPool.full())
            {
                    
                timeMinus += 0.2;

                enemyPool.push_back(bn::sprite_items:: zombie.create_sprite((rng.get_int() % 20) + bn::display::width(), (rng.get_int() % 20) + bn::display::height()));
                enemySpeed.push_back(.5);
                enemyHealth.push_back(100);
            }

            score++;
        }

        // If there is at-least one enemy and one speed for an enemy
        if (enemyPool.size() != 0 && enemySpeed.size() != 0)
        {
            // Loop through the vector
            for (int i = 0; i < enemyPool.size(); i++)
            {

                int enemyIndivSpeed = 0;

                // Make a box around enemy
                bn::rect enemy_rect = bn::rect(enemyPool[i].x().round_integer(),
                                               enemyPool[i].y().round_integer(),
                                               ENEMY_SIZE.width(),
                                               ENEMY_SIZE.height());

                // If a enemy touches player, reset the game
                if (enemy_rect.intersects(player_rect))
                {

                    isPlayerDead = true;

                    bn::sound::stop_all();

                    enemyIndivSpeed = 0;
                   
                }
                else
                {
                    // How fast this enemy moves
                    bn::fixed speed = enemySpeed.at(0);

                    // start with the enemy's current spot
                    bn::fixed next_x = enemyPool[i].x();
                    bn::fixed next_y = enemyPool[i].y();

                    // Move the next spot left or right toward the player
                    if (player.x() > next_x)
                    {
                        next_x = next_x + speed;
                    }
                    else if (player.x() < next_x)
                    {
                        next_x = next_x - speed;
                    }

                    // Move the next spot up or down toward the player
                    if (player.y() > next_y)
                    {
                        next_y = next_y + speed;
                    }
                    else if (player.y() < next_y)
                    {
                        next_y = next_y - speed;
                    }

                    // Draw an invisible box around the next spot
                    bn::rect next_rect = bn::rect(next_x.round_integer(),
                                                  next_y.round_integer(),
                                                  ENEMY_SIZE.width(),
                                                  ENEMY_SIZE.height());

                    // This becomes true if another enemy is already there
                    bool touching_another_enemy = false;

                    // Look at every enemy in the list
                    for (int j = 0; j < enemyPool.size(); j++)
                    {

                        // Skip the enemy we are moving, so it doesn't check itself
                        if (j != i)
                        {

                            // Draw an invisible box around the other enemy
                            bn::rect other_rect = bn::rect(enemyPool[j].x().round_integer(),
                                                           enemyPool[j].y().round_integer(),
                                                           ENEMY_SIZE.width(),
                                                           ENEMY_SIZE.height());

                            // If the boxes overlap, another enemy is in the way
                            if (next_rect.intersects(other_rect))
                            {
                                touching_another_enemy = true;
                            }
                        }
                    }

                    // Only move if no other enemy is in the way
                    if (touching_another_enemy == false)
                    {
                        enemyPool[i].set_position(next_x, next_y);
                    }
                }
            }
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        // Loop through the enemyPool vector
            for (int i = enemyPool.size() - 1; i >= 0; i--) {
               
                // Get a box around them
                bn::rect enemy = bn::rect(enemyPool[i].x().round_integer(),
                                                enemyPool[i].y().round_integer(),
                                                ENEMY_SIZE.width(),
                                                ENEMY_SIZE.height());
                
                // Loop through the bullet vector
                for (int j = bulletPool.size() - 1; j >= 0; j--) {

                    // Get a box around the bullet
                    bn::rect bullet = bn::rect(bulletPool[j].x().round_integer(),
                                                bulletPool[j].y().round_integer(),
                                                BULLET_SIZE.width(),
                                                BULLET_SIZE.height());

                    // If the bullet touches a enemy, remove health and remove the bullet + bullet direction
                    if (bullet.intersects(enemy)) {
                        enemyHealth[i] -= 40;
                        bulletPool.erase(bulletPool.begin() + j);
                        bulletDirection.erase(bulletDirection.begin() + j);
                    }

                   
                }
                
                // If the health of the enemy is below 0, remove the enemy + related enemy stats
                 if (enemyHealth[i] <= 0) {
                        enemyPool.erase(enemyPool.begin() + i);
                        enemySpeed.erase(enemySpeed.begin() + i);
                        enemyHealth.erase(enemyHealth.begin() + i);
                }
            }
        }

        bn::core::update();
    }
}