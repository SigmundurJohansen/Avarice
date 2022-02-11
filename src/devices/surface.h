#pragma once

//#include <volk.h>
#include <vulkan/vulkan.h>

namespace Avarice
{
	class Instance;
	class PhysicalDevice;

	class Surface
	{
		friend class Graphics;

	public:
		Surface(const Instance *_instance, const PhysicalDevice *_physicalDevice);
		~Surface();

		operator const VkSurfaceKHR &() const { return m_surface; }

		const VkSurfaceKHR &GetSurface() const { return m_surface; }
		const VkSurfaceCapabilitiesKHR &GetCapabilities() const { return m_capabilities; }
		const VkSurfaceFormatKHR &GetFormat() const { return m_format; }

	private:
		const Instance *m_instance;
		const PhysicalDevice *m_physicalDevice;

		VkSurfaceKHR m_surface = VK_NULL_HANDLE;
		VkSurfaceCapabilitiesKHR m_capabilities = {};
		VkSurfaceFormatKHR m_format = {};
	};
}
