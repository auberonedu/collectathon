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
#include "common_fixed_8x16_font.h"
// body asset
#include "bn_sprite_items_body.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 1;

// Boost speed
static constexpr bn::fixed BOOSTED_SPEED = 2;

// Boost duration
static constexpr int BOOST_DURATION_FRAMES = 300;
static constexpr int MAX_BOOSTS = 3;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// new starting location for treasure and player
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;
static constexpr int TREASURE_START_X = 0;
static constexpr int TREASURE_START_Y = 0;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Maximum number of Snake Segments including the head
static constexpr int MAX_SEGMENTS = 64;
// frames between each position so it looks smooth
static constexpr int POSITION_STEP_FRAMES = 1;

// setting up background grid for better movement mechanics
static constexpr int CELL_SIZE = 8;
// snakes position in the grid space not in pixels
int snake_grid_x = 0;
int snake_grid_y = 0;

// Direction Enum
enum class Direction
{
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
Direction last_dir = Direction::NONE;

// Self Collision
bool self_collision = false;

int main()
{
    bn::core::init();

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    int boost_remaining = MAX_BOOSTS;

    int boost_duration_counter = 0;

    bn::fixed current_speed = SPEED;

    bn::fixed current_angle = 0;

    // bn::fixed dx = 0;
    // bn::fixed dy = 0;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_START_X, PLAYER_START_Y);

    // initialize grid coordinates to match starting pixel position
    snake_grid_x = PLAYER_START_X / CELL_SIZE;
    snake_grid_y = PLAYER_START_Y / CELL_SIZE;

    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(TREASURE_START_X, TREASURE_START_Y);

    // snakes body not including the head
    bn::vector<bn::sprite_ptr, MAX_SEGMENTS> body_segments;
    // using vector to store the exisiting position of the head so the body can follow
    static constexpr int MAX_TAIL_SEGMENTS = MAX_SEGMENTS * 8;
    bn::vector<bn::fixed_point, MAX_TAIL_SEGMENTS> head_positions;
    // spacing the body segments along the tail
    int position_step_counter = 0;

    // Backdrop Color
    bn::backdrop::set_color(bn::color(30, 0, 30));

    while (true)
    {
        // // Move player with d-pad
        // if (bn::keypad::left_held())
        // {
        //     player.set_x(player.x() - current_speed);
        // }
        // if (bn::keypad::right_held())
        // {
        //     player.set_x(player.x() + current_speed);
        // }
        // if (bn::keypad::up_held())
        // {
        //     player.set_y(player.y() - current_speed);
        // }
        // if (bn::keypad::down_held())
        // {
        //     player.set_y(player.y() + current_speed);
        // }

        // Move player with d-pad (but no diagonal movement allowed (only one button can be pressed at a time))

        if (bn::keypad::left_pressed() && last_dir != Direction::RIGHT)
        {
            last_dir = Direction::LEFT;
            current_angle = bn::fixed(90);
        }
        if (bn::keypad::right_pressed() && last_dir != Direction::LEFT)
        {
            last_dir = Direction::RIGHT;
            current_angle = bn::fixed(270);
        }
        if (bn::keypad::down_pressed() && last_dir != Direction::UP)
        {
            last_dir = Direction::DOWN;
            current_angle = bn::fixed(180);
        }
        if (bn::keypad::up_pressed() && last_dir != Direction::DOWN)
        {
            last_dir = Direction::UP;
            current_angle = bn::fixed(0);
        }

        // player.set_x(player.x() + dx);
        // player.set_y(player.y() + dy);
        // decide when grid movement happens: one per frame
        bool step = true;
        if (step)
        {
            switch (last_dir)
            {
            case Direction::LEFT:
                snake_grid_x -= 1;
                break;
            case Direction::RIGHT:
                snake_grid_x += 1;
                break;
            case Direction::UP:
                snake_grid_y -= 1;
                break;
            case Direction::DOWN:
                snake_grid_y += 1;
                break;
            default:
                break;
            }
        }

        // set sprite positions to the grid coordinates
        bn::fixed pixel_x = snake_grid_x * CELL_SIZE;
        bn::fixed pixel_y = snake_grid_y * CELL_SIZE;
        player.set_x(pixel_x);
        player.set_y(pixel_y);

        // Speed Boost
        // if (bn::keypad::a_pressed() && (boost_remaining > 0) && (boost_duration_counter == 0))
        // {
        //     boost_remaining--;
        //     boost_duration_counter = BOOST_DURATION_FRAMES;
        // }

        current_speed = SPEED;
        if (boost_duration_counter > 0)
        {
            current_speed = BOOSTED_SPEED;
            boost_duration_counter--;
        }
        // Reset game if Start is pressed or self collision is true
        if (bn::keypad::start_pressed() || self_collision)
        {
            // on reset re-initialize from our start position
            snake_grid_x = PLAYER_START_X / CELL_SIZE;
            snake_grid_y = PLAYER_START_Y / CELL_SIZE;
            player.set_position(snake_grid_x * CELL_SIZE, snake_grid_y * CELL_SIZE);
            score = 0;
            last_dir = Direction::NONE;
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            // boost_remaining = MAX_BOOSTS;
            body_segments.clear();
            head_positions.clear();
            position_step_counter = 0;
            self_collision = false;
        }

        // // Wrap player around screen edges
        // if (player.x() < MIN_X)
        // {
        //     player.set_x(MAX_X);
        // }
        // else if (player.x() > MAX_X)
        // {
        //     player.set_x(MIN_X);
        // }

        // if (player.y() < MIN_Y)
        // {
        //     player.set_y(MAX_Y);
        // }
        // else if (player.y() > MAX_Y)
        // {
        //     player.set_y(MIN_Y);
        // }

        int grid_min_x = MIN_X / CELL_SIZE;
        int grid_max_x = (MAX_X - 1) / CELL_SIZE;
        int grid_min_y = MIN_Y / CELL_SIZE;
        int grid_max_y = (MAX_Y - 1) / CELL_SIZE;

        if (snake_grid_x < grid_min_x)
            snake_grid_x = grid_max_x;
        else if (snake_grid_x > grid_max_x)
            snake_grid_x = grid_min_x;
        if (snake_grid_y < grid_min_y)
            snake_grid_y = grid_max_y;
        else if (snake_grid_y > grid_max_y)
            snake_grid_y = grid_min_y;
        player.set_position(snake_grid_x * CELL_SIZE, snake_grid_y * CELL_SIZE);

        // https://gvaliente.github.io/butano/classbn_1_1fixed__point__t.html
        // Explained Butano docs found on github pages used for positioning
        // if statement to keep head at fixed position and add body segments to length of head
        position_step_counter++;
        if (position_step_counter >= POSITION_STEP_FRAMES)
        {
            position_step_counter = 0;

            bn::fixed_point current_head(player.x(), player.y());

            // Only shift if head moved
            if (head_positions.empty() || head_positions[0] != current_head)
            {
                if (head_positions.size() < MAX_TAIL_SEGMENTS)
                {
                    // push the head vector down by one element
                    head_positions.push_back(bn::fixed_point());
                }
                // push the rest of the body down by one index point
                for (int i = head_positions.size() - 1; i > 0; --i)
                {
                    head_positions[i] = head_positions[i - 1];
                }
                // keep our head stored at index 0
                head_positions[0] = bn::fixed_point(player.x(), player.y());
            }
        }

        // Update body segments to follow the head
        for (int i = 0; i < body_segments.size(); ++i)
        {
            int tail_index = (i + 1) * 9;
            if (tail_index < head_positions.size())
            {
                body_segments[i].set_position(head_positions[tail_index]);
            }
        }

        // applying the current_angles from the movement section to player rotation
        player.set_rotation_angle_safe(current_angle);
        for (bn::sprite_ptr &seg : body_segments)
        {
            seg.set_rotation_angle_safe(current_angle);
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels and body segments
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        // Check for collision between player (head) and body segments
        int start_index = 3;
        for (int i = start_index; i < body_segments.size(); ++i)
        {
            bn::rect body_rect = bn::rect(body_segments[i].x().round_integer(),
                                          body_segments[i].y().round_integer(),
                                          PLAYER_SIZE.width(),
                                          PLAYER_SIZE.height());
            if (player_rect.intersects(body_rect))
            {
                self_collision = true;
                break;
            }
        }
        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;

            // Add a new body segment if we have space ( - 1 to account for the head)
            if (body_segments.size() < MAX_SEGMENTS - 1 && !head_positions.empty())
            {
                bn::fixed_point tail_pos = head_positions.back();
                body_segments.push_back(
                    bn::sprite_items::body.create_sprite(tail_pos.x(), tail_pos.y()));
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

        bn::core::update();
    }
}