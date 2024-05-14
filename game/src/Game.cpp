//
// Created by anton on 5/13/24.
//

#include "Game.h"
#include "Background.h"
#include "Floor.h"
#include "LightSource.h"
#include "WorldWindow.h"
#include "utils.h"

#include <SFML/Graphics.hpp>

#include <loguru.hpp>

#include <chrono>
#include <iostream>

Game::Game()
{
  // objects for rendering
  Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(ScreenWidth, ScreenHeight),
                                              Title);
//                                              sf::Style::Fullscreen);
  Window->setFramerateLimit(100);
  RenderTex = std::make_unique<sf::RenderTexture>();
  RenderTex->create(ScreenWidth, ScreenHeight);
  RenderTex->display();
  ScreenSprite = std::make_unique<sf::Sprite>(RenderTex->getTexture());

  // keyboard
  Key2IsPressed["left"] = false;
  Key2IsPressed["right"] = false;

  // game objects
  Lantern = std::make_unique<LightSource>(0, 0);
  Backgr = std::make_unique<Background>(ScreenWidth, ScreenHeight);
  FloorObj = std::make_unique<Floor>(-3.0f, 3.0f, 0.01f);
  WorldWindowObj = std::make_unique<WorldWindow>(
      sf::Vector2f(0, 0), sf::Vector2f(3, 2),
      FloorObj->getStartX(), FloorObj->getEndX());

  // shaders
  if (!sf::Shader::isAvailable())
    throw std::runtime_error("Shaders are not available!");
  PostprocessingShader = std::make_unique<sf::Shader>();
  PostprocessingShader->loadFromFile("../../game/shaders/lighting_vertex_shader.glsl",
                              "../../game/shaders/lighting_fragment_shader.glsl");
  // always use current texture
  // (the texture of the current object being drawn)
  PostprocessingShader->setUniform("texture", sf::Shader::CurrentTexture);
}

Game::~Game() = default;

void Game::handleUserInput()
{
  sf::Event Event;
  while (Window->pollEvent(Event))
  {
    if (Event.type == sf::Event::Closed)
      Window->close();
    if (Event.type == sf::Event::KeyPressed)
    {
      if (Event.key.code == sf::Keyboard::Left)
        Key2IsPressed["left"] = true;
      else if (Event.key.code == sf::Keyboard::Right)
        Key2IsPressed["right"] = true;
      else if (Event.key.code == sf::Keyboard::Escape)
        Window->close();
    }
    else if (Event.type == sf::Event::KeyReleased)
    {
      if (Event.key.code == sf::Keyboard::Left)
        Key2IsPressed["left"] = false;
      else if (Event.key.code == sf::Keyboard::Right)
        Key2IsPressed["right"] = false;
    }
  }
}

void Game::processLogic(float FrameDrawingTimeMs)
{
  float LanternFrameSpeed = FrameDrawingTimeMs / 1000 * 1.0;
  sf::Vector2f LanternDxDy;
  if (Key2IsPressed["left"])  LanternDxDy.x -= LanternFrameSpeed;
  if (Key2IsPressed["right"]) LanternDxDy.x += LanternFrameSpeed;

  Lantern->setPosition(Lantern->getPosition() + LanternDxDy);

  WorldWindowObj->updateByPlayerPos(Lantern->getPosition());
}

void Game::renderScene()
{
  Window->clear(sf::Color::Black);

  Backgr->draw(*RenderTex, *WorldWindowObj);
//  Lantern->draw(*RenderTex, *WorldWindowObj);
  FloorObj->draw(*RenderTex, *WorldWindowObj);

  PostprocessingShader->setUniform("world_window_center", WorldWindowObj->getCenter());
  PostprocessingShader->setUniform("world_window_dimensions", WorldWindowObj->getSize());
  PostprocessingShader->setUniform("world_light_pos", Lantern->getPosition());

  Window->draw(*ScreenSprite, PostprocessingShader.get());
  Window->display();
}

void Game::run()
{
  auto BeginTime = std::chrono::high_resolution_clock::now();
  long long FrameCount = 0;
  float FrameDrawingTimeMs = 0;

  while (Window->isOpen())
  {
    auto BeginFrameTime = std::chrono::high_resolution_clock::now();

    handleUserInput();

    processLogic(FrameDrawingTimeMs);

    renderScene();

    FrameCount++;

    auto FrameTimeDelta = std::chrono::high_resolution_clock::now() - BeginFrameTime;
    FrameDrawingTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(FrameTimeDelta).count();
  }

  auto TimeDelta = std::chrono::high_resolution_clock::now() - BeginTime;
  auto TimeDeltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(TimeDelta);
  std::cout << "Avg FPS: " << 1000.0 / (static_cast<double>(TimeDeltaMs.count()) / FrameCount) << "\n";
  std::cout << "Total frames: " << FrameCount << "\n";
}