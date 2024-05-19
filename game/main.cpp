//
// Created by anton on 5/12/24.
//

#include <loguru.hpp>

#include <sound/facade/facade.hpp>

#include "Game.h"

void soundTest()
{
  SoundFacade Snd;
  auto SndId = Snd.ImplInstance.loadSound("../../game/resources/sound_test_mono.wav");
  auto SndSrcParams = SoundStructs::SoundSourceParams {
      .Pitch = 1.0f, .Gain = 1.0f,
      .PosX = 0, .PosY = 0, .PosZ = 0,
      .VelX = 0, .VelY = 0, .VelZ = 0,
      .Looping = false,
      .SndID = SndId
  };
  auto SndSrc = Snd.ImplInstance.createSoundSource(SndSrcParams);
  Snd.ImplInstance.playSoundSource(SndSrc);

  while (Snd.ImplInstance.isPlaying(SndSrc))
  {
  }
}

int main(int Argc, char** Argv)
{
  loguru::init(Argc, Argv);
  // clang-format off
  loguru::add_file("game.log", loguru::Append, loguru::Verbosity_MAX);
  // clang-format on

  Game GameObj;
  GameObj.run();

  return 0;
}