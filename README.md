# TI-Tanks by Connor McDonald and Krish Singh

You can watch a demo here! 

# https://www.youtube.com/shorts/Xj_KYZfzogk

A top-down 2-player game inspired by the classic game "Tank Trouble", where your goal is to destroy your opponent's tank before they destroy yours. Each player will have a slide pot to turn their tank. They also have 3 buttons each, one for shooting, moving forward, and activating power-ups. The shooting button and power-ups have cooldowns; an in-game sound indicates the power-up availability, and onboard LED lights on the game's PCB.  The bullets are released from the direction the player is facing. The bullets ricochet off the environment (walls) without losing momentum, but eventually disappear. The dash allows for dodging by quickly moving in the direction being faced. Each player has 1-7 (customizable) health points; getting hit by any bullet (even yours) decreases the HP by 1. Once a player reaches 0 hp, they lose, and the other player wins. The game features 3 different power-ups that arrive at certain intervals and last ~10 seconds.

Features:
1 slidepot for each player for rotation
1 button for each player to shoot
1 button for each player to power up
1 button for each player for forward movement
Sounds for shooting, getting hit, receiving powerup, and power-up activation
Different maps for variance (10 available)
Multiple language support
LED Screen featured on custom PCB with all components + LED lights
Start and game end menu
Score display on game end and health display
Two ISRs: one to shoot bullets and Joystick movements (TimerG12), Sound (periodic SysTick interrupt, high priority
Score: health is displayed in hearts on each corner. Stats will be displayed after the round finishes



