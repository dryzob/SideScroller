#pragma once
#include "images.h"


enum class ObstacleType : uint8_t {
  SingleCactus,
  DoubleCactus,
  TripleCactus,
  Pterodactyl1,
  Pterodactyl2,
  Count_AllObstacles = 4
};

struct Obstacle {
  int8_t x;
  uint8_t y;
  ObstacleType type;
  bool enabled;
  const uint8_t *image;
};
//***************************************
// Function names
//***************************************
void launchObstacle(uint8_t obstacleNumber);
void obstacleLauncher();
void updateObstacles();
void drawObstacles();

Obstacle obstacles[NUMBER_OF_OBSTACLES] = {
      { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
      { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
      { 0, 0, ObstacleType::Pterodactyl1, false, pterodactyl_1 },
};

const uint8_t *obstacle_images[] = {cactus_1, cactus_2, cactus_3, pterodactyl_1, pterodactyl_2};

//***************************************
// Function Definitions
//***************************************
void obstacleLauncher() {
  --obstacleLaunchCountdown;
  // arduboy.println(obstacleLaunchCountdown);

  if(obstacleLaunchCountdown == 0) {
    for(uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
      if(!obstacles[i].enabled) {
        launchObstacle(i);
        break;
      }
    }
    obstacleLaunchCountdown = random(LAUNCH_DELAY_MIN, LAUNCH_DELAY_MAX);
  }
}

void launchObstacle(uint8_t obstacleNumber) {
  // Choose obstacle

  ObstacleType randomUpper = ObstacleType::SingleCactus;

  switch(score) {
    case 0 ... 99:
      randomUpper = ObstacleType::SingleCactus;
      break;
    case 100 ... 199:
      randomUpper = ObstacleType::DoubleCactus;
      break;
    case 200 ... 299:
      randomUpper = ObstacleType::TripleCactus;
      break;
    default:
      randomUpper = ObstacleType::Count_AllObstacles;
      break;
  }

  uint8_t randomLowerVal = static_cast<uint8_t>(ObstacleType::SingleCactus);
  uint8_t randomUpperVal = static_cast<uint8_t>(randomUpper);
  uint8_t randomObstacle = random(randomLowerVal, randomUpperVal + 1);

  ObstacleType type = static_cast<ObstacleType>(randomObstacle);

  // Launch the obstacle ..
  
  obstacles[obstacleNumber].type = type;
  obstacles[obstacleNumber].enabled = true;
  obstacles[obstacleNumber].x = WIDTH - 1;

  if(type == ObstacleType::Pterodactyl1) {
    obstacles[obstacleNumber].y = random(PTERODACTYL_UPPER_LIMIT, PTERODACTYL_LOWER_LIMIT);
  } else {
    obstacles[obstacleNumber].y = CACTUS_GROUND_LEVEL;
  }
}

void updateObstacles() {
  for(uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    if(obstacles[i].enabled == true) {
      switch(obstacles[i].type) {
        case ObstacleType::Pterodactyl1:
        case ObstacleType::Pterodactyl2:
          if(arduboy.everyXFrames(5)) {
            if(obstacles[i].type == ObstacleType::Pterodactyl1) {
              obstacles[i].type = ObstacleType::Pterodactyl2;
            } else {
              obstacles[i].type = ObstacleType::Pterodactyl1;
            }
          }
          obstacles[i].x--;
          break;
        
        case ObstacleType::SingleCactus:
        case ObstacleType::DoubleCactus:
        case ObstacleType::TripleCactus:
          obstacles[i].x--;
          break;
      }

      // Leaving view, delete
      if (obstacles[i].x < -getImageWidth(obstacles[i].image)) {
        obstacles[i].enabled = false;
      }
    }
  }
}

void drawObstacles() {
  for(uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    if (obstacles[i].enabled == true) {
      uint8_t imageIndex = static_cast<uint8_t>(obstacles[i].type);

      obstacles[i].image = obstacle_images[imageIndex];
      Sprites::drawOverwrite(obstacles[i].x, obstacles[i].y - getImageHeight(obstacles[i].image), obstacles[i].image, 0);
    }
  }
}