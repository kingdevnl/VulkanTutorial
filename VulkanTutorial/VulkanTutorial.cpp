#include "VulkanTutorial.hpp"

#pragma comment(lib, "vulkan-1.lib")


void VulkanTutorialApplication::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}


void VulkanTutorialApplication::InitWindow()
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Vulkan", 0, 0);
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_window, (mode->width - WIDTH) / 2, (mode->height - HEIGHT) / 2);
}

bool VulkanTutorialApplication::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}
}

std::vector<const char*> VulkanTutorialApplication::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_enableValidationLayers)
	{
		spdlog::info("Adding VK_EXT_DEBUG_UTILS_EXTENSION_NAME");
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanTutorialApplication::SetupDebugMessenger()
{
	if (!m_enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
	if (!func)
	{
		throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");
	}


	VK_CHECKERROR(func(m_instance, &createInfo, nullptr, &m_debugMessenger), "Failed to set up debug messenger")
	spdlog::info("Debug Messenger created");
}


void VulkanTutorialApplication::InitVulkan()
{
	if (m_enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}
	CreateInstance();
	SetupDebugMessenger();
	PickPhysicalDevice();
	CreateLogicalDevice();
}


void VulkanTutorialApplication::CreateInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "None";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
	createInfo.ppEnabledLayerNames = m_validationLayers.data();
	VK_CHECKERROR(vkCreateInstance(&createInfo, nullptr, &m_instance), "Failed to create instance")

	spdlog::info("Vulkan Instance created with layers: [{}] and extensions: [{}]",
	             fmt::join(m_validationLayers, ", "),
	             fmt::join(extensions, ", "));
}

void VulkanTutorialApplication::PickPhysicalDevice()
{
	this->m_physicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPU's with VK support");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (int i = 0; i < deviceCount; i++)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader)
		{
			this->m_physicalDevice = devices[i];
			break;
		}
	}

	if (this->m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("No acceptable GPU found.");
	}
	spdlog::info("Found gpu.");
}

void VulkanTutorialApplication::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies();
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = Temp(1.0f);

	VkPhysicalDeviceFeatures deviceFeatures{};


	VkDeviceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;
	if (m_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(this->m_validationLayers.size());
		createInfo.ppEnabledLayerNames = this->m_validationLayers.data();
	}

	VK_CHECKERROR(vkCreateDevice(this->m_physicalDevice, &createInfo, nullptr, &this->m_device),
	              "Failed to create logical device")


	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(this->m_physicalDevice, &physicalDeviceProperties);


	vkGetDeviceQueue(this->m_device, indices.graphicsFamily,0, &this->graphicsQueue);


	spdlog::info("Created logical device. {}", physicalDeviceProperties.deviceName);
}

QueueFamilyIndices VulkanTutorialApplication::FindQueueFamilies()
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->m_physicalDevice, &queueFamilyCount, 0);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(this->m_physicalDevice, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
			spdlog::info("Found VK_QUEUE_GRAPHICS_BIT {}", i);
			break;
		}
	}


	return indices;
}

void VulkanTutorialApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void VulkanTutorialApplication::Cleanup()
{
	DestroyDebugMessenger();
	vkDestroyDevice(this->m_device, nullptr);

	vkDestroyInstance(m_instance, nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void VulkanTutorialApplication::DestroyDebugMessenger()
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		m_instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (!func)
	{
		throw std::runtime_error("Failed to load vkDestroyDebugUtilsMessengerEXT");
	}

	func(m_instance, m_debugMessenger, nullptr);
	spdlog::info("Debug Messenger destroyed");
}

VkBool32 VulkanTutorialApplication::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                  void* pUserData)
{
	if (messageSeverity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) return VK_FALSE;
	spdlog::critical("Validation layer: {}", pCallbackData->pMessage);
	throw std::runtime_error("Validation layer: " + std::string(pCallbackData->pMessage));
}


int main(int argc, char* argv[])
{
	VulkanTutorialApplication app;

	try
	{
		app.Run();
	}
	catch (std::exception& e)
	{
		spdlog::critical("ERROR: {}", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
