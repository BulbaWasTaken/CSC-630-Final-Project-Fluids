#pragma once

#include "MatrixStack.h"
#include "WindowManager.h"
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
  float camSpeed;
  float deltaTime;
  float lastFrame;

  float lastX;
  float lastY;
  float yaw;
  float pitch;

  bool firstMouse = true;
  bool movingForward;
  bool movingBack;
  bool movingLeft;
  bool movingRight;

  glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

  std::shared_ptr<MatrixStack> View;

public:
  Camera();
  // get the View ptr for Matrix Stack
  void setViewPtr(std::shared_ptr<MatrixStack> View);
  // uses LookAt to set the current view based on camera position
  void look();
  // getter for camPos
  glm::vec3 getCameraPos();
  // setter for camFront
  void setCameraFront();
  // getter for camFront
  glm::vec3 getCameraFront();
  // setter for camUp
  void setCameraUp();
  // getter for camUp
  glm::vec3 getCameraUp();
  // setter for camSpeed
  void setCamSpeed(float currentFrame, bool sprint);
  // handle mouse look
  void lookAround(GLFWwindow* window, double xpos, double ypos);

  // movement setters
  void moveForward();
  void setMovingForward(bool value);
  void moveBack();
  void setMovingBackward(bool value);
  void moveRight();
  void setMovingRight(bool value);
  void moveLeft();
  void setMovingLeft(bool value);


};