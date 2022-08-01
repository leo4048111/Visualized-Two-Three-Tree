#include "stdafx.h"
#include "Menu.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.hpp"

#define BAIL_IF_ERROR(ok) if(!ok) goto bail

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main()
{
	FreeConsole();
	GLFWwindow* window = nullptr;
	GLFWimage icon = {32, 32, nullptr};
	int channels = 3;
	glfwSetErrorCallback(glfw_error_callback);
	//init glfw
	BAIL_IF_ERROR(glfwInit());
	LOG("[%s] %s\n", "Info", "GLFW has been initialized...");
	//init window
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(1280, 720, "TwoThreeTree Demo", NULL, NULL);
	BAIL_IF_ERROR(window);
	LOG("[%s] %s\n", "Info", "Window has been created...");
	//set icon
	icon.pixels = stbi_load("icon.jpeg", &icon.width, &icon.height, &channels, 4);
	glfwSetWindowIcon(window, 1, &icon);
	//context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); //enable vsync

	//init menu
	BAIL_IF_ERROR(g_menu->init(window));
	LOG("[%s] %s\n", "Info", "Application is running...");
	g_menu->mainloop();

	//cleanup
	g_menu->destroy();
	glfwDestroyWindow(window);
	glfwTerminate();

	bail:
	return 0;
}