﻿// ReSharper disable CppUninitializedNonStaticDataMember
#pragma once
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <limits>
#include  <algorithm>


#define LOG_INFO(msg) spdlog::info("{}:{} {}", __FILE__, __LINE__, std::string(msg));
#define VK_CHECKERROR(X, error) \
    if((X) != VK_SUCCESS) \
    { \
        throw std::runtime_error(std::string("Vulkan Error ") + std::string((error))); \
    }

template <class T>
T* Temp(T&& t) { return &t; }


namespace IO
{
	std::vector<char> ReadFile(const std::string& path);
}


struct QueueFamilyIndices
{
	uint32_t graphicsFamily;
	uint32_t presentFamily;
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

const int MAX_FRAMES_IN_FLIGHT = 2;

class VulkanTutorialApplication
{
public:
	void Run();

private:
	const uint32_t WIDTH = 1080;
	const uint32_t HEIGHT = 720;
	std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
	std::vector<const char*> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	bool m_enableValidationLayers = true;
	GLFWwindow* m_window;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;

	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	std::vector<const char*> GetRequiredExtensions();
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFrameBuffers;

	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	VkPipelineLayout m_pipelineLayout;
	VkRenderPass m_renderPass;
	VkPipeline m_graphicsPipeline;
	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	uint32_t currentFrame = 0;
	void InitWindow();
	void InitVulkan();
	bool CheckValidationLayerSupport();
	void SetupDebugMessenger();
	void CreateInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	VkSurfaceFormatKHR ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	QueueFamilyIndices FindQueueFamilies();
	void CreateSurface();
	SwapChainSupportDetails QuerySwapChainSupport();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffer();
	VkShaderModule CreateShaderModule(std::vector<char>& code);
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void DrawFrame();
	void CreateSyncObjects();
	void MainLoop();
	void Cleanup();
	void DestroyDebugMessenger();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
};
