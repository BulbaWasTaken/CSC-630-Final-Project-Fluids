#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream> 

enum CameraDirection {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera 
{
public: 
	virtual glm::mat4 getViewMatrix() = 0;

	virtual void processKeyboard(CameraDirection direction, float dTime) = 0;

	virtual void processMouseMovement(float xOffset, float yOffset, GLboolean constraintPitch = true) = 0;

	virtual void processMouseScroll(float yOffset) = 0;

	virtual void updateCameraVectors() = 0;
};

class fps_Camera : public Camera 
{
public:
	fps_Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = -90.f,
		float pitch = 0.0f
	);

	fps_Camera(float x, float y, float z, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 getViewMatrix() override;

	void processKeyboard(CameraDirection direction, float dTime) override;

	void processMouseMovement(float xOffset, float yOffset, GLboolean constraintPitch = true) override;

	void processMouseScroll(float yOffset) override;

	void updateCameraVectors() override;

	void setLock();

	void setPosition(glm::vec3 position);
	
	void resetOrientation();

	float getZoom() const;

	glm::vec3 getPosition()const;

protected:
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 10.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	
private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 worldUp;

	float Yaw = 0.0f;
	float Pitch = 0.0f;

	float speed = 0.0f;
	float mouseSensitivity = 0.0f;
	float zoom = 0.0f;

	bool LOCKED = false;
};


#endif