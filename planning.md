A place to write your findings and plans

## Understanding
I understand the portions of like player size the buttons what they're trying to do, the score and character sprite but not much more beyond what the games general idea is.

As far as I understand, the code is basically creating a game for people to compete with each other. As the player, you pick up gold dots and gain points up to a max of 11 characters (So the number can go up to 99999999999 essentially?). From what I understand there is no time limit, so the game currently is to just spend as much time as you want and get the highest score.
## Planning required changes
We were confused on this step, because in the instructions.md it says to plan inside of the isntructions.md, so we took it as some sort of typo. We ended up planning everything in Discord voice chat together instead. Here are the steps/ideas we did have while in voice chat together though.

For required change #2 we first referred back to one of the previous lessons and used the backdrop code to change the backdrop here. But it didn't initially work because we didn't have the backdrop #include. So after adding the #include, the code worked.

For required change #5 I had prior experience in one of the earlier assignments where I added my own twist to make the sprite not be able to leave the boundaries of the screen. So I brought up the code to Raul and we discussed how to alter the code slightly to make the looping work. Since my code was based on setting the sprites x/y values to exactly where the border of the screen would be, we just reversed the order to loop the sprite instead. So we made it so that when the sprite goes beyond the boundary in the positive value of either axis, then it would change the sprites x/y values to be in the negative value (the other side), and vice versa. But the code still didn't work and we struggled with some testing for about 10 minutes. Until I looked back at my code and realized that we forgot to set the sprites new x/y values (this is important because the values were basically just sitting there not being used, but after setting the sprites value, we actually USED the new x/y values).

Now, we are on #6 which is the last change. And so far, we set up the if statement for when someone presses a. And we changed around some of the value like speed to not be fixed and instead adjustable. From there, I made a boosts variable to keep track of how many boosts the payer currently has. Then we decided the next step was to combine all of these to make the boosts counter decrease per button press, and apply the speed boost per button press. But now what we're stuck on is a way to create a timer to time how long each burst of speed should last for. We're not quite sure how to do this, but we're currently thinking that we may need to have another #include that allows the usage of a timer/clock instead of making an artificial timer.

-On #1 we changed the player speed to 5 for player speed which ended up working right away.

- #3 I wanted to just add P_START X and Y along with the dot but it didn't work once we had pushed due to it not reading it for a slight error I had over looked.
-Didn't work we ended up having to change sprites default starting position with P_START and DOT_START after this it ran perfectly and we didn't have any issues and it ran once we restarted it and I think after this we had no more issues or anything else to change.

-#4 Made a reset button by just choosing the default spawn positions for X and Y on treasure and player along with this we just set the score to zero manually after this we didn't have much issues since it seemed pretty straight foward.

#6 We are gonna attempt to make a loop with 3 different 60 frame loops for a total 180 frames/ 3 seconds. Along with this if we can hopefully we can make it just one big 180 frame loop.

(I apologize for all of the text, but since we weren't aware of the planning step and instead did it all in vc, this was the best way to make up for the missed commits)
## Brainstorming game ideas

Timer in the middle of the screen to start, speed boost displayed in the bottom right corner, make multiple treasures spawn in, Match timer

The match timer will determine the end of the match. But there's no set default for the "end" of the game since the game was originally infinite, so we need to make our own replay menu/end of the game menu.

## Plan for implementing game

