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

enum class GameStatus : uint8_t {
  Introduction,
  PlayGame,
  GameOver
};

enum class GroundType : uint8_t {
  Flat,
  Bump,
  Hole
};

Arduboy2 arduboy;
uint8_t groundX = 0;
uint8_t obstacleLaunchCountdown = LAUNCH_DELAY_MAX;
uint16_t score = 0;
uint16_t highScore = 0;

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
GameStatus gameStatus = GameStatus::Introduction;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);
  initEEPROM();

}

void loop() {

  switch(gameStatus) {
    case GameStatus::Introduction:
      introduction();
      break;
    case GameStatus::PlayGame:
      playGame();
      break;
    case GameStatus::GameOver:
      gameOver();
      break;
  }

  // controlDino();
  // updateDino();
  // updateObstacles();
  // collision();

  // obstacleLauncher();

  // drawObstacles();
  // renderGround();
  // drawDino();

  // if(arduboy.everyXFrames(3)) score++;
  // if(arduboy.everyXFrames(2)) arduboy.digitalWriteRGB(RED_LED, RGB_OFF);

  arduboy.display();
}

void initialiseGame() {

  for(uint8_t i = 0; i < NUMBER_OF_OBSTACLES; i++) {
    obstacles[i].enabled = false;
  }

  score = 0;
  dino.x = 2;
  dino.y = DINO_GROUND_LEVEL;
  dino.jumping = false;
  dino.stance = Stance::Standing;

}

void introduction() {
  EEPROM.get(EEPROM_SCORE, highScore);
  arduboy.clear();

  initialiseGame();

  arduboy.setCursor(17, 12);
  arduboy.print(F("Press A to play!"));

  drawGround(true);
  drawDino();
  drawScoreboard(false);
  arduboy.display();

  if (arduboy.pressed(A_BUTTON)) {
    gameStatus = GameStatus::PlayGame;
    dino.stance = Stance::Running1;
  }
}

void drawScoreboard(bool displayScore) {
  arduboy.fillRect(0, 0, WIDTH, 10, BLACK);

  if (displayScore) {
    arduboy.setCursor(1, 0);
    arduboy.print(F("Score: "));
    arduboy.setCursor(39, 0);
    if(score < 1000); arduboy.print(0);
    if(score < 100); arduboy.print(0);
    if(score < 10); arduboy.print(0);
    arduboy.print(score);

  }

  arduboy.setCursor(72, 0);
  arduboy.print(F("High: "));
  arduboy.setCursor(104, 0);
  if(score < 1000); arduboy.print(0);
  if(score < 100); arduboy.print(0);
  if(score < 10); arduboy.print(0);
  arduboy.print(highScore);

  arduboy.drawLine(0, 9, WIDTH, 9, WHITE);
}

void renderGround(bool idle) {
  if(!idle){
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
    }
    ground[0] = ground[1];
    ground[1] = ground[2];
    ground[2] = ground[3];
    ground[3] = ground[4];
    ground[4] = groundType;
  
    groundX++;

  }

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
