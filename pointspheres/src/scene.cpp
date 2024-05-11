#include "scene.h"


Scene* Scene::instance = nullptr;

void errorcallback(int error, const char* desc)
{
    std::cerr << desc << std::endl;
}

Scene::Scene()
{
    if (instance)
    {
        std::cerr << "Instance of scene already running" << std::endl;
    }

    instance = this;
}

Scene::~Scene()
{
    if (instance == this)
    {
        instance = nullptr;
    }
}

bool Scene::init(const int width, const int height)
{
    glfwSetErrorCallback(errorcallback);

    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Error: Couldn't initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

    std::string title = std::to_string(width) + "x" + std::to_string(height) + "@";

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    

    if (!window)
    {   
        std::cerr << "Error: Couldn't initliaze window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    // Load GLAD
    if (!gladLoadGL())
    {
        std::cerr << "Error: Couldn't load GLAD" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);

    return true;
}

void Scene::shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Scene::getWindow() const 
{
    return window;
}

void Scene::setTitle(std::string title)
{
    glfwSetWindowTitle(getWindow(), title.c_str());
}

void Scene::setEventCallbacks(EventCallbacks* callbacks_in)
{
	callbacks = callbacks_in;
}

void Scene::resizeCallback(GLFWwindow* window, int in_width, int in_height)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->frameBuffer_size_callback(window, in_width, in_height);
	}
}

void Scene::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->key_callback(window, key, scancode, action, mods);
	}
}

void Scene::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->scroll_callback(window, xoffset, yoffset);
	}
}

void Scene::mousePosCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->mousePos_callback(window, xPosIn, yPosIn);
	}
}

void Scene::processInput(GLFWwindow* window)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->process_Input(window);
	}
}
