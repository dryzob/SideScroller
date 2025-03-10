#include <Arduboy2.h>
#include "images.h"

#define FPS 75
#define GROUND_LEVEL 55
#define DINO_GROUND_LEVEL GROUND_LEVEL + 7

enum class GroundType : uint8_t {
  Flat,
  Bump,
  Hole
};

enum class Stance : uint8_t {
  Standing,
  Running1,
  Running2,
  Ducking1,
  Ducking2,
  Dead1,
  Dead2,
};

struct Dino {
  uint8_t x;
  uint8_t y;
  Stance stance;
  bool jumping;
  uint8_t jumpIndex;
  const uint8_t *image;
  const uint8_t *mask;
};

Arduboy2 arduboy;
uint8_t groundX = 0;

GroundType ground[5] = { // This is the initial layout for the road before the random comes in
  GroundType::Flat,
  GroundType::Bump,
  GroundType::Flat,
  GroundType::Flat,
  GroundType::Hole,
};

Dino dino = {0, DINO_GROUND_LEVEL, Stance::Standing, false, 0, dinosaur_still, dinosaur_still_mask};

uint8_t jumpCoords[] = {DINO_GROUND_LEVEL, DINO_GROUND_LEVEL-2, DINO_GROUND_LEVEL-5, DINO_GROUND_LEVEL-7, 
                        DINO_GROUND_LEVEL-9, DINO_GROUND_LEVEL-11, DINO_GROUND_LEVEL-13, DINO_GROUND_LEVEL-14,
                        DINO_GROUND_LEVEL-16, DINO_GROUND_LEVEL-17, DINO_GROUND_LEVEL-19, DINO_GROUND_LEVEL-20,
                        DINO_GROUND_LEVEL-21, DINO_GROUND_LEVEL-22, DINO_GROUND_LEVEL-23, DINO_GROUND_LEVEL-23,
                        DINO_GROUND_LEVEL-24, DINO_GROUND_LEVEL-24, DINO_GROUND_LEVEL-25, DINO_GROUND_LEVEL-25,
                        DINO_GROUND_LEVEL-25, DINO_GROUND_LEVEL-25, DINO_GROUND_LEVEL-25, DINO_GROUND_LEVEL-24,
                        DINO_GROUND_LEVEL-24, DINO_GROUND_LEVEL-23, DINO_GROUND_LEVEL-23, DINO_GROUND_LEVEL-22,
                        DINO_GROUND_LEVEL-21, DINO_GROUND_LEVEL-20, DINO_GROUND_LEVEL-19, DINO_GROUND_LEVEL-17,
                        DINO_GROUND_LEVEL-16, DINO_GROUND_LEVEL-14, DINO_GROUND_LEVEL-13, DINO_GROUND_LEVEL-11,
                        DINO_GROUND_LEVEL-9, DINO_GROUND_LEVEL-7, DINO_GROUND_LEVEL-5, DINO_GROUND_LEVEL-2, DINO_GROUND_LEVEL};

const uint8_t *ground_images[] = {ground_flat, ground_bump, ground_hole};
const uint8_t *dino_images[] = {dinosaur_still, 
                                dinosaur_running_1, dinosaur_running_2, 
                                dinosaur_ducking_1, dinosaur_ducking_2, 
                                dinosaur_dead_1, dinosaur_dead_2};

const uint8_t *dino_masks[] = {dinosaur_still_mask, 
                               dinosaur_running_1_mask, dinosaur_running_2_mask, 
                               dinosaur_ducking_1_mask, dinosaur_ducking_2_mask, 
                               dinosaur_dead_2_mask, dinosaur_dead_2_mask};

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

  renderGround();
  drawDino();

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

void controlDino() {
  if(!dino.jumping) {
    if(arduboy.justPressed(A_BUTTON)) {
      dino.jumping = true;
      dino.jumpIndex = 0;
    }
    if(arduboy.justPressed(B_BUTTON)) {
      if(dino.stance != Stance::Ducking2) {
        dino.stance = Stance::Ducking1;
      }
    }
    if(arduboy.pressed(LEFT_BUTTON) && dino.x > 0) {
      dino.x--;
    }
    if(arduboy.pressed(RIGHT_BUTTON) && dino.x < 100) {
      dino.x++;
    }
    if(arduboy.notPressed(B_BUTTON) &&
      (dino.stance == Stance::Ducking1 || dino.stance == Stance::Ducking2)) {
      dino.stance = Stance::Running1;
      }
  }
}

void updateDino() {
  if(dino.jumping) {
    dino.y = jumpCoords[dino.jumpIndex];
    dino.jumpIndex++;

    if(dino.jumpIndex == sizeof(jumpCoords)) {
      dino.jumping = false;
      dino.jumpIndex = 0;
      dino.y = DINO_GROUND_LEVEL;
    }
  } else {
    if(arduboy.everyXFrames(3)) {
      switch(dino.stance) {
        case Stance::Running1:
          dino.stance = Stance::Running2;
          break;

        case Stance::Running2:
          dino.stance = Stance::Running1;
          break;

        case Stance::Ducking1:
          dino.stance = Stance::Ducking2;
          break;

        case Stance::Ducking2:
          dino.stance = Stance::Ducking1;
          break;

        case Stance::Dead1:
          dino.stance = Stance::Dead2;
          break;

        default:
          break;
      }
    }
  }
}

void drawDino() {
  uint8_t imageIndex = static_cast<uint8_t>(dino.stance);

  dino.image = dino_images[imageIndex];
  dino.mask = dino_masks[imageIndex];
  Sprites::drawExternalMask(dino.x, dino.y - getImageHeight(dino.image), dino.image, dino.mask, 0, 0);

}