### Feed The Worm ###
-------------------
Contributors: Haoyu Wang, Chia-Shen Lin

Hello everyone. Inspired by the phone game "slither.io", we decided to create a game called "Feed the Worm" for the final project. This game is very easy and fun to play.

== Requirements ==

* This game is ran straight from the computer terminal, so a laptop is required to play the game.
* In order for the terminal to successfully compile/run the game code and start up the game, the following SFML packages must be installed and configured:
-sfml-graphics-d.lib
-sfml-window-d.lib
-sfml-system-d.lib
    * For more detailed information on how to do so, please refer to the website:
    https://www.sfml-dev.org/tutorials/2.5/start-vc.php

== Gameplay and Controls ==

The game starts automatically as soon as the gaming window is launched. On the window, you will see a round circular worm head moving on the main map with important game data such as the up time, difficulty level, and eating score located to the left of the map. The objective of this game is to grow the worm by guiding it to the one fruit that spawns randomly on the map as a small magenta-colored block. The fruit's location can be better seen from the mini view on the top right corner that displays the entire map. Whenever the worm touches and eats the fruit, the worm length increases and your score goes up by one. However, the worm dies immdeiately if it touches the map border.
  
  * Press the keyboard keys "w", "a", "s", or "d" to move the worm up, left, down or right.
  * Press either one of the num keys from 1 to 8 to adjust the difficulty level (game speed). The higher the number, the faster the worm travels.
  * Press the space bar to pause the game. Press it again to continue.
  * If the worm dies, press enter to respawn the worm and restart the game.

==Instructions on how to execute and start up the game==

1. Download everything from main in the FeedtheWorm repository as a zip. Recommended way of doing it is to select the "Download ZIP" option by pressing the green "download code" button.
2. Extract the zip folder. After extracting, you should be find a folder named something like "FeedTheWorm-Main" on your computer.
3. Move that folder to your home directory and open your computer (preferably linux) terminal.
4. In your terminal, type in "cd" followed by a space and the folder name before pressing enter.
5. Next, type in the terminal "make Gmain" and press enter.
6. Finally, type "Gmain" to run the game.
