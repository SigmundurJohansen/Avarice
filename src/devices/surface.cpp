#include "surface.h"
#include "rendering/graphics.h"
#include "devices/instance.h"
#include "physical_device.h"

namespace Avarice
{
	Surface::Surface(const Instance *_instance, const PhysicalDevice *_physicalDevice) : m_instance(_instance),
																					   m_physicalDevice(_physicalDevice)
	{

		Graphics::CheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_physicalDevice, m_surface, &m_capabilities));

		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physicalDevice, m_surface, &surfaceFormatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*m_physicalDevice, m_surface, &surfaceFormatCount, surfaceFormats.data());

		if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			m_format.format = VK_FORMAT_B8G8R8A8_UNORM;
			m_format.colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// Iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;

			for (auto &surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_format.format = surfaceFormat.format;
					m_format.colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// In case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_format.format = surfaceFormats[0].format;
				m_format.colorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(*m_instance, m_surface, nullptr);
	}
}
