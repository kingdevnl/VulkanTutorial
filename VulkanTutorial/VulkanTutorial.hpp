// ReSharper disable CppUninitializedNonStaticDataMember
#pragma once
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>


#define LOG_INFO(msg) spdlog::info("{}:{} {}", __FILE__, __LINE__, std::string(msg));
#define VK_CHECKERROR(X, error) \
    if((X) != VK_SUCCESS) \
    { \
        throw std::runtime_error(std::string("Vulkan Error") + std::string((error))); \
    }


template <class T> T* Temp(T&& t) { return &t; }


struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
};

class VulkanTutorialApplication
{
public:
    void Run();
private:
    const uint32_t WIDTH = 1080;
    const uint32_t HEIGHT = 720;
    std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
    bool m_enableValidationLayers = true;
    GLFWwindow* m_window;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue graphicsQueue;
    
    void InitWindow();
    void InitVulkan();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void SetupDebugMessenger();
    void CreateInstance();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    QueueFamilyIndices FindQueueFamilies();
    void MainLoop();
    void Cleanup();
    void DestroyDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
};