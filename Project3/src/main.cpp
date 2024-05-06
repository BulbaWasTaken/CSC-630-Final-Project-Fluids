/* Lab 6 base code - transforms using matrix stack built on glm
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>
#include <string.h>
#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Camera.h"
#include "Texture.h"
#include "stb_image.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <CL/cl.h>
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{
public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> cubeProg;
	std::shared_ptr<Program> floorProg;

	// Shape to be used (from obj file)
	shared_ptr<Shape> head;
	shared_ptr<Shape> body;
	shared_ptr<Shape> arm_upper;
	shared_ptr<Shape> arm_lower;
	shared_ptr<Shape> leg_upper;
	shared_ptr<Shape> leg_lower;
	shared_ptr<Shape> flower;
	shared_ptr<Shape> skyboxShape;
	shared_ptr<Shape> boat;
	shared_ptr<Shape> floor;

	vector<shared_ptr<Shape>> flowerMesh;
	vector<shared_ptr<Shape>> boatMesh;

	Camera camera;
	Texture floorText;


	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	bool animate = false;

	float lightTrans = 0;
	float gRot = 0;
	float gTrans = 0;
	float rotAmt = 0;
	float rotInc = 0.01;

	float deg45 = 0.785398;
	float deg90 = 1.5708;
	float deg135 = 2.35619;
	

	double mouseX;
	double mouseY;

	GLuint cubeMapSkyBox;
	char* testOpenCL() {
		cl_platform_id platform_id = NULL;
		cl_device_id device_id = NULL;
		cl_uint ret_num_devices;
		cl_uint ret_num_platforms;
		char device_name[1024];

		// Get platform and device information
		cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

		// Get the device name
		ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 1024, device_name, NULL);

		// Print the device name
		std::cout << "Graphics Card: " << device_name << std::endl;
	
		return device_name;
	
	}
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		if (key == GLFW_KEY_O && action == GLFW_RELEASE) {
			lightTrans--;
		}
		if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
			lightTrans++;
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			camera.setMovingForward(true);
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			camera.setMovingForward(false);
		}

		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			camera.setMovingBackward(true);
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			camera.setMovingBackward(false);
		}

		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			camera.setMovingLeft(true);
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			camera.setMovingLeft(false);
		}

		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			camera.setMovingRight(true);
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			camera.setMovingRight(false);
		}

		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			animate = !animate;
		}
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
			camera.setCamSpeed(static_cast<float>(glfwGetTime()), true);
		}
		else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
			camera.setCamSpeed(static_cast<float>(glfwGetTime()), false);
		}
	}

	void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			cout << "X: " << mouseX << " Y: " << mouseY << endl;
		}
	}

	void moveCallback(GLFWwindow* window, double xpos, double ypos) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwGetCursorPos(window, &mouseX, &mouseY);
		camera.lookAround(window, mouseX, mouseY);
	}
	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightP");
		prog->addUniform("viewP");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");

		floorProg = make_shared<Program>();
		floorProg->setVerbose(true);
		floorProg->setShaderNames(resourceDirectory + "/floor_vert.glsl", resourceDirectory + "/floor_frag.glsl");
		floorProg->init();
		floorProg->addUniform("P");
		floorProg->addUniform("V");
		floorProg->addUniform("M");
		floorProg->addUniform("floorText"); 
		floorProg->addAttribute("vertPos");
		floorProg->addAttribute("vertNor");



		mouseX = 0.0f;
		mouseY = 0.0f;

	}


	void initGeom(const std::string& objectDirectory)
	{
		
		// Initialize meshs.
		head = make_shared<Shape>();
		head->loadMesh(objectDirectory + "/robot/head.obj");
		head->init();

		body = make_shared<Shape>();
		body->loadMesh(objectDirectory + "/robot/body.obj");
		body->init();

		arm_upper = make_shared<Shape>();
		arm_upper->loadMesh(objectDirectory + "/robot/arm_upper.obj");
		arm_upper->init();

		arm_lower = make_shared<Shape>();
		arm_lower->loadMesh(objectDirectory + "/robot/arm_lower.obj");
		arm_lower->init();

		leg_upper = make_shared<Shape>();
		leg_upper->loadMesh(objectDirectory + "/robot/leg_upper.obj");
		leg_upper->init();

		leg_lower = make_shared<Shape>();
		leg_lower->loadMesh(objectDirectory + "/leg_lower.obj");
		leg_lower->init();

		boat = make_shared<Shape>();
		boat->loadMesh(objectDirectory + "/Boat.obj");
		boat->init();

		floor = make_shared<Shape>();
		floor->loadMesh(objectDirectory + "/cube.obj");
		floor->init();
		floorText.setFilename(objectDirectory + "/water.jpg");
		floorText.init();
		floorText.setUnit(1);
		floorText.setWrapModes(GL_REPEAT, GL_REPEAT);


		skyboxShape = make_shared<Shape>();
		skyboxShape->loadMesh(objectDirectory + "/cube.obj");
		skyboxShape->init();
		string facesCubeMap[6] = {
			objectDirectory + "/right.jpg",
			objectDirectory + "/left.jpg",
			objectDirectory + "/top.jpg",
			objectDirectory + "/bottom.jpg",
			objectDirectory + "/front.jpg",
			objectDirectory + "/back.jpg"
		};

		glGenTextures(1, &cubeMapSkyBox);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapSkyBox);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (unsigned int i = 0; i < 6; i++)
		{
			int width, height, nrChannels;
			unsigned char* data = stbi_load(facesCubeMap[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				stbi_set_flip_vertically_on_load(false);
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB,
					width,
					height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Failed to load texture: " << facesCubeMap[i] << std::endl;
				stbi_image_free(data);
			}
		}

		// init multi shape object
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		// loads object into shapes and stores in TOshapes
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (objectDirectory + "/cartoon_flower.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			for (int i = 0; i < TOshapes.size(); i++) {
				// Initialize each mesh.
				shared_ptr<Shape> s;
				s = make_shared<Shape>();
				s->loadMultiMesh(TOshapes[i]);
				s->init();

				flowerMesh.push_back(s);
			}
		}

		vector<tinyobj::shape_t> boatShapes;
		vector<tinyobj::material_t> boatobjMaterials;
		bool brc = tinyobj::LoadObj(boatShapes, boatobjMaterials, errStr, (objectDirectory + "/Boat.obj").c_str());
		if (!brc) {
			cerr << errStr << endl;
		}
		else {
			for (int i = 0; i < boatShapes.size(); i++) {
				shared_ptr<Shape> s;
				s = make_shared<Shape>();
				s->loadMultiMesh(boatShapes[i]);
				s->init();

				boatMesh.push_back(s);
			}
		}
	}

	void SetMaterial(int i) {

    	prog->bind();
    	switch (i) {
    	case 0: // color by normal
    		glUniform3f(prog->getUniform("MatAmb"), -1, -1, -1);
    		glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
    		glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
    		glUniform1f(prog->getUniform("MatShine"), 120.0);
    	break;
    	case 1: // pink
    		glUniform3f(prog->getUniform("MatAmb"), 0.93, 0.13, 0.24);
    		glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
    		glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    		glUniform1f(prog->getUniform("MatShine"), 4.0);
    	break;
    	case 2: //brass
    		glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
    		glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
    		glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
    		glUniform1f(prog->getUniform("MatShine"), 27.9);
    	break;
  	}
	}

	void drawFlower(shared_ptr<MatrixStack> Model)
	{
		Model->pushMatrix();
			Model->translate(vec3(-0.4, -1, 0));
			Model->rotate(deg90, vec3(0, 0, 1));
			Model->rotate(deg45, vec3(0, 1, 0));
			Model->scale(7);
			SetMaterial(1);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			// draw each shape in flower
			for (int i = 0; i < flowerMesh.size(); i++) {
				flowerMesh[i]->draw(prog);
			}
		Model->popMatrix();
	}
	void drawBoat(shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		Model->translate(vec3(-0.4, -5, -4));
		Model->scale(7);
		SetMaterial(2);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		for (int i = 0; i < boatMesh.size(); i++) {
			boatMesh[i]->draw(prog);
		}
		Model->popMatrix();
	}

	void drawRobot(shared_ptr<MatrixStack> Model)
	{
		Model->pushMatrix();
		//global rotate (the whole scene)
		Model->rotate(gRot, vec3(0, 1, 0));
		
			// draw body
			Model->pushMatrix(); // body push
				Model->translate(vec3(0, 0.5, -9 + gTrans));
				Model->rotate(deg45, vec3(0, 1, 0)); 
				SetMaterial(0);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				body->draw(prog);

				// draw head
				Model->pushMatrix(); // head push
					Model->translate(vec3(0, 1.5, 0));
					Model->rotate(rotAmt, vec3(0, 1, 0));
					SetMaterial(0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					head->draw(prog);
				Model->popMatrix(); // head pop

				
				// draw right arm
				// upper
				Model->pushMatrix(); // right upper arm push
					Model->translate(vec3(0, 1.5, -1.5));
					Model->rotate(rotAmt, vec3(0, 0, -1));
					SetMaterial(0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					arm_upper->draw(prog);
			
					// lower
					Model->pushMatrix(); // right lower arm push
						Model->translate(vec3(0, -1.5, 0));
						Model->rotate(deg45, vec3(0, 0, -1));
						SetMaterial(0);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						arm_lower->draw(prog);

						drawFlower(Model);

					Model->popMatrix(); // right lower arm pop
				Model->popMatrix(); // right upper arm pop
				
				// draw left arm
				// upper
				Model->pushMatrix(); // left upper arm push
					Model->translate(vec3(0, 1.5, 1.5));
					Model->rotate(rotAmt, vec3(0, 0, 1));
					SetMaterial(0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					arm_upper->draw(prog);
			
					// lower
					Model->pushMatrix(); // left lower arm push
						Model->translate(vec3(0, -1.5, 0));
						SetMaterial(0);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						arm_lower->draw(prog);
					Model->popMatrix(); // left lower arm pop
				Model->popMatrix(); // left upper arm pop

				
				// draw right leg
				// upper
				Model->pushMatrix(); // right upper leg push
					Model->translate(vec3(0, -1.5, -0.5));
					if (animate) Model->rotate(rotAmt, vec3(0, 0, 1));
					else Model->rotate(0, vec3(0, 0, 1));
					SetMaterial(0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					leg_upper->draw(prog);

					Model->pushMatrix(); // right lower leg push
						Model->translate(vec3(0, -1.5, 0));
						if (animate) Model->rotate(deg45, vec3(0, 0, 1));
						else Model->rotate(0, vec3(0, 0, 1));
						SetMaterial(0);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						leg_upper->draw(prog);
					Model->popMatrix(); // right lower leg pop
				Model->popMatrix(); // right upper leg pop
				
				// draw left leg
				// upper
				Model->pushMatrix(); // left upper leg push
					Model->translate(vec3(0, -1.5, 0.5));
					if (animate) Model->rotate(rotAmt, vec3(0, 0, -1));
					else Model->rotate(0, vec3(0, 0, 1));
					SetMaterial(0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					leg_upper->draw(prog);

					Model->pushMatrix(); // left lower leg push
						Model->translate(vec3(0, -1.5, 0));
						if (animate) Model->rotate(deg45, vec3(0, 0, 1));
						else Model->rotate(0, vec3(0, 0, 1));
						SetMaterial(0);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						leg_upper->draw(prog);
					Model->popMatrix(); // left lower leg pop
				Model->popMatrix(); // left upper leg pop
			Model->popMatrix(); // body pop		
		Model->popMatrix();

		if (animate)
		{
			rotAmt += rotInc;

			if (rotAmt >= deg45)
			{
				rotAmt = deg45;
				rotInc *= -1;
			}

			else if (rotAmt <= -deg45)
			{
				rotAmt = -deg45;
				rotInc *= -1;
			}
		}
	}
	void drawSkyBox(shared_ptr<MatrixStack> Model) {
		glDepthFunc(GL_LEQUAL);
		Model->pushMatrix();
		Model->translate(vec3(0, 15, 0));
		Model->scale(100);

		glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapSkyBox);
		
		skyboxShape->draw(cubeProg);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glDepthFunc(GL_LESS);
		Model->popMatrix();
	}

	void drawFloor(shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		Model->translate(vec3(0, -55, 0));
		Model->rotate(deg90, vec3(1, 0, 0));
		Model->scale(100);

		glActiveTexture(GL_TEXTURE1);
		floorText.bind(floorProg->getUniform("floorText"));

		glUniformMatrix4fv(floorProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		floor->draw(floorProg);

		floorText.unbind();

		Model->popMatrix();
	}

	void render()
	{
		camera.moveForward();
		camera.moveBack();
		camera.moveLeft();
		camera.moveRight();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View is identity - for now
		View->pushMatrix();
		View->lookAt(camera.getCameraPos(), camera.getCameraPos() + camera.getCameraFront(), camera.getCameraUp());

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("lightP"), lightTrans+2, 3, 5);
		glUniform3f(prog->getUniform("viewP"), camera.getCameraPos().x, camera.getCameraPos().y, camera.getCameraPos().z);
		


		drawRobot(Model);
		drawBoat(Model);

		prog->unbind();
		
		floorProg->bind();
		glUniformMatrix4fv(floorProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(floorProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		drawFloor(Model);
		floorProg->unbind();

		cubeProg->bind();
			glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			drawSkyBox(Model);
		cubeProg->unbind();



		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();


	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";
	std::string objectDir = "../objects";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;


	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(objectDir);


	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(windowManager->getHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	application->testOpenCL();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{

		// Render scene.
		application->render();
		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGUI window creation
		ImGui::Begin("My name is window, ImGUI window");
		// Text that appears in the window
		ImGui::Text("Hello there adventurer!");

		// Ends the window
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Quit program.
	windowManager->shutdown();
	return 0;
}
