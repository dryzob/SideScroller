#include <Arduboy2.h>
#include "images.h"

#define FPS 75
#define GROUND_LEVEL 55
#define DINO_GROUND_LEVEL GROUND_LEVEL + 7
#define CACTUS_GROUND_LEVEL GROUND_LEVEL + 8
#define NUMBER_OF_OBSTACLES 3
#define LAUNCH_DELAY_MIN 90
#define LAUNCH_DELAY_MAX 200
#define PTERODACTYL_UPPER_LIMIT 27
#define PTERODACTYL_LOWER_LIMIT 48

enum class GroundType : uint8_t {
  Flat,
  Bump,
  Hole
};

Arduboy2 arduboy;
uint8_t groundX = 0;
uint8_t obstacleLaunchCountdown = LAUNCH_DELAY_MAX;
uint16_t score = 0;

#include "dinoFunctions.h"
#include "obstacleFunctions.h"

GroundType ground[5] = { // This is the initial layout for the road before the random comes in
  GroundType::Flat,
  GroundType::Bump,
  GroundType::Flat,
  GroundType::Flat,
  GroundType::Hole,
};

const uint8_t *ground_images[] = {ground_flat, ground_bump, ground_hole};


void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);

}

void loop() {
  if (!(arduboy.nextFrame())) return;
  arduboy.clear();
  arduboy.pollButtons();

  controlDino();
  updateDino();
  updateObstacles();
  collision();

  obstacleLauncher();

  drawObstacles();
  renderGround();
  drawDino();

  if(arduboy.everyXFrames(3)) score++;

  arduboy.display();
}

void renderGround() {
  if(groundX == 32) {
    groundX = 0;

    uint8_t type = random(0, 6);
    GroundType groundType;

    switch(type) {
      case 0 ... 3:
        groundType = GroundType::Flat;
        break;
      case 4:
        groundType = GroundType::Bump;
        break;
      case 5:
        groundType = GroundType::Hole;
        break;
    }
    ground[0] = ground[1];
    ground[1] = ground[2];
    ground[2] = ground[3];
    ground[3] = ground[4];
    ground[4] = groundType;
  }
  
  groundX++;

  // Rending the road comes last
  for (uint8_t i = 0; i < 5; i++) {
    uint8_t imageIndex = static_cast<uint8_t>(ground[i]);
    Sprites::drawSelfMasked((i * 32) - groundX, GROUND_LEVEL, ground_images[imageIndex], 0);
  }
}

bool collision() {
  for(uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    if(obstacles[i].enabled == true) {
      Rect dinoRect = Rect{dino.x,
                          (dino.y - getImageHeight(dino.image)),
                          getImageWidth(dino.image),
                          getImageHeight(dino.image)};

      Rect obstRect = Rect{obstacles[i].x,
                          (obstacles[i].y - getImageHeight(obstacles[i].image)),
                          getImageWidth(obstacles[i].image),
                          getImageHeight(obstacles[i].image)};

      if(arduboy.collide(dinoRect, obstRect)) {
        arduboy.digitalWriteRGB(RED_LED, RGB_ON);
        return true;
      }
    }
  }
  return false;
}
