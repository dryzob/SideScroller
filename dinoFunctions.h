#pragma once
#include "images.h"

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

//*********************************
// Function names
//*********************************
void controlDino();
void updateDino();
void drawDino();

Dino dino = {0, DINO_GROUND_LEVEL, Stance::Standing, false, 0, dinosaur_still, dinosaur_still_mask};

uint8_t jumpCoords[] = {62, 60, 57, 55, 53, 51, 49, 47, 45, 44, 42, 40, 39, 38, 36, 35, 34, 33,
                        32, 31, 30, 29, 29, 28, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 27, 27,
                        27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 42, 44, 45, 47,
                        49, 51, 53, 55, 57, 60, 62};

const uint8_t *dino_images[] = {dinosaur_still, 
                                dinosaur_running_1, dinosaur_running_2, 
                                dinosaur_ducking_1, dinosaur_ducking_2, 
                                dinosaur_dead_1, dinosaur_dead_2};
const uint8_t *dino_masks[] = {dinosaur_still_mask, 
                               dinosaur_running_1_mask, dinosaur_running_2_mask, 
                               dinosaur_ducking_1_mask, dinosaur_ducking_2_mask, 
                               dinosaur_dead_2_mask, dinosaur_dead_2_mask};


//***********************************
// Function Definitions
//***********************************
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
    if(arduboy.everyXFrames(6)) {
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