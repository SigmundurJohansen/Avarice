#include "model.h"
#include <cassert>

namespace Avarice{

Model::Model(const Model::Builder &builder)
{
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
}

Model::~Model() {}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->GetBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}
}
