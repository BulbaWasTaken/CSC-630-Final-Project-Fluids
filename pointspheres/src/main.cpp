#include <iostream>
#include <memory>
#include <string>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "render/shader.h"
#include "render/model.h"
#include "render/particle_system.h"
#include "utils/camera.h"
#include "utils/matrix_stack.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Application : public EventCallbacks
{
public:
    void frameBuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);

		aspect_ratio = (float)width / height;
        
        SCR_WIDTH = width;
        SCR_HEIGHT = height;

        title = std::to_string(SCR_WIDTH) + "x" + std::to_string(SCR_HEIGHT) + "@" + std::to_string(fps);

        glfwSetWindowTitle(window, title.c_str());

		if (std::isnan(aspect_ratio))
		{
			aspect_ratio = 0.00001f;
			draw = false;
		}  
        else 
            draw = true;

		if (draw) 
            render();
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(getWindow(), GL_TRUE);
		}
		else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			camera->resetOrientation();
		}
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera->processMouseScroll((float)yoffset);
    }

    void mousePos_callback(GLFWwindow* window, double xPosIn, double yPosIn)
    {
        float xPos = (float)xPosIn;
		float yPos = (float)yPosIn;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{   
            // debug output
			// std::cout << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;
			if (first_mouse)
			{
				lastX = xPos;
				lastY = yPos;
				first_mouse = false;
			}

			float xOffset = xPos - lastX;
			float yOffset = lastY - yPos;

			camera->processMouseMovement(xOffset, yOffset);
		}
		lastX = xPos;
		lastY = yPos;
    }

    void process_Input(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->processKeyboard(FORWARD, delta_time);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->processKeyboard(BACKWARD, delta_time);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->processKeyboard(LEFT, delta_time);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->processKeyboard(RIGHT, delta_time);
    }

    GLFWwindow* getWindow() const
    {
        return scene->getWindow();
    }

    void initGeom()
    {   
        particle_system = std::make_shared<ParticleSystem>();
    }

    void init() 
    {
        scene = std::make_shared<Scene>();
        scene->init(SCR_WIDTH, SCR_HEIGHT);
        scene->setEventCallbacks(this);
        
        std::string vert_file = shader_dir + "point_vert.glsl";
        std::string frag_file = shader_dir + "point_frag.glsl";

        std::cout << vert_file << std::endl;
        std::cout << frag_file << std::endl;

        point_shader = std::make_shared<Shader>();
        point_shader->init(vert_file, frag_file);
        
        camera = std::make_shared<fps_Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

        initGeom();
    }

    void shutdown()
    {
        scene->shutdown();
        point_shader->free();
    }

    void render()
    {   
        float current_frame = (float) glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        
        fps = std::round(1.0f / delta_time);
        
        update += delta_time;

        if (update >= 1.0f)
        {
            std::ostringstream oss;
            oss << SCR_WIDTH << "x" << SCR_HEIGHT << "@" << fps;

            title = oss.str();

            glfwSetWindowTitle(getWindow(), title.c_str());
            update = 0.0f;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        point_shader->use();

        auto projection = std::make_shared<MatrixStack>();
        auto model = std::make_shared<MatrixStack>();

        projection->pushMatrix();
        projection->perspective(glm::radians(camera->getZoom()), aspect_ratio, 0.1f, 100.0f);
        point_shader->setMat4("proj", projection->topMatrix());

        glm::mat4 view = camera->getViewMatrix();
        point_shader->setMat4("view", view);

        point_shader->setVec3("viewPos", camera->getPosition());
        point_shader->setVec3("aColor", glm::vec3(1.0f, 0.0f, 0.0f));

        particle_system->render(point_shader, delta_time);

        process_Input(getWindow());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("CSC 630 Fluid Simulation");
        ImGui::Text("ImGUI Test:");
        ImGui::SliderFloat("Number of Particles", particle_system->getNumPoints(), 0.5f, 1000.0f);
        ImGui::SliderFloat("Density", &density, 0.5f, 100.0f);
        ImGui::SliderFloat("Pressure Force", &pressureForce, 0.5f, 100.0f);
        ImGui::SliderFloat("Viscosity", &viscosity, 0.5f, 100.0f);
        ImGui::SliderFloat("Damping", &damping, 0.5f, 100.0f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(getWindow());
        glfwPollEvents();
    }
        
private:
    std::shared_ptr<Scene> scene = nullptr;

    bool draw = true;

    int SCR_WIDTH = 1024;
    int SCR_HEIGHT = 720;

    float aspect_ratio = (float)SCR_WIDTH / SCR_HEIGHT;

    std::string shader_dir = "./Fluids/pointspheres/res/shaders/";
    std::string obj_dir = "./Fluids/pointspheres/res/objects/";

    std::shared_ptr<Shader> point_shader = nullptr;

    std::shared_ptr<fps_Camera> camera = nullptr;

    std::shared_ptr<ParticleSystem> particle_system = nullptr;

    bool first_mouse = true;

    float lastX = SCR_WIDTH / 2.0f, 
          lastY = SCR_HEIGHT / 2.0f;

    float delta_time = 0.0f,
          last_frame = 0.0f;

    float fps = 0.0f, update = 0.0f;

    std::string title 
        = std::to_string(SCR_WIDTH) + "x" + std::to_string(SCR_HEIGHT) + "@" + std::to_string(fps);
    float density = 5;
    float pressureForce = 5;
    float viscosity = 5;
    float damping = 5;
};

int main()
{
    std::shared_ptr<Application> app = std::make_shared<Application>();
    
    app->init();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(app->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(app->getWindow()))
    {
        app->render();
    }

    app->shutdown();

    return 0;
}