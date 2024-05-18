//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP

#include "glm/glm.hpp"
#include <vector>

class BaseObject {
public:
  BaseObject() = default;
  virtual ~BaseObject() = default;

  virtual std::vector<uint8_t> getVertices() = 0;
  virtual glm::vec3 getPosition() = 0;

  enum class ObjectTypes {
    TRIANGLE,
    SQUARE,
  };

  virtual ObjectTypes getType() = 0;
  virtual std::string getDumpName() = 0;
  virtual std::string getName() { return Name; }

  virtual void moveBy(glm::vec3 Vector) = 0;

protected:
  std::string Name;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
