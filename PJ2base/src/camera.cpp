#include "camera.h"

fps_Camera::fps_Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	speed(10.0f),
	mouseSensitivity(0.1f),
	zoom(45.0f)
{
	Position = position;
	worldUp = up;
	Yaw = yaw;
	Pitch = pitch;

	updateCameraVectors();
}

fps_Camera::fps_Camera
(
	float x,
	float y,
	float z,
	float upX,
	float upY,
	float upZ,
	float yaw,
	float pitch
) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	speed(10.0f),
	mouseSensitivity(0.1f),
	zoom(45.0f)
{
	Position = glm::vec3(x, y, z);
	worldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;

	updateCameraVectors();
}


glm::mat4 fps_Camera::getViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void fps_Camera::processKeyboard(CameraDirection direction, float dTime)
{
	float velocity = speed * dTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
}

void fps_Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constraintPitch)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	if (constraintPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	updateCameraVectors();
}

void fps_Camera::processMouseScroll(float yOffset)
{
	zoom -= (float)yOffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void fps_Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, worldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

void fps_Camera::setLock()
{
	LOCKED = !LOCKED;
}

void fps_Camera::setPosition(glm::vec3 position)
{
	Position = position;
}

void fps_Camera::resetOrientation()
{
	Position = glm::vec3(0.0f, 0.0f, 3.0f);
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	Pitch = 0.0f;
	Yaw = -90.0f;
	zoom = 45.0f;

	updateCameraVectors();
}

float fps_Camera::getZoom() const
{
	return zoom;
}

glm::vec3 fps_Camera::getPosition() const
{
	return Position;
}