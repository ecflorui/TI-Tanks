# TI-Tanks by Connor McDonald and Krish Singh

You can watch a demo here!  https://www.youtube.com/shorts/Xj_KYZfzogk

A top-down 2-player game inspired by the classic game "Tank Trouble", where your goal is to destroy your opponent's tank before they destroy yours. Each player will have a slide pot to turn their tank. They also have 3 buttons each, one for shooting, moving forward, and activating power-ups. The shooting button and power-ups have cooldowns; an in-game sound indicates the power-up availability, and onboard LED lights on the game's PCB.  The bullets are released from the direction the player is facing. The bullets ricochet off the environment (walls) without losing momentum, but eventually disappear. The dash allows for dodging by quickly moving in the direction being faced. Each player has 1-7 (customizable) health points; getting hit by any bullet (even yours) decreases the HP by 1. Once a player reaches 0 hp, they lose, and the other player wins. The game features 3 different power-ups that arrive at certain intervals and last ~10 seconds.

## Features

- Slide-pot for each player to rotate their tank  
- Button for each player to shoot  
- Button for each player for forward movement  
- Button for each player to activate power-ups (cooldown & LED indicator)  
- In-game sounds for shooting, getting hit, power-up availability & activation  
- On-board LEDs to show power-up status  
- Bullets that ricochet off walls without losing momentum  
- Dash ability for quick dodging  
- Customizable health (1–7 HP per player)  
- Three different power-ups, spawning at set intervals (~10 s duration)  
- Ten different maps for variety  
- Multiple language support  
- Start and end menus with score/health displays  
- Two ISRs (TimerG12: shooting & movement, SysTick: sound)

### Prerequisites
- Texas Instruments CCS 20.0.0 IDE 

### Build & Flash
```bash
git clone https://github.com/ecflorui/ti-tanks.git
cd ti-tanks
```
Then build/flash in CCS. 

## 📂 Project Structure

```text
.
├── .settings
├── cjm6842
│   ├── cjm6842-backups
│   └── plots
├── Debug
│   ├── .clangd
│   │   └── .cache
│   │       └── clangd
│   │           └── index
│   ├── images
│   │   ├── pipedreams
│   │   ├── SpaceInvaders
│   │   │   └── pipedreams
│   │   └── TI-Sprites
│   │       └── Alternative Versions (unused)
│   ├── sounds
│   └── ticlang
├── designdocuments
├── images
│   ├── blender
│   ├── breakout
│   ├── Connect4
│   ├── numbers
│   ├── SpaceInvaders
│   │   └── pipedreams
│   ├── spaceship
│   └── TI-Sprites
│       └── Alternative Versions (unused)
├── sounds
├── targetConfigs
├── TI-Tanks
└── ticlang

All relevant PCB files can be found in the cjm6842 folder. 

Thanks to Professor Valavano and the UT ECE department.
