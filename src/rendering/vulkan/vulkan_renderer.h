#pragma once

#include "../rendering/renderer.h"
#include "vulkan_device.h"
#include "vulkan_swap_chain.h"
#include <cassert>
#include <memory>
#include <vector>

namespace Avarice {
class VulkanRenderer{
 public:
  VulkanRenderer(VulkanDevice &device);
  ~VulkanRenderer();

  VulkanRenderer(const VulkanRenderer &) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
  float getAspectRatio() const { return swapChain->extentAspectRatio(); }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer BeginFrame();
  void EndFrame();
  void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

 private:
  void CreateCommandBuffers();
  void FreeCommandBuffers();
  void RecreateSwapChain();

  VulkanDevice &device;
  std::unique_ptr<VulkanSwapChain> swapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{0};
  bool isFrameStarted{false};
};
}