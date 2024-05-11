#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class EventCallbacks
{
public:
    // Interface to circumvent C style global callbacks
    virtual void frameBuffer_size_callback(GLFWwindow* window, int width, int height) = 0;
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void mousePos_callback(GLFWwindow* window, double xPosIn, double yPosIn) = 0;
	virtual void process_Input(GLFWwindow* window) = 0;
};

class Scene
{
public:
    Scene();
    ~Scene();

    bool init(int const width, int const height);
    void shutdown();

    GLFWwindow* getWindow() const;

    void setTitle(std::string title);

    void setEventCallbacks(EventCallbacks* callbacks_in);

protected:
    // Allows us to implement singleton OOP design
    static Scene* instance;

    GLFWwindow* window = nullptr;
    EventCallbacks* callbacks = nullptr;

private:
    static void resizeCallback(GLFWwindow* window, int in_width, int in_height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mousePosCallback(GLFWwindow* window, double xPosIn, double yPosIn);
	static void processInput(GLFWwindow* window);
};

#endif