#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Avarice {
class Swapchain {
public:
	explicit Swapchain(const VkExtent2D &extent, const Swapchain *oldSwapchain = nullptr);	
	~Swapchain();
	VkResult AcquireNextImage(const VkSemaphore &presentCompleteSemaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);
	VkResult QueuePresent(const VkQueue &presentQueue, const VkSemaphore &waitSemaphore = VK_NULL_HANDLE);
	bool IsSameExtent(const VkExtent2D &extent2D) { return m_extent.width == extent2D.width && m_extent.height == extent2D.height; }
	operator const VkSwapchainKHR &() const { return m_swapchain; }
	const VkExtent2D &GetExtent() const { return m_extent; }
	uint32_t GetImageCount() const { return m_imageCount; }
	VkSurfaceTransformFlagsKHR GetPreTransform() const { return m_preTransform; }
	VkCompositeAlphaFlagBitsKHR GetCompositeAlpha() const { return m_compositeAlpha; }
	const std::vector<VkImage> &GetImages() const { return m_images; }
	const VkImage &GetActiveImage() const { return m_images[m_activeImageIndex]; }
	const std::vector<VkImageView> &GetImageViews() const { return m_imageViews; }
	const VkSwapchainKHR &GetSwapchain() const { return m_swapchain; }
	uint32_t GetActiveImageIndex() const { return m_activeImageIndex; }

private:
	VkExtent2D m_extent;
	VkPresentModeKHR m_presentMode;
	uint32_t m_imageCount = 0;
	VkSurfaceTransformFlagsKHR m_preTransform;
	VkCompositeAlphaFlagBitsKHR m_compositeAlpha;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkFence m_fenceImage = VK_NULL_HANDLE;
	uint32_t m_activeImageIndex;
};
}
