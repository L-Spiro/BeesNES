/**
 * Create a new device instance.
 * 
 * \param _physicalDevice Must be one of the device handles returned from a call to vkEnumeratePhysicalDevices.
 * \param _pCreateInfo A pointer to a VkDeviceCreateInfo structure containing information about how to create the device.
 * \param _pAllocator Controls host memory allocation.
 * \param _pDevice A pointer to a handle in which the created VkDevice is returned.
 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_FEATURE_NOT_PRESENT, VK_ERROR_TOO_MANY_OBJECTS, VK_ERROR_DEVICE_LOST.
 **/
static PFN_vkCreateDevice								m_pfCreateDevice;

/**
 * Destroy a logical device.
 * 
 * \param _device The logical device to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyDevice								m_pfDestroyDevice;

/**
 * Create a new Vulkan instance.
 * 
 * \param _pCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
 * \param _pAllocator Controls host memory allocation.
 * \param _pInstance Points a VkInstance handle in which the resulting instance is returned.
 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_LAYER_NOT_PRESENT, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_INCOMPATIBLE_DRIVER
 **/
static PFN_vkCreateInstance								m_pfCreateInstance;

/**
 * Destroy an instance of Vulkan.
 * 
 * \param _instance The handle of the instance to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyInstance							m_pfDestroyInstance;

/**
 * Get a queue handle from a device.
 * 
 * \param _device The logical device that owns the queue.
 * \param _queueFamilyIndex The index of the queue family to which the queue belongs.
 * \param _queueIndex The index within this queue family of the queue to retrieve.
 * \param _pQueue A pointer to a VkQueue object that will be filled with the handle for the requested queue.
 **/
static PFN_vkGetDeviceQueue								m_pfGetDeviceQueue;

/**
 * Enumerates the physical devices accessible to a Vulkan instance.
 * 
 * \param _instance A handle to a Vulkan instance previously created with vkCreateInstance.
 * \param _pPhysicalDeviceCount A pointer to an integer related to the number of physical devices available or queried.
 * \param _pPhysicalDevices Either NULL or a pointer to an array of VkPhysicalDevice handles.
 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED.
 **/
static PFN_vkEnumeratePhysicalDevices					m_pfEnumeratePhysicalDevices;

/**
 * Returns properties of a physical device.
 * 
 * \param _physicalDevice The handle to the physical device whose properties will be queried.
 * \param _pProperties A pointer to a VkPhysicalDeviceProperties structure in which properties are returned.
 **/
static PFN_vkGetPhysicalDeviceProperties				m_pfGetPhysicalDeviceProperties;

#ifdef LSN_WINDOWS
/**
 * Create a VkSurfaceKHR object for an Win32 native window.
 * 
 * \param _instance The instance to associate the surface with.
 * \param _pCreateInfo A pointer to a VkWin32SurfaceCreateInfoKHR structure containing parameters affecting the creation of the surface object.
 * \param _pAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
 * \param _pSurface A pointer to a VkSurfaceKHR handle in which the created surface object is returned.
 * \return On success, this command returns: VK_SUCCESS.  On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY
 **/
static PFN_vkCreateWin32SurfaceKHR						m_pfCreateWin32SurfaceKHR;

/**
 * Destroy a VkSurfaceKHR object.
 * 
 * \param _instance The instance used to create the surface.
 * \param _surface The surface to destroy.
 * \param _pAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
 **/
static PFN_vkDestroySurfaceKHR							m_pfDestroySurfaceKHR;

/**
 * Create a swapchain.
 * 
 * \param _device The device to create the swapchain for.
 * \param _pCreateInfo A pointer to a VkSwapchainCreateInfoKHR structure specifying the parameters of the created swapchain.
 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
 * \param _pSwapchain A pointer to a VkSwapchainKHR handle in which the created swapchain object will be returned.
 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_DEVICE_LOST, VK_ERROR_SURFACE_LOST_KHR, VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_COMPRESSION_EXHAUSTED_EXT.
 **/
static PFN_vkCreateSwapchainKHR							m_pfCreateSwapchainKHR;

/**
 * Destroy a swapchain object.
 * 
 * \param _device The VkDevice associated with swapchain.
 * \param _swapchain The swapchain to destroy.
 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
 **/
static PFN_vkDestroySwapchainKHR						m_pfDestroySwapchainKHR;

/**
 * Obtain the array of presentable images associated with a swapchain.
 **/
static PFN_vkGetSwapchainImagesKHR						m_pfGetSwapchainImagesKHR;

/**
 * Query color formats supported by surface.
 * 
 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
 * \param _surface The surface that will be associated with the swapchain.
 * \param _pSurfaceFormatCount A pointer to an integer related to the number of format pairs available or queried.
 * \param _pSurfaceFormats Either NULL or a pointer to an array of VkSurfaceFormatKHR structures.
 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
 **/
static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR			m_pfGetPhysicalDeviceSurfaceFormatsKHR;

/**
 * Query supported presentation modes.
 * 
 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
 * \param _surface The surface that will be associated with the swapchain.
 * \param _pPresentModeCount A pointer to an integer related to the number of presentation modes available or queried, as described below.
 * \param _pPresentModes Either NULL or a pointer to an array of VkPresentModeKHR values, indicating the supported presentation modes
 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
 **/
static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	m_pfGetPhysicalDeviceSurfacePresentModesKHR;

/**
 * Query surface capabilities.
 * 
 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
 * \param _surface The surface that will be associated with the swapchain.
 * \param _pSurfaceCapabilities A pointer to a VkSurfaceCapabilitiesKHR structure in which the capabilities are returned.
 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
 **/
static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR;
#endif	// #ifdef LSN_WINDOWS

/**
 * Create a new image object.
 * 
 * \param _device The the logical device that creates the image.
 * \param _pCreateInfo A pointer to a VkImageCreateInfo structure containing parameters to be used to create the image.
 * \param _pAllocator Controls host memory allocation as described in the Memory Allocation chapter.
 * \param _pImage A pointer to a VkImage handle in which the resulting image object is returned.
 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_LAYER_NOT_PRESENT, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_INCOMPATIBLE_DRIVER
 **/
static PFN_vkCreateImage								m_pfCreateImage;

/**
 * Destroy an image object.
 *
 * \param _device The logical device that destroys the image.
 * \param _image The image to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyImage								m_pfDestroyImage;

/**
 * Retrieve the index of the next available presentable image.
 * 
 * \param _device The logical device associated with the swapchain.
 * \param _swapchain The non-retired swapchain from which an image is being acquired.
 * \param _timeout Specifies how long the function waits, in nanoseconds, if no image is available.
 * \param _semaphore VK_NULL_HANDLE or a semaphore to signal.
 * \param _fence VK_NULL_HANDLE or a fence to signal.
 * \param _pImageIndex A pointer to a uint32_t in which the index of the next image is returned.
 * \return Returns VK_SUCCESS on success, or an appropriate error code (like VK_SUBOPTIMAL_KHR or VK_ERROR_OUT_OF_DATE_KHR) on failure.
 **/
static PFN_vkAcquireNextImageKHR                           m_pfAcquireNextImageKHR;

/**
 * Returns the memory requirements for specified Vulkan object.
 *
 * \param _device The logical device that owns the image.
 * \param _image The image to query.
 * \param _pMemoryRequirements A pointer to a VkMemoryRequirements structure in which the memory requirements of the image object are returned.
 **/
static PFN_vkGetImageMemoryRequirements					m_pfGetImageMemoryRequirements;

/**
 * Allocate device memory.
 *
 * \param _device The logical device that owns the memory.
 * \param _pAllocateInfo A pointer to a VkMemoryAllocateInfo structure describing parameters of the allocation. A successfully returned allocation must use the requested parameters-no substitution is permitted by the implementation.
 * \param _pAllocator Controls host memory allocation.
 * \param _pMemory A pointer to a VkDeviceMemory handle in which information about the allocated memory is returned.
 **/
static PFN_vkAllocateMemory								m_pfAllocateMemory;

/**
 * Free device memory.
 *
 * \param _device The logical device that owns the memory.
 * \param _memory The VkDeviceMemory object to be freed.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkFreeMemory									m_pfFreeMemory;

/**
 * Returns up to requested number of global extension properties.
 * 
 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried.
 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
 **/
static PFN_vkEnumerateInstanceExtensionProperties		m_pfEnumerateInstanceExtensionProperties;

/**
 * Returns properties of available physical device extensions.
 * 
 * \param _physicalDevice The physical device that will be queried.
 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried, and is treated in the same fashion as the vkEnumerateInstanceExtensionProperties::pPropertyCount parameter.
 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
 **/
static PFN_vkEnumerateDeviceExtensionProperties			m_pfEnumerateDeviceExtensionProperties;

/**
 * Allocate command buffers from a command pool.
 * 
 * \param _device The logical device that owns the command pool.
 * \param _pAllocateInfo A pointer to a VkCommandBufferAllocateInfo structure specifying the command pool and number of buffers.
 * \param _pCommandBuffers A pointer to an array of VkCommandBuffer handles in which the resulting command buffer objects are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkAllocateCommandBuffers                        m_pfAllocateCommandBuffers;

/**
 * Free command buffers.
 * 
 * \param _device The logical device that owns the command pool.
 * \param _commandPool The command pool from which the command buffers were allocated.
 * \param _commandBufferCount The number of command buffers to free.
 * \param _pCommandBuffers An array of VkCommandBuffer handles to be freed.
 **/
static PFN_vkFreeCommandBuffers                            m_pfFreeCommandBuffers;

/**
 * Begin recording commands to a command buffer.
 * 
 * \param _commandBuffer The command buffer to start recording.
 * \param _pBeginInfo A pointer to a VkCommandBufferBeginInfo structure specifying the parameters for beginning command buffer recording.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkBeginCommandBuffer                            m_pfBeginCommandBuffer;

/**
 * End recording commands to a command buffer.
 * 
 * \param _commandBuffer The command buffer to end recording.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkEndCommandBuffer                              m_pfEndCommandBuffer;

/**
 * Reset a command buffer.
 * 
 * \param _commandBuffer The command buffer to reset.
 * \param _flags Flags specifying additional parameters.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkResetCommandBuffer                            m_pfResetCommandBuffer;

/**
 * Create a command pool for allocating command buffers.
 * 
 * \param _device The logical device that owns the command pool.
 * \param _pCreateInfo A pointer to a VkCommandPoolCreateInfo structure specifying the parameters for creating the command pool.
 * \param _pAllocator Controls host memory allocation.
 * \param _pCommandPool A pointer to a VkCommandPool handle in which the resulting command pool is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateCommandPool                             m_pfCreateCommandPool;

/**
 * Destroy a command pool.
 * 
 * \param _device The logical device that owns the command pool.
 * \param _commandPool The command pool to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyCommandPool                            m_pfDestroyCommandPool;

/**
 * Reset a command pool.
 * 
 * \param _device The logical device that owns the command pool.
 * \param _commandPool The command pool to reset.
 * \param _flags Flags specifying additional parameters.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkResetCommandPool                              m_pfResetCommandPool;

/**
 * Create a new query pool object.
 * 
 * \param _device The logical device that creates the query pool.
 * \param _pCreateInfo A pointer to a VkQueryPoolCreateInfo structure containing parameters to be used to create the pool.
 * \param _pAllocator Controls host memory allocation.
 * \param _pQueryPool A pointer to a VkQueryPool handle in which the resulting query pool object is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateQueryPool                               m_pfCreateQueryPool;

/**
 * Destroy a query pool object.
 *
 * \param _device The logical device that destroys the query pool.
 * \param _queryPool The query pool to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyQueryPool                              m_pfDestroyQueryPool;

/**
 * Copy results of queries in a query pool to a host memory buffer.
 *
 * \param _device The logical device that owns the query pool.
 * \param _queryPool The query pool managing the queries containing the desired results.
 * \param _firstQuery The initial query index.
 * \param _queryCount The number of queries to read.
 * \param _dataSize The size in bytes of the buffer pointed to by _pData.
 * \param _pData A pointer to a user-allocated buffer where the results will be written.
 * \param _stride The stride in bytes between results for individual queries within _pData.
 * \param _flags A bitmask of VkQueryResultFlagBits specifying how and when results are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkGetQueryPoolResults                           m_pfGetQueryPoolResults;

/**
 * Create a fence object.
 * 
 * \param _device The logical device that owns the fence.
 * \param _pCreateInfo A pointer to a VkFenceCreateInfo structure specifying the parameters for creating the fence.
 * \param _pAllocator Controls host memory allocation.
 * \param _pFence A pointer to a VkFence handle in which the resulting fence is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateFence                                   m_pfCreateFence;

/**
 * Destroy a fence object.
 * 
 * \param _device The logical device that owns the fence.
 * \param _fence The fence to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyFence                                  m_pfDestroyFence;

/**
 * Wait for one or more fence objects to become signaled.
 * 
 * \param _device The logical device that owns the fences.
 * \param _fenceCount The number of fences to wait for.
 * \param _pFences An array of VkFence handles.
 * \param _waitAll If true, waits for all fences; otherwise, waits for any fence.
 * \param _timeout The timeout period in nanoseconds.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkWaitForFences                                 m_pfWaitForFences;

/**
 * Reset one or more fence objects.
 * 
 * \param _device The logical device that owns the fences.
 * \param _fenceCount The number of fences to reset.
 * \param _pFences An array of VkFence handles.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkResetFences                                   m_pfResetFences;

/**
 * Create a semaphore object.
 * 
 * \param _device The logical device that owns the semaphore.
 * \param _pCreateInfo A pointer to a VkSemaphoreCreateInfo structure specifying the parameters for creating the semaphore.
 * \param _pAllocator Controls host memory allocation.
 * \param _pSemaphore A pointer to a VkSemaphore handle in which the resulting semaphore is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateSemaphore                               m_pfCreateSemaphore;

/**
 * Destroy a semaphore object.
 * 
 * \param _device The logical device that owns the semaphore.
 * \param _semaphore The semaphore to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroySemaphore                              m_pfDestroySemaphore;

/**
 * Create a buffer object.
 * 
 * \param _device The logical device that owns the buffer.
 * \param _pCreateInfo A pointer to a VkBufferCreateInfo structure specifying the parameters for creating the buffer.
 * \param _pAllocator Controls host memory allocation.
 * \param _pBuffer A pointer to a VkBuffer handle in which the resulting buffer is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateBuffer                                  m_pfCreateBuffer;

/**
 * Destroy a buffer object.
 * 
 * \param _device The logical device that owns the buffer.
 * \param _buffer The buffer to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyBuffer                                 m_pfDestroyBuffer;

/**
 * Retrieve the memory requirements for a buffer.
 * 
 * \param _device The logical device that owns the buffer.
 * \param _buffer The buffer to query.
 * \param _pMemoryRequirements A pointer to a VkMemoryRequirements structure in which the memory requirements are returned.
 **/
static PFN_vkGetBufferMemoryRequirements                   m_pfGetBufferMemoryRequirements;

/**
 * Bind device memory to a buffer.
 * 
 * \param _device The logical device that owns the buffer.
 * \param _buffer The buffer to bind.
 * \param _memory The device memory to bind to the buffer.
 * \param _memoryOffset The start offset within the device memory to bind.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkBindBufferMemory                              m_pfBindBufferMemory;

/**
 * Bind device memory to an image.
 * 
 * \param _device The logical device that owns the image.
 * \param _image The image to bind.
 * \param _memory The device memory to bind to the image.
 * \param _memoryOffset The start offset within the device memory to bind.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkBindImageMemory                               m_pfBindImageMemory;

/**
 * Create an image view object.
 * 
 * \param _device The logical device that owns the image view.
 * \param _pCreateInfo A pointer to a VkImageViewCreateInfo structure specifying the parameters for creating the image view.
 * \param _pAllocator Controls host memory allocation.
 * \param _pImageView A pointer to a VkImageView handle in which the resulting image view is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateImageView                               m_pfCreateImageView;

/**
 * Destroy an image view object.
 * 
 * \param _device The logical device that owns the image view.
 * \param _imageView The image view to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyImageView                              m_pfDestroyImageView;

/**
 * Create a new sampler object.
 * 
 * \param _device The logical device that creates the sampler.
 * \param _pCreateInfo A pointer to a VkSamplerCreateInfo structure specifying the state of the sampler object.
 * \param _pAllocator Controls host memory allocation.
 * \param _pSampler A pointer to a VkSampler handle in which the resulting sampler object is returned.
 **/
static PFN_vkCreateSampler                                 m_pfCreateSampler;

/**
 * Destroy a sampler object.
 * 
 * \param _device The logical device that destroys the sampler.
 * \param _sampler The sampler to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroySampler                                m_pfDestroySampler;

/**
 * Create a render pass object.
 * 
 * \param _device The logical device that owns the render pass.
 * \param _pCreateInfo A pointer to a VkRenderPassCreateInfo structure specifying the parameters for creating the render pass.
 * \param _pAllocator Controls host memory allocation.
 * \param _pRenderPass A pointer to a VkRenderPass handle in which the resulting render pass is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateRenderPass                              m_pfCreateRenderPass;

/**
 * Destroy a render pass object.
 * 
 * \param _device The logical device that owns the render pass.
 * \param _renderPass The render pass to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyRenderPass                             m_pfDestroyRenderPass;

/**
 * Create a framebuffer object.
 * 
 * \param _device The logical device that owns the framebuffer.
 * \param _pCreateInfo A pointer to a VkFramebufferCreateInfo structure specifying the parameters for creating the framebuffer.
 * \param _pAllocator Controls host memory allocation.
 * \param _pFramebuffer A pointer to a VkFramebuffer handle in which the resulting framebuffer is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateFramebuffer                             m_pfCreateFramebuffer;

/**
 * Destroy a framebuffer object.
 * 
 * \param _device The logical device that owns the framebuffer.
 * \param _framebuffer The framebuffer to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyFramebuffer                            m_pfDestroyFramebuffer;

/**
 * Begin a new render pass.
 * 
 * \param _commandBuffer The command buffer in which to record the command.
 * \param _pRenderPassBegin A pointer to a VkRenderPassBeginInfo structure.
 * \param _contents Specifies how the commands within the first subpass will be provided.
 **/
static PFN_vkCmdBeginRenderPass                            m_pfCmdBeginRenderPass;

/**
 * End the current render pass.
 * 
 * \param _commandBuffer The command buffer in which to record the command.
 **/
static PFN_vkCmdEndRenderPass                              m_pfCmdEndRenderPass;

/**
 * Update the contents of a descriptor set object.
 * 
 * \param _device The logical device that updates the descriptor sets.
 * \param _descriptorWriteCount The number of elements in the _pDescriptorWrites array.
 * \param _pDescriptorWrites A pointer to an array of VkWriteDescriptorSet structures.
 * \param _descriptorCopyCount The number of elements in the _pDescriptorCopies array.
 * \param _pDescriptorCopies A pointer to an array of VkCopyDescriptorSet structures.
 **/
static PFN_vkUpdateDescriptorSets                          m_pfUpdateDescriptorSets;

/**
 * Create graphics pipelines.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipelineCache A pipeline cache to store the pipeline.
 * \param _createInfoCount The number of pipelines to create.
 * \param _pCreateInfos An array of VkGraphicsPipelineCreateInfo structures specifying the parameters for creating the pipelines.
 * \param _pAllocator Controls host memory allocation.
 * \param _pPipelines A pointer to an array of VkPipeline handles in which the resulting pipelines are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateGraphicsPipelines                       m_pfCreateGraphicsPipelines;

/**
 * Create compute pipelines.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipelineCache A pipeline cache to store the pipeline.
 * \param _createInfoCount The number of pipelines to create.
 * \param _pCreateInfos An array of VkComputePipelineCreateInfo structures specifying the parameters for creating the pipelines.
 * \param _pAllocator Controls host memory allocation.
 * \param _pPipelines A pointer to an array of VkPipeline handles in which the resulting pipelines are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateComputePipelines                        m_pfCreateComputePipelines;

/**
 * Destroy a pipeline object.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipeline The pipeline to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyPipeline                               m_pfDestroyPipeline;

/**
 * Create a pipeline layout object.
 * 
 * \param _device The logical device that owns the pipeline layout.
 * \param _pCreateInfo A pointer to a VkPipelineLayoutCreateInfo structure specifying the parameters for creating the pipeline layout.
 * \param _pAllocator Controls host memory allocation.
 * \param _pPipelineLayout A pointer to a VkPipelineLayout handle in which the resulting pipeline layout is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreatePipelineLayout                          m_pfCreatePipelineLayout;

/**
 * Destroy a pipeline layout object.
 * 
 * \param _device The logical device that owns the pipeline layout.
 * \param _pipelineLayout The pipeline layout to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyPipelineLayout                         m_pfDestroyPipelineLayout;

/**
 * Create a shader module object.
 * 
 * \param _device The logical device that owns the shader module.
 * \param _pCreateInfo A pointer to a VkShaderModuleCreateInfo structure specifying the parameters for creating the shader module.
 * \param _pAllocator Controls host memory allocation.
 * \param _pShaderModule A pointer to a VkShaderModule handle in which the resulting shader module is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateShaderModule                            m_pfCreateShaderModule;

/**
 * Destroy a shader module object.
 * 
 * \param _device The logical device that owns the shader module.
 * \param _shaderModule The shader module to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyShaderModule                           m_pfDestroyShaderModule;

/**
 * Bind a pipeline object to a command buffer.
 * 
 * \param _commandBuffer The command buffer that the pipeline will be bound to.
 * \param _pipelineBindPoint Specifies whether to bind to the compute or graphics pipeline.
 * \param _pipeline The pipeline to be bound.
 **/
static PFN_vkCmdBindPipeline                               m_pfCmdBindPipeline;

/**
 * Binds descriptor sets to a command buffer.
 * 
 * \param _commandBuffer The command buffer that the descriptor sets will be bound to.
 * \param _pipelineBindPoint Indicates the type of the pipeline that will use the descriptors.
 * \param _layout The pipeline layout that the descriptor sets are compatible with.
 * \param _firstSet The set number of the first descriptor set to be bound.
 * \param _descriptorSetCount The number of elements in the _pDescriptorSets array.
 * \param _pDescriptorSets A pointer to an array of handles to descriptor sets to bind.
 * \param _dynamicOffsetCount The number of dynamic offsets in the _pDynamicOffsets array.
 * \param _pDynamicOffsets A pointer to an array of uint32_t values specifying dynamic offsets.
 **/
static PFN_vkCmdBindDescriptorSets                         m_pfCmdBindDescriptorSets;

/**
 * Bind vertex buffers to a command buffer.
 * 
 * \param _commandBuffer The command buffer into which the command is recorded.
 * \param _firstBinding The index of the first vertex input binding whose state is updated.
 * \param _bindingCount The number of vertex input bindings whose state is updated.
 * \param _pBuffers A pointer to an array of buffer handles.
 * \param _pOffsets A pointer to an array of buffer offsets.
 **/
static PFN_vkCmdBindVertexBuffers                          m_pfCmdBindVertexBuffers;

/**
 * Draw primitives.
 * 
 * \param _commandBuffer The command buffer into which the command is recorded.
 * \param _vertexCount The number of vertices to draw.
 * \param _instanceCount The number of instances to draw.
 * \param _firstVertex The index of the first vertex to draw.
 * \param _firstInstance The instance ID of the first instance to draw.
 **/
static PFN_vkCmdDraw                                       m_pfCmdDraw;

/**
 * Set the viewport dynamically for a command buffer.
 * 
 * \param _commandBuffer The command buffer into which the command is recorded.
 * \param _firstViewport The index of the first viewport whose parameters are updated.
 * \param _viewportCount The number of viewports whose parameters are updated.
 * \param _pViewports A pointer to an array of VkViewport structures specifying viewport parameters.
 **/
static PFN_vkCmdSetViewport                                m_pfCmdSetViewport;

/**
 * Set the dynamic scissor rectangles for a command buffer.
 * 
 * \param _commandBuffer The command buffer into which the command is recorded.
 * \param _firstScissor The index of the first scissor whose parameters are updated.
 * \param _scissorCount The number of scissors whose parameters are updated.
 * \param _pScissors A pointer to an array of VkRect2D structures specifying scissor rectangles.
 **/
static PFN_vkCmdSetScissor                                 m_pfCmdSetScissor;

/**
 * Update the values of push constants.
 * 
 * \param _commandBuffer The command buffer in which the push constant update will be recorded.
 * \param _layout The pipeline layout used to program the push constant updates.
 * \param _stageFlags A bitmask specifying the shader stages that will use the push constants.
 * \param _offset The start offset of the push constant range to update, in bytes.
 * \param _size The size of the push constant range to update, in bytes.
 * \param _pValues A pointer to an array of bytes containing the new push constant values.
 **/
static PFN_vkCmdPushConstants                              m_pfCmdPushConstants;

/**
 * Create a descriptor set layout object.
 * 
 * \param _device The logical device that owns the descriptor set layout.
 * \param _pCreateInfo A pointer to a VkDescriptorSetLayoutCreateInfo structure specifying the parameters for creating the descriptor set layout.
 * \param _pAllocator Controls host memory allocation.
 * \param _pSetLayout A pointer to a VkDescriptorSetLayout handle in which the resulting descriptor set layout is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateDescriptorSetLayout                     m_pfCreateDescriptorSetLayout;

/**
 * Destroy a descriptor set layout object.
 * 
 * \param _device The logical device that owns the descriptor set layout.
 * \param _descriptorSetLayout The descriptor set layout to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyDescriptorSetLayout                    m_pfDestroyDescriptorSetLayout;

/**
 * Create a descriptor pool object.
 * 
 * \param _device The logical device that owns the descriptor pool.
 * \param _pCreateInfo A pointer to a VkDescriptorPoolCreateInfo structure specifying the parameters for creating the descriptor pool.
 * \param _pAllocator Controls host memory allocation.
 * \param _pDescriptorPool A pointer to a VkDescriptorPool handle in which the resulting descriptor pool is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateDescriptorPool                          m_pfCreateDescriptorPool;

/**
 * Destroy a descriptor pool object.
 * 
 * \param _device The logical device that owns the descriptor pool.
 * \param _descriptorPool The descriptor pool to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyDescriptorPool                         m_pfDestroyDescriptorPool;

/**
 * Reset a descriptor pool object.
 * 
 * \param _device The logical device that owns the descriptor pool.
 * \param _descriptorPool The descriptor pool to reset.
 * \param _flags Flags specifying additional parameters.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkResetDescriptorPool                           m_pfResetDescriptorPool;

/**
 * Allocate descriptor sets from a descriptor pool.
 * 
 * \param _device The logical device that owns the descriptor pool.
 * \param _pAllocateInfo A pointer to a VkDescriptorSetAllocateInfo structure specifying the descriptor pool and layout.
 * \param _pDescriptorSets A pointer to an array of VkDescriptorSet handles in which the resulting descriptor sets are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkAllocateDescriptorSets                        m_pfAllocateDescriptorSets;

/**
 * Free descriptor sets back to a descriptor pool.
 * 
 * \param _device The logical device that owns the descriptor pool.
 * \param _descriptorPool The descriptor pool that the descriptor sets were allocated from.
 * \param _descriptorSetCount The number of descriptor sets to free.
 * \param _pDescriptorSets A pointer to an array of VkDescriptorSet handles specifying the descriptor sets to free.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkFreeDescriptorSets                            m_pfFreeDescriptorSets;

/**
 * Submit a queue for execution.
 * 
 * \param _queue The queue to submit the command buffers to.
 * \param _submitCount The number of VkSubmitInfo structures to submit.
 * \param _pSubmits An array of VkSubmitInfo structures specifying the command buffers to submit.
 * \param _fence A fence that will be signaled once the command buffers have completed execution.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkQueueSubmit                                   m_pfQueueSubmit;

#ifdef LSN_WINDOWS
/**
 * Present a swapchain image to the display.
 * 
 * \param _queue The queue to present the image on.
 * \param _pPresentInfo A pointer to a VkPresentInfoKHR structure specifying the parameters of the presentation.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkQueuePresentKHR                               m_pfQueuePresentKHR;
#endif	// #ifdef LSN_WINDOWS

/**
 * Wait for a queue to become idle.
 * 
 * \param _queue The queue to wait for.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkQueueWaitIdle                                 m_pfQueueWaitIdle;

/**
 * Wait for all device operations to complete.
 * 
 * \param _device The logical device to wait for.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkDeviceWaitIdle                                m_pfDeviceWaitIdle;

/**
 * Map a memory object into application address space.
 * 
 * \param _device The logical device that owns the memory.
 * \param _memory The memory object to map.
 * \param _offset The start offset within the memory object to map.
 * \param _size The size of the memory range to map, or VK_WHOLE_SIZE to map the entire range.
 * \param _flags Flags controlling the behavior of the mapping.
 * \param _ppData A pointer to a pointer in which the host-accessible pointer to the mapped memory is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkMapMemory                                     m_pfMapMemory;

/**
 * Unmap a memory object previously mapped into application address space.
 * 
 * \param _device The logical device that owns the memory.
 * \param _memory The memory object to unmap.
 **/
static PFN_vkUnmapMemory                                   m_pfUnmapMemory;

/**
 * Flush ranges of mapped memory to ensure host writes are visible to the device.
 * 
 * \param _device The logical device that owns the memory.
 * \param _memoryRangeCount The number of memory ranges to flush.
 * \param _pMemoryRanges A pointer to an array of VkMappedMemoryRange structures specifying the memory ranges to flush.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkFlushMappedMemoryRanges                       m_pfFlushMappedMemoryRanges;

/**
 * Invalidate ranges of mapped memory to ensure device writes are visible to the host.
 * 
 * \param _device The logical device that owns the memory.
 * \param _memoryRangeCount The number of memory ranges to invalidate.
 * \param _pMemoryRanges A pointer to an array of VkMappedMemoryRange structures specifying the memory ranges to invalidate.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkInvalidateMappedMemoryRanges                  m_pfInvalidateMappedMemoryRanges;

#ifdef LSN_WINDOWS
/**
 * Create a ray tracing pipeline object.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _deferredOperation An optional deferred operation.
 * \param _pipelineCache The pipeline cache to use.
 * \param _createInfoCount The number of ray tracing pipelines to create.
 * \param _pCreateInfos A pointer to an array of VkRayTracingPipelineCreateInfoKHR structures specifying the parameters for creating the pipelines.
 * \param _pAllocator Controls host memory allocation.
 * \param _pPipelines A pointer to an array of VkPipeline handles in which the resulting pipelines are returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateRayTracingPipelinesKHR                  m_pfCreateRayTracingPipelinesKHR;

/**
 * Get ray tracing shader group handles.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipeline The ray tracing pipeline.
 * \param _firstGroup The index of the first shader group.
 * \param _groupCount The number of shader groups.
 * \param _dataSize The size of the output data buffer.
 * \param _pData A pointer to a buffer to receive the shader group handles.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkGetRayTracingShaderGroupHandlesKHR            m_pfGetRayTracingShaderGroupHandlesKHR;

/**
 * Get capture replay shader group handles.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipeline The ray tracing pipeline.
 * \param _firstGroup The index of the first shader group.
 * \param _groupCount The number of shader groups.
 * \param _dataSize The size of the output data buffer.
 * \param _pData A pointer to a buffer to receive the shader group handles.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR m_pfGetRayTracingCaptureReplayShaderGroupHandlesKHR;

/**
 * Create an acceleration structure object.
 * 
 * \param _device The logical device that owns the acceleration structure.
 * \param _pCreateInfo A pointer to a VkAccelerationStructureCreateInfoKHR structure specifying the parameters for creating the acceleration structure.
 * \param _pAllocator Controls host memory allocation.
 * \param _pAccelerationStructure A pointer to a VkAccelerationStructureKHR handle in which the resulting acceleration structure is returned.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCreateAccelerationStructureKHR                m_pfCreateAccelerationStructureKHR;

/**
 * Destroy an acceleration structure object.
 * 
 * \param _device The logical device that owns the acceleration structure.
 * \param _accelerationStructure The acceleration structure to destroy.
 * \param _pAllocator Controls host memory allocation.
 **/
static PFN_vkDestroyAccelerationStructureKHR               m_pfDestroyAccelerationStructureKHR;

/**
 * Get the device address of an acceleration structure.
 * 
 * \param _device The logical device that owns the acceleration structure.
 * \param _pInfo A pointer to a VkAccelerationStructureDeviceAddressInfoKHR structure specifying the acceleration structure to query.
 * \return The device address of the acceleration structure.
 **/
static PFN_vkGetAccelerationStructureDeviceAddressKHR      m_pfGetAccelerationStructureDeviceAddressKHR;

/**
 * Build acceleration structures.
 * 
 * \param _device The logical device that owns the acceleration structures.
 * \param _deferredOperation An optional deferred operation.
 * \param _infoCount The number of acceleration structures to build.
 * \param _pInfos A pointer to an array of VkAccelerationStructureBuildGeometryInfoKHR structures specifying the build operations.
 * \param _ppBuildRangeInfos A pointer to an array of VkAccelerationStructureBuildRangeInfoKHR pointers specifying the ranges for each geometry in the acceleration structures.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkBuildAccelerationStructuresKHR                m_pfBuildAccelerationStructuresKHR;

/**
 * Copy an acceleration structure.
 * 
 * \param _device The logical device that owns the acceleration structures.
 * \param _deferredOperation An optional deferred operation.
 * \param _pInfo A pointer to a VkCopyAccelerationStructureInfoKHR structure specifying the copy operation.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCopyAccelerationStructureKHR                  m_pfCopyAccelerationStructureKHR;

/**
 * Copy an acceleration structure to memory.
 * 
 * \param _device The logical device that owns the acceleration structures.
 * \param _deferredOperation An optional deferred operation.
 * \param _pInfo A pointer to a VkCopyAccelerationStructureToMemoryInfoKHR structure specifying the copy operation.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkCopyAccelerationStructureToMemoryKHR          m_pfCopyAccelerationStructureToMemoryKHR;

/**
 * Copy memory to an acceleration structure.
 * 
 * \param _commandBuffer The command buffer in which the copy command is recorded.
 * \param _pInfo A pointer to a VkCopyMemoryToAccelerationStructureInfoKHR structure specifying the copy operation.
 **/
static PFN_vkCmdCopyMemoryToAccelerationStructureKHR       m_pfCmdCopyMemoryToAccelerationStructureKHR;

/**
 * Write acceleration structures properties.
 * 
 * \param _device The logical device that owns the acceleration structures.
 * \param _accelerationStructureCount The number of acceleration structures to query.
 * \param _pAccelerationStructures A pointer to an array of VkAccelerationStructureKHR handles specifying the acceleration structures to query.
 * \param _queryType The type of query to perform.
 * \param _dataSize The size of the output data buffer.
 * \param _pData A pointer to a buffer to receive the query results.
 * \param _stride The stride in bytes between successive results.
 * \return VK_SUCCESS on success, or a negative error code on failure.
 **/
static PFN_vkWriteAccelerationStructuresPropertiesKHR      m_pfWriteAccelerationStructuresPropertiesKHR;

/**
 * Get the build sizes for an acceleration structure.
 * 
 * \param _device The logical device that owns the acceleration structures.
 * \param _buildType The type of build operation.
 * \param _pBuildInfo A pointer to a VkAccelerationStructureBuildGeometryInfoKHR structure specifying the build parameters.
 * \param _pMaxPrimitiveCounts A pointer to an array of maximum primitive counts for each geometry in the build operation.
 * \param _pSizeInfo A pointer to a VkAccelerationStructureBuildSizesInfoKHR structure in which the build sizes are returned.
 **/
static PFN_vkGetAccelerationStructureBuildSizesKHR         m_pfGetAccelerationStructureBuildSizesKHR;

/**
 * Trace rays in a ray tracing pipeline.
 * 
 * \param _commandBuffer The command buffer in which the trace command is recorded.
 * \param _pRaygenShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the ray generation shader binding table.
 * \param _pMissShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the miss shader binding table.
 * \param _pHitShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the hit shader binding table.
 * \param _pCallableShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the callable shader binding table.
 * \param _width The width of the ray trace operation.
 * \param _height The height of the ray trace operation.
 * \param _depth The depth of the ray trace operation.
 **/
static PFN_vkCmdTraceRaysKHR                               m_pfCmdTraceRaysKHR;

/**
 * Trace rays in a ray tracing pipeline using indirect commands.
 * 
 * \param _commandBuffer The command buffer in which the trace command is recorded.
 * \param _pRaygenShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the ray generation shader binding table.
 * \param _pMissShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the miss shader binding table.
 * \param _pHitShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the hit shader binding table.
 * \param _pCallableShaderBindingTable A pointer to a VkStridedDeviceAddressRegionKHR structure specifying the callable shader binding table.
 * \param _indirectDeviceAddress The device address of the indirect command buffer.
 **/
static PFN_vkCmdTraceRaysIndirectKHR                       m_pfCmdTraceRaysIndirectKHR;

/**
 * Get the stack size required for a ray tracing shader group.
 * 
 * \param _device The logical device that owns the pipeline.
 * \param _pipeline The ray tracing pipeline.
 * \param _group The index of the shader group.
 * \param _groupShader The specific shader within the group.
 * \return The stack size in bytes required for the shader group.
 **/
static PFN_vkGetRayTracingShaderGroupStackSizeKHR          m_pfGetRayTracingShaderGroupStackSizeKHR;
#endif	// #ifdef LSN_WINDOWS

/**
 * Returns properties of available physical device memory.
 * 
 * \param _physicalDevice The handle to the physical device whose properties will be queried.
 * \param _pMemoryProperties A pointer to a VkPhysicalDeviceMemoryProperties structure in which the properties are returned.
 **/
static PFN_vkGetPhysicalDeviceMemoryProperties             m_pfGetPhysicalDeviceMemoryProperties;

/**
 * Insert a memory dependency.
 * 
 * \param _commandBuffer The command buffer into which the command is recorded.
 * \param _srcStageMask Specifies the source stage mask.
 * \param _dstStageMask Specifies the destination stage mask.
 * \param _dependencyFlags A bitmask of VkDependencyFlagBits specifying how execution and memory dependencies are formed.
 * \param _memoryBarrierCount The number of elements in the _pMemoryBarriers array.
 * \param _pMemoryBarriers A pointer to an array of VkMemoryBarrier structures.
 * \param _bufferMemoryBarrierCount The number of elements in the _pBufferMemoryBarriers array.
 * \param _pBufferMemoryBarriers A pointer to an array of VkBufferMemoryBarrier structures.
 * \param _imageMemoryBarrierCount The number of elements in the _pImageMemoryBarriers array.
 * \param _pImageMemoryBarriers A pointer to an array of VkImageMemoryBarrier structures.
 **/
static PFN_vkCmdPipelineBarrier                            m_pfCmdPipelineBarrier;

/**
 * Copy data from a buffer into an image.
 * 
 * \param _commandBuffer The command buffer into which the command will be recorded.
 * \param _srcBuffer The source buffer.
 * \param _dstImage The destination image.
 * \param _dstImageLayout The layout of the destination image subresources for the copy.
 * \param _regionCount The number of regions to copy.
 * \param _pRegions A pointer to an array of VkBufferImageCopy structures specifying the regions to copy.
 **/
static PFN_vkCmdCopyBufferToImage                          m_pfCmdCopyBufferToImage;
