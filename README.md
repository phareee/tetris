# tetris
![tetris](https://user-images.githubusercontent.com/54446123/104612637-018eb400-56b9-11eb-86d8-d44e15d4e35f.png)
Tetris, tile-matching puzzle game, in a version of ascii on the screen which consists of 84x25 characters. Goal of the game is to continually stack the blocks and clear lines as long as you can. To do that, you need to fill an entire line horizontally with blocks, causing those lines to disappear and giving you more space to stack. You will get 100 points from clearing one line and if you can clear more than one line at a time you will get the combo points X2 from your latest cleared line.


Player can control the game through serial terminal via UART: press ‘a’ to move block to the left, press ‘s’ to move block to bottom quicker, press ‘d’ to move block to the right and press space bar to rotate block. The interrupt is also set, the first priority is USER BUTTON. It means that whenever the USER BUTTON is pushed, it will perform its function that is to pause and resume playing. The RESET BUTTON will reset and start new game whenever it’s pushed.

Click here to see the demonstration video
https://drive.google.com/open?id=1fvV52Wdov8W2aoxaEbBZufu9WPfSl3Yf
