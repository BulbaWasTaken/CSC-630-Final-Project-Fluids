#include "Camera.h"
#include <iostream>
using namespace std;

Camera::Camera() : lastFrame(0), deltaTime(0), camSpeed(0.1f),
                    movingForward(false), movingBack(false),
                    movingLeft(false), movingRight(false) {}

void Camera::setViewPtr(std::shared_ptr<MatrixStack> view)
{
  View = view;
}

void Camera::look()
{
// Please fill in
    View->lookAt(camPos, camPos + camFront, camUp);
}

glm::vec3 Camera::getCameraPos()
{
  return camPos;
}
void Camera::setCameraFront()
{
    // Calculate new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(front);
}

glm::vec3 Camera::getCameraFront()
{
    return camFront;
}

void Camera::setCameraUp()
{
    // Camera up vector always remains constant
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getCameraUp()
{
    return camUp;
}
void Camera::setCamSpeed(float currentFrame, bool sprint)
{
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  float speed;
  if (sprint) speed = 2.0f;
  else speed = 0.5f;
  camSpeed = speed * deltaTime;
}

void Camera::moveForward() {
    if (movingForward) {
        camPos += camSpeed * camFront;
    }
}

void Camera::setMovingForward(bool value) {
    movingForward = value;
}

void Camera::moveBack(){
    if (movingBack) {
        camPos -= camSpeed * camFront;
    }

}

void Camera::setMovingBackward(bool value) {
    movingBack = value;
}

void Camera::moveRight(){
    if (movingRight) {
        camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    }
}

void Camera::setMovingRight(bool value) {
    movingRight = value;
}
void Camera::moveLeft(){
    if (movingLeft) {
        camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    }
}
void Camera::setMovingLeft(bool value) {
    movingLeft = value;
}

void Camera::lookAround(GLFWwindow* window, double xpos, double ypos)
{
// Please fill in
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
// Please fill in

  direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  direction.y = sin(glm::radians(pitch));
  direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  camFront = glm::normalize(direction);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  if (xpos >= width - 1) {
    glfwSetCursorPos(window, 0, ypos);
    lastX = 0;
  }
  else if (xpos <= 0) {
    glfwSetCursorPos(window, width - 1, ypos);
    lastX = width - 1;
  }

  if (ypos >= height - 1) {
      glfwSetCursorPos(window, xpos, 0);
      lastY = 0;    
  }
  else if (ypos <= 0) {
      glfwSetCursorPos(window, xpos, height-1);
      lastY = height - 1;
  }

}