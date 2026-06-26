/** A Vulkan VkAccelerationStructureKHR wrapper. */
struct LSN_ACCELERATION_STRUCTURE {
    inline LSN_ACCELERATION_STRUCTURE() :
        asAccelerationStructure( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_ACCELERATION_STRUCTURE( VkDevice _dDevice, const VkAccelerationStructureCreateInfoKHR * _pascCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateAccelerationStructureKHR( _dDevice, _pascCreateInfo, _pacAllocator, &asAccelerationStructure ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_ACCELERATION_STRUCTURE( LSN_ACCELERATION_STRUCTURE &&_asOther ) noexcept : LSN_ACCELERATION_STRUCTURE() {
		(*this) = std::move( _asOther );
	}
    inline ~LSN_ACCELERATION_STRUCTURE() {
        Reset();
    }

    // == Operators.
    /**
     * Moves from _asOther to this object. _asOther is left in a valid state and could be reused.
     * 
     * \param _asOther The object to move.
     * \return Returns this object after the move.
     **/
    LSN_ACCELERATION_STRUCTURE &                        operator = (LSN_ACCELERATION_STRUCTURE &&_asOther ) noexcept {
        if ( this != &_asOther ) {
            Reset();
            rRes = _asOther.rRes;
            asAccelerationStructure = _asOther.asAccelerationStructure;
            pacAllocCallbacks = _asOther.pacAllocCallbacks;

            _asOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _asOther.asAccelerationStructure = VK_NULL_HANDLE;
            _asOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    }

    // == Functions.
    /**
     * Create a new acceleration structure object.
     *
     * \param _dDevice The logical device that creates the acceleration structure.
     * \param _pascCreateInfo A pointer to a VkAccelerationStructureCreateInfoKHR structure controlling creation of the acceleration structure.
     * \param _pacAllocator Controls host memory allocation.
     * \return Returns true if vkCreateAccelerationStructureKHR() returns VK_SUCCESS.
     **/
    inline bool                                         Create( VkDevice _dDevice, const VkAccelerationStructureCreateInfoKHR * _pascCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateAccelerationStructureKHR( _dDevice, _pascCreateInfo, _pacAllocator, &asAccelerationStructure );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    /**
     * Determines if the acceleration structure is valid.
     *
     * \return Returns true if the initial call to vkCreateAccelerationStructureKHR() was successful.
     **/
    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    /**
     * Resets the object to scratch.
     **/
    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyAccelerationStructureKHR( dDevice, asAccelerationStructure, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            asAccelerationStructure = VK_NULL_HANDLE;
        }
    }

    /**
     * Gets the return code after creation.
     *
     * \return Returns the result of creation.
     **/
    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkAccelerationStructureKHR                          asAccelerationStructure = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkBuffer wrapper. */
struct LSN_BUFFER {
    inline LSN_BUFFER() :
        bBuffer( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_BUFFER( VkDevice _dDevice, const VkBufferCreateInfo * _pbciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateBuffer( _dDevice, _pbciCreateInfo, _pacAllocator, &bBuffer ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_BUFFER( LSN_BUFFER &&_bOther ) noexcept : LSN_BUFFER() {
		(*this) = std::move( _bOther );
	}
    inline ~LSN_BUFFER() {
        Reset();
    }

    // == Operators.
    LSN_BUFFER &                                        operator = ( LSN_BUFFER &&_bOther ) noexcept {
        if ( this != &_bOther ) {
            Reset();
            rRes = _bOther.rRes;
            bBuffer = _bOther.bBuffer;
            pacAllocCallbacks = _bOther.pacAllocCallbacks;

            _bOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _bOther.bBuffer = VK_NULL_HANDLE;
            _bOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkBufferCreateInfo * _pbciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateBuffer( _dDevice, _pbciCreateInfo, _pacAllocator, &bBuffer );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyBuffer( dDevice, bBuffer, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            bBuffer = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkBuffer                                            bBuffer                 = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkCommandBuffer wrapper. */
struct LSN_COMMAND_BUFFER {
	inline LSN_COMMAND_BUFFER() :
		cbCommandBuffer( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		cpCommandPool( VK_NULL_HANDLE ),
		dDevice( nullptr ) {
	}
	inline LSN_COMMAND_BUFFER( VkDevice _dDevice, const VkCommandBufferAllocateInfo * _pcbaiAllocInfo ) :
		rRes( CVulkan::m_pfAllocateCommandBuffers( _dDevice, _pcbaiAllocInfo, &cbCommandBuffer ) ),
		cpCommandPool( _pcbaiAllocInfo ? _pcbaiAllocInfo->commandPool : VK_NULL_HANDLE ),
		dDevice( _dDevice ) {
	}
	inline LSN_COMMAND_BUFFER( LSN_COMMAND_BUFFER &&_cbOther ) noexcept : LSN_COMMAND_BUFFER() {
		(*this) = std::move( _cbOther );
	}
	inline ~LSN_COMMAND_BUFFER() {
		Reset();
	}

	// == Operators.
	inline LSN_COMMAND_BUFFER &							operator = ( LSN_COMMAND_BUFFER &&_cbOther ) noexcept {
		if ( this != &_cbOther ) {
			Reset();
			rRes = _cbOther.rRes;
			cbCommandBuffer = _cbOther.cbCommandBuffer;
			cpCommandPool = _cbOther.cpCommandPool;
			dDevice = _cbOther.dDevice;

			_cbOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_cbOther.cbCommandBuffer = VK_NULL_HANDLE;
			_cbOther.cpCommandPool = VK_NULL_HANDLE;
			_cbOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkCommandBufferAllocateInfo * _pcbaiAllocInfo ) {
		Reset();
		if ( !_pcbaiAllocInfo ) { return false; }
		rRes = CVulkan::m_pfAllocateCommandBuffers( _dDevice, _pcbaiAllocInfo, &cbCommandBuffer );
		cpCommandPool = _pcbaiAllocInfo->commandPool;
		dDevice = _dDevice;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || cbCommandBuffer != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && cpCommandPool && cbCommandBuffer ) {
				CVulkan::m_pfFreeCommandBuffers( dDevice, cpCommandPool, 1, &cbCommandBuffer );
			}
			cbCommandBuffer = VK_NULL_HANDLE;
			cpCommandPool = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkCommandBuffer										cbCommandBuffer			= VK_NULL_HANDLE;

private :
	VkCommandPool										cpCommandPool			= VK_NULL_HANDLE;
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
};

/** A Vulkan VkCommandPool wrapper. */
struct LSN_COMMAND_POOL {
    inline LSN_COMMAND_POOL() :
        cpCommandPool( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_COMMAND_POOL( VkDevice _dDevice, const VkCommandPoolCreateInfo * _pcpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateCommandPool( _dDevice, _pcpciCreateInfo, _pacAllocator, &cpCommandPool ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_COMMAND_POOL( LSN_COMMAND_POOL &&_cpOther ) noexcept : LSN_COMMAND_POOL() {
		(*this) = std::move( _cpOther );
	}
    inline ~LSN_COMMAND_POOL() {
        Reset();
    }

    // == Operators.
    LSN_COMMAND_POOL &                                  operator = ( LSN_COMMAND_POOL &&_cpOther ) noexcept {
        if ( this != &_cpOther ) {
            Reset();
            rRes = _cpOther.rRes;
            cpCommandPool = _cpOther.cpCommandPool;
            pacAllocCallbacks = _cpOther.pacAllocCallbacks;

            _cpOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _cpOther.cpCommandPool = VK_NULL_HANDLE;
            _cpOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkCommandPoolCreateInfo * _pcpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateCommandPool( _dDevice, _pcpciCreateInfo, _pacAllocator, &cpCommandPool );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyCommandPool( dDevice, cpCommandPool, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            cpCommandPool = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkCommandPool                                       cpCommandPool           = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkDescriptorPool wrapper. */
struct LSN_DESCRIPTOR_POOL {
    inline LSN_DESCRIPTOR_POOL() :
        dpDescriptorPool( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_DESCRIPTOR_POOL( VkDevice _dDevice, const VkDescriptorPoolCreateInfo * _pdpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateDescriptorPool( _dDevice, _pdpciCreateInfo, _pacAllocator, &dpDescriptorPool ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_DESCRIPTOR_POOL( LSN_DESCRIPTOR_POOL &&_dpOther ) noexcept : LSN_DESCRIPTOR_POOL() {
		(*this) = std::move( _dpOther );
	}
    inline ~LSN_DESCRIPTOR_POOL() {
        Reset();
    }

    // == Operators.
    LSN_DESCRIPTOR_POOL &                               operator = ( LSN_DESCRIPTOR_POOL &&_dpOther ) noexcept {
        if ( this != &_dpOther ) {
            Reset();
            rRes = _dpOther.rRes;
            dpDescriptorPool = _dpOther.dpDescriptorPool;
            pacAllocCallbacks = _dpOther.pacAllocCallbacks;

            _dpOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _dpOther.dpDescriptorPool = VK_NULL_HANDLE;
            _dpOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkDescriptorPoolCreateInfo * _pdpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateDescriptorPool( _dDevice, _pdpciCreateInfo, _pacAllocator, &dpDescriptorPool );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyDescriptorPool( dDevice, dpDescriptorPool, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            dpDescriptorPool = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkDescriptorPool                                    dpDescriptorPool        = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkDescriptorSet wrapper. */
struct LSN_DESCRIPTOR_SET {
	inline LSN_DESCRIPTOR_SET() :
		dsDescriptorSet( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		dpDescriptorPool( VK_NULL_HANDLE ),
		dDevice( nullptr ) {
	}
	inline LSN_DESCRIPTOR_SET( VkDevice _dDevice, const VkDescriptorSetAllocateInfo * _pdsaiAllocInfo ) :
		rRes( CVulkan::m_pfAllocateDescriptorSets( _dDevice, _pdsaiAllocInfo, &dsDescriptorSet ) ),
		dpDescriptorPool( _pdsaiAllocInfo ? _pdsaiAllocInfo->descriptorPool : VK_NULL_HANDLE ),
		dDevice( _dDevice ) {
	}
	inline LSN_DESCRIPTOR_SET( LSN_DESCRIPTOR_SET &&_dsOther ) noexcept : LSN_DESCRIPTOR_SET() {
		(*this) = std::move( _dsOther );
	}
	inline ~LSN_DESCRIPTOR_SET() {
		Reset();
	}

	// == Operators.
	inline LSN_DESCRIPTOR_SET &							operator = ( LSN_DESCRIPTOR_SET &&_dsOther ) noexcept {
		if ( this != &_dsOther ) {
			Reset();
			rRes = _dsOther.rRes;
			dsDescriptorSet = _dsOther.dsDescriptorSet;
			dpDescriptorPool = _dsOther.dpDescriptorPool;
			dDevice = _dsOther.dDevice;

			_dsOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_dsOther.dsDescriptorSet = VK_NULL_HANDLE;
			_dsOther.dpDescriptorPool = VK_NULL_HANDLE;
			_dsOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkDescriptorSetAllocateInfo * _pdsaiAllocInfo ) {
		Reset();
		if ( !_pdsaiAllocInfo ) { return false; }
		rRes = CVulkan::m_pfAllocateDescriptorSets( _dDevice, _pdsaiAllocInfo, &dsDescriptorSet );
		dpDescriptorPool = _pdsaiAllocInfo->descriptorPool;
		dDevice = _dDevice;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || dsDescriptorSet != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && dpDescriptorPool && dsDescriptorSet ) {
				CVulkan::m_pfFreeDescriptorSets( dDevice, dpDescriptorPool, 1, &dsDescriptorSet );
			}
			dsDescriptorSet = VK_NULL_HANDLE;
			dpDescriptorPool = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkDescriptorSet										dsDescriptorSet			= VK_NULL_HANDLE;

private :
	VkDescriptorPool									dpDescriptorPool		= VK_NULL_HANDLE;
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
};

/** A Vulkan VkDescriptorSetLayout wrapper. */
struct LSN_DESCRIPTOR_SET_LAYOUT {
    inline LSN_DESCRIPTOR_SET_LAYOUT() :
        dslDescriptorSetLayout( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_DESCRIPTOR_SET_LAYOUT( VkDevice _dDevice, const VkDescriptorSetLayoutCreateInfo * _pdslciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateDescriptorSetLayout( _dDevice, _pdslciCreateInfo, _pacAllocator, &dslDescriptorSetLayout ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_DESCRIPTOR_SET_LAYOUT( LSN_DESCRIPTOR_SET_LAYOUT &&_dslOther ) noexcept : LSN_DESCRIPTOR_SET_LAYOUT() {
		(*this) = std::move( _dslOther );
	}
    inline ~LSN_DESCRIPTOR_SET_LAYOUT() {
        Reset();
    }

    // == Operators.
    LSN_DESCRIPTOR_SET_LAYOUT &                         operator = ( LSN_DESCRIPTOR_SET_LAYOUT &&_dslOther ) noexcept {
        if ( this != &_dslOther ) {
            Reset();
            rRes = _dslOther.rRes;
            dslDescriptorSetLayout = _dslOther.dslDescriptorSetLayout;
            pacAllocCallbacks = _dslOther.pacAllocCallbacks;

            _dslOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _dslOther.dslDescriptorSetLayout = VK_NULL_HANDLE;
            _dslOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkDescriptorSetLayoutCreateInfo * _pdslciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateDescriptorSetLayout( _dDevice, _pdslciCreateInfo, _pacAllocator, &dslDescriptorSetLayout );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyDescriptorSetLayout( dDevice, dslDescriptorSetLayout, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            dslDescriptorSetLayout = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkDescriptorSetLayout                               dslDescriptorSetLayout  = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkDeviceMemory wrapper. */
struct LSN_DEVICE_MEMORY {
    inline LSN_DEVICE_MEMORY() :
        dmDeviceMemory( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_DEVICE_MEMORY( VkDevice _dDevice, const VkMemoryAllocateInfo * _pmaiAllocateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfAllocateMemory( _dDevice, _pmaiAllocateInfo, _pacAllocator, &dmDeviceMemory ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_DEVICE_MEMORY( LSN_DEVICE_MEMORY &&_dmOther ) noexcept : LSN_DEVICE_MEMORY() {
		(*this) = std::move( _dmOther );
	}
    inline ~LSN_DEVICE_MEMORY() {
        Reset();
    }

    // == Operators.
    LSN_DEVICE_MEMORY &                                 operator = ( LSN_DEVICE_MEMORY &&_dmOther ) noexcept {
        if ( this != &_dmOther ) {
            Reset();
            rRes = _dmOther.rRes;
            dmDeviceMemory = _dmOther.dmDeviceMemory;
            pacAllocCallbacks = _dmOther.pacAllocCallbacks;

            _dmOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _dmOther.dmDeviceMemory = VK_NULL_HANDLE;
            _dmOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkMemoryAllocateInfo * _pmaiAllocateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfAllocateMemory( _dDevice, _pmaiAllocateInfo, _pacAllocator, &dmDeviceMemory );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfFreeMemory( dDevice, dmDeviceMemory, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            dmDeviceMemory = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkDeviceMemory                                      dmDeviceMemory          = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkFence wrapper. */
struct LSN_FENCE {
	inline LSN_FENCE() :
		fFence( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_FENCE( VkDevice _dDevice, const VkFenceCreateInfo * _pfciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateFence( _dDevice, _pfciCreateInfo, _pacAllocator, &fFence ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_FENCE( LSN_FENCE &&_fOther ) noexcept : LSN_FENCE() {
		(*this) = std::move( _fOther );
	}
	inline ~LSN_FENCE() {
		Reset();
	}

	// == Operators.
	inline LSN_FENCE &									operator = ( LSN_FENCE &&_fOther ) noexcept {
		if ( this != &_fOther ) {
			Reset();
			rRes = _fOther.rRes;
			fFence = _fOther.fFence;
			pacAllocCallbacks = _fOther.pacAllocCallbacks;
			dDevice = _fOther.dDevice;

			_fOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_fOther.fFence = VK_NULL_HANDLE;
			_fOther.pacAllocCallbacks = nullptr;
			_fOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkFenceCreateInfo * _pfciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateFence( _dDevice, _pfciCreateInfo, _pacAllocator, &fFence );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || fFence != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && fFence ) {
				CVulkan::m_pfDestroyFence( dDevice, fFence, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			fFence = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Wait( uint64_t _ui64Timeout = UINT64_MAX ) const {
		if ( !Valid() ) { return VK_ERROR_INITIALIZATION_FAILED; }
		return CVulkan::m_pfWaitForFences( dDevice, 1, &fFence, VK_TRUE, _ui64Timeout );
	}

	inline VkResult										ResetFence() const {
		if ( !Valid() ) { return VK_ERROR_INITIALIZATION_FAILED; }
		return CVulkan::m_pfResetFences( dDevice, 1, &fFence );
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkFence												fFence					= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkFramebuffer wrapper. */
struct LSN_FRAMEBUFFER {
	inline LSN_FRAMEBUFFER() :
		fbFramebuffer( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_FRAMEBUFFER( VkDevice _dDevice, const VkFramebufferCreateInfo * _pfbciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateFramebuffer( _dDevice, _pfbciCreateInfo, _pacAllocator, &fbFramebuffer ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_FRAMEBUFFER( LSN_FRAMEBUFFER &&_fbOther ) noexcept : LSN_FRAMEBUFFER() {
		(*this) = std::move( _fbOther );
	}
	inline ~LSN_FRAMEBUFFER() {
		Reset();
	}

	// == Operators.
	inline LSN_FRAMEBUFFER &							operator = ( LSN_FRAMEBUFFER &&_fbOther ) noexcept {
		if ( this != &_fbOther ) {
			Reset();
			rRes = _fbOther.rRes;
			fbFramebuffer = _fbOther.fbFramebuffer;
			pacAllocCallbacks = _fbOther.pacAllocCallbacks;
			dDevice = _fbOther.dDevice;

			_fbOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_fbOther.fbFramebuffer = VK_NULL_HANDLE;
			_fbOther.pacAllocCallbacks = nullptr;
			_fbOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkFramebufferCreateInfo * _pfbciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		if ( !_pfbciCreateInfo ) { return false; }
		rRes = CVulkan::m_pfCreateFramebuffer( _dDevice, _pfbciCreateInfo, _pacAllocator, &fbFramebuffer );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || fbFramebuffer != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && fbFramebuffer ) {
				CVulkan::m_pfDestroyFramebuffer( dDevice, fbFramebuffer, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			fbFramebuffer = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkFramebuffer										fbFramebuffer			= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkDevice wrapper. */
struct LSN_DEVICE {
	inline LSN_DEVICE() :
		dDevice( nullptr ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ) {
	}
	inline LSN_DEVICE( VkPhysicalDevice _pdPhysicalDevice, const VkDeviceCreateInfo * _pdciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateDevice( _pdPhysicalDevice, _pdciCreateInfo, _pacAllocator, &dDevice ) ),
		pacAllocCallbacks( _pacAllocator ) {
	}
	inline LSN_DEVICE( LSN_DEVICE &&_dOther ) noexcept : LSN_DEVICE() {
		(*this) = std::move( _dOther );
	}
	inline ~LSN_DEVICE() {
		Reset();
	}


	// == Operators.
	/**
	 * Moves from _dOther to this object.  _dOther is left in a valid state and could be reused.
	 * 
	 * \param _dOther The object to move.
	 * \return Returns this object after the move.
	 **/
	LSN_DEVICE &										operator = ( LSN_DEVICE &&_dOther ) noexcept {
		if ( this != &_dOther ) {
			Reset();
			rRes = _dOther.rRes;
			dDevice = _dOther.dDevice;
			pacAllocCallbacks = _dOther.pacAllocCallbacks;

			_dOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_dOther.dDevice = nullptr;
			_dOther.pacAllocCallbacks = nullptr;
		}
		return (*this);
	};


	// == Functions.
	/**
	 * Create a new device instance.
	 * 
	 * \param _piciCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if vkCreateInstance() returns VK_SUCCESS.
	 **/
	inline bool											Create( VkPhysicalDevice _pdPhysicalDevice, const VkDeviceCreateInfo * _pdciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateDevice( _pdPhysicalDevice, _pdciCreateInfo, _pacAllocator, &dDevice );
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	/**
	 * Determines if the instance is valid.
	 *
	 * \return Returns true if the initial call to vkCreateInstance() was successful.
	 **/
	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	/**
	 * Resets the object to scratch.
	 **/
	inline void											Reset() {
		if ( Valid() ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			CVulkan::m_pfDestroyDevice( dDevice, pacAllocCallbacks );
			pacAllocCallbacks = nullptr;
			dDevice = nullptr;
		}
	}

	/**
	 * Gets the return code after creation.
	 *
	 * \return Returns the result of creation.
	 **/
	inline VkResult										Result() const { return rRes; }


	// == Members.
	VkDevice											dDevice					= nullptr;


private :
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkInstance wrapper. */
struct LSN_INSTANCE {
	inline LSN_INSTANCE() :
		iInstance( nullptr ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ) {
	}
	inline LSN_INSTANCE( const VkInstanceCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateInstance( _piciCreateInfo, _pacAllocator, &iInstance ) ),
		pacAllocCallbacks( _pacAllocator ) {
	}
	inline LSN_INSTANCE( LSN_INSTANCE &&_iOther ) noexcept : LSN_INSTANCE() {
		(*this) = std::move( _iOther );
	}
	inline ~LSN_INSTANCE() {
		Reset();
	}


	// == Operators.
	/**
	 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
	 * 
	 * \param _iOther The object to move.
	 * \return Returns this object after the move.
	 **/
	LSN_INSTANCE &										operator = ( LSN_INSTANCE &&_iOther ) noexcept {
		if ( this != &_iOther ) {
			Reset();
			rRes = _iOther.rRes;
			iInstance = _iOther.iInstance;
			pacAllocCallbacks = _iOther.pacAllocCallbacks;

			_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_iOther.iInstance = nullptr;
			_iOther.pacAllocCallbacks = nullptr;
		}
		return (*this);
	};


	// == Functions.
	/**
	 * Create a new Vulkan instance.
	 * 
	 * \param _piciCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if vkCreateInstance() returns VK_SUCCESS.
	 **/
	inline bool											Create( const VkInstanceCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateInstance( _piciCreateInfo, _pacAllocator, &iInstance );
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	/**
	 * Determines if the instance is valid.
	 *
	 * \return Returns true if the initial call to vkCreateInstance() was successful.
	 **/
	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	/**
	 * Resets the object to scratch.
	 **/
	inline void											Reset() {
		if ( Valid() ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			CVulkan::m_pfDestroyInstance( iInstance, pacAllocCallbacks );
			pacAllocCallbacks = nullptr;
			iInstance = nullptr;
		}
	}

	/**
	 * Gets the return code after creation.
	 *
	 * \return Returns the result of creation.
	 **/
	inline VkResult										Result() const { return rRes; }


	// == Members.
	VkInstance											iInstance				= nullptr;


private :
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkImage wrapper. */
struct LSN_IMAGE {
	inline LSN_IMAGE() :
		iImage( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_IMAGE( VkDevice _dDevice, const VkImageCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateImage( _dDevice, _piciCreateInfo, _pacAllocator, &iImage ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_IMAGE( LSN_IMAGE &&_iOther ) noexcept : LSN_IMAGE() {
		(*this) = std::move( _iOther );
	}
	inline ~LSN_IMAGE() {
		Reset();
	}


	// == Operators.
	/**
	 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
	 * 
	 * \param _iOther The object to move.
	 * \return Returns this object after the move.
	 **/
	LSN_IMAGE &											operator = ( LSN_IMAGE &&_iOther ) noexcept {
		if ( this != &_iOther ) {
			Reset();
			rRes = _iOther.rRes;
			iImage = _iOther.iImage;
			pacAllocCallbacks = _iOther.pacAllocCallbacks;

			_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_iOther.iImage = VK_NULL_HANDLE;
			_iOther.pacAllocCallbacks = nullptr;
		}
		return (*this);
	};


	// == Functions.
	/**
	 * Create a new image object.
	 *
	 * \param _dDevice The logical device that creates the image.
	 * \param _piciCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the image.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if vkCreateInstance() returns VK_SUCCESS.
	 **/
	inline bool											Create( VkDevice _dDevice, const VkImageCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateImage( _dDevice, _piciCreateInfo, _pacAllocator, &iImage );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	/**
	 * Determines if the image is valid.
	 *
	 * \return Returns true if the initial call to vkCreateInstance() was successful.
	 **/
	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	/**
	 * Resets the object to scratch.
	 **/
	inline void											Reset() {
		if ( Valid() ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			CVulkan::m_pfDestroyImage( dDevice, iImage, pacAllocCallbacks );
			pacAllocCallbacks = nullptr;
			iImage = VK_NULL_HANDLE;
		}
	}

	/**
	 * Gets the return code after creation.
	 *
	 * \return Returns the result of creation.
	 **/
	inline VkResult										Result() const { return rRes; }


	// == Members.
	VkImage												iImage				    = VK_NULL_HANDLE;


private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkImageView wrapper. */
struct LSN_IMAGE_VIEW {
    inline LSN_IMAGE_VIEW() :
        ivImageView( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_IMAGE_VIEW( VkDevice _dDevice, const VkImageViewCreateInfo * _pivciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateImageView( _dDevice, _pivciCreateInfo, _pacAllocator, &ivImageView ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_IMAGE_VIEW( LSN_IMAGE_VIEW &&_ivOther ) noexcept : LSN_IMAGE_VIEW() {
		(*this) = std::move( _ivOther );
	}
    inline ~LSN_IMAGE_VIEW() {
        Reset();
    }

    // == Operators.
    LSN_IMAGE_VIEW &                                    operator = ( LSN_IMAGE_VIEW &&_ivOther ) noexcept {
        if ( this != &_ivOther ) {
            Reset();
            rRes = _ivOther.rRes;
            ivImageView = _ivOther.ivImageView;
            pacAllocCallbacks = _ivOther.pacAllocCallbacks;

            _ivOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _ivOther.ivImageView = VK_NULL_HANDLE;
            _ivOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkImageViewCreateInfo * _pivciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateImageView( _dDevice, _pivciCreateInfo, _pacAllocator, &ivImageView );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyImageView( dDevice, ivImageView, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            ivImageView = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkImageView                                         ivImageView             = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkPipeline wrapper. */
struct LSN_PIPELINE {
    inline LSN_PIPELINE() :
        pPipeline( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_PIPELINE( VkDevice _dDevice, const VkGraphicsPipelineCreateInfo * _pgpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateGraphicsPipelines( _dDevice, VK_NULL_HANDLE, 1, _pgpciCreateInfo, _pacAllocator, &pPipeline ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_PIPELINE( LSN_PIPELINE &&_pOther ) noexcept : LSN_PIPELINE() {
		(*this) = std::move( _pOther );
	}
    inline ~LSN_PIPELINE() {
        Reset();
    }

    // == Operators.
    LSN_PIPELINE &                                      operator = ( LSN_PIPELINE &&_pOther ) noexcept {
        if ( this != &_pOther ) {
            Reset();
            rRes = _pOther.rRes;
            pPipeline = _pOther.pPipeline;
            pacAllocCallbacks = _pOther.pacAllocCallbacks;

            _pOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _pOther.pPipeline = VK_NULL_HANDLE;
            _pOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkGraphicsPipelineCreateInfo * _pgpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateGraphicsPipelines( _dDevice, VK_NULL_HANDLE, 1, _pgpciCreateInfo, _pacAllocator, &pPipeline );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

	inline bool											Create( VkDevice _dDevice, const VkComputePipelineCreateInfo * _pcpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateComputePipelines( _dDevice, VK_NULL_HANDLE, 1, _pcpciCreateInfo, _pacAllocator, &pPipeline );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyPipeline( dDevice, pPipeline, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            pPipeline = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkPipeline                                          pPipeline               = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkPipelineLayout wrapper. */
struct LSN_PIPELINE_LAYOUT {
    inline LSN_PIPELINE_LAYOUT() :
        plPipelineLayout( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_PIPELINE_LAYOUT( VkDevice _dDevice, const VkPipelineLayoutCreateInfo * _pplciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreatePipelineLayout( _dDevice, _pplciCreateInfo, _pacAllocator, &plPipelineLayout ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_PIPELINE_LAYOUT( LSN_PIPELINE_LAYOUT &&_plOther ) noexcept : LSN_PIPELINE_LAYOUT() {
		(*this) = std::move( _plOther );
	}
    inline ~LSN_PIPELINE_LAYOUT() {
        Reset();
    }

    // == Operators.
    LSN_PIPELINE_LAYOUT &                               operator = ( LSN_PIPELINE_LAYOUT &&_plOther ) noexcept {
        if ( this != &_plOther ) {
            Reset();
            rRes = _plOther.rRes;
            plPipelineLayout = _plOther.plPipelineLayout;
            pacAllocCallbacks = _plOther.pacAllocCallbacks;

            _plOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _plOther.plPipelineLayout = VK_NULL_HANDLE;
            _plOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkPipelineLayoutCreateInfo * _pplciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreatePipelineLayout( _dDevice, _pplciCreateInfo, _pacAllocator, &plPipelineLayout );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroyPipelineLayout( dDevice, plPipelineLayout, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            plPipelineLayout = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkPipelineLayout                                    plPipelineLayout        = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkQueryPool wrapper. */
struct LSN_QUERY_POOL {
	inline LSN_QUERY_POOL() :
		qpQueryPool( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_QUERY_POOL( VkDevice _dDevice, const VkQueryPoolCreateInfo * _pqpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateQueryPool( _dDevice, _pqpciCreateInfo, _pacAllocator, &qpQueryPool ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_QUERY_POOL( LSN_QUERY_POOL &&_qpOther ) noexcept : LSN_QUERY_POOL() {
		(*this) = std::move( _qpOther );
	}
	inline ~LSN_QUERY_POOL() {
		Reset();
	}

	// == Operators.
	LSN_QUERY_POOL &									operator = ( LSN_QUERY_POOL &&_qpOther ) noexcept {
		if ( this != &_qpOther ) {
			Reset();
			rRes = _qpOther.rRes;
			qpQueryPool = _qpOther.qpQueryPool;
			pacAllocCallbacks = _qpOther.pacAllocCallbacks;
			dDevice = _qpOther.dDevice;

			_qpOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_qpOther.qpQueryPool = VK_NULL_HANDLE;
			_qpOther.pacAllocCallbacks = nullptr;
			_qpOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkQueryPoolCreateInfo * _pqpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateQueryPool( _dDevice, _pqpciCreateInfo, _pacAllocator, &qpQueryPool );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || qpQueryPool != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && qpQueryPool ) {
				CVulkan::m_pfDestroyQueryPool( dDevice, qpQueryPool, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			qpQueryPool = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkQueryPool											qpQueryPool				= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkRenderPass wrapper. */
struct LSN_RENDER_PASS {
	inline LSN_RENDER_PASS() :
		rpRenderPass( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_RENDER_PASS( VkDevice _dDevice, const VkRenderPassCreateInfo * _prpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateRenderPass( _dDevice, _prpciCreateInfo, _pacAllocator, &rpRenderPass ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_RENDER_PASS( LSN_RENDER_PASS &&_rpOther ) noexcept : LSN_RENDER_PASS() {
		(*this) = std::move( _rpOther );
	}
	inline ~LSN_RENDER_PASS() {
		Reset();
	}

	// == Operators.
	inline LSN_RENDER_PASS &							operator = ( LSN_RENDER_PASS &&_rpOther ) noexcept {
		if ( this != &_rpOther ) {
			Reset();
			rRes = _rpOther.rRes;
			rpRenderPass = _rpOther.rpRenderPass;
			pacAllocCallbacks = _rpOther.pacAllocCallbacks;
			dDevice = _rpOther.dDevice;

			_rpOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_rpOther.rpRenderPass = VK_NULL_HANDLE;
			_rpOther.pacAllocCallbacks = nullptr;
			_rpOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkRenderPassCreateInfo * _prpciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		if ( !_prpciCreateInfo ) { return false; }
		rRes = CVulkan::m_pfCreateRenderPass( _dDevice, _prpciCreateInfo, _pacAllocator, &rpRenderPass );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || rpRenderPass != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && rpRenderPass ) {
				CVulkan::m_pfDestroyRenderPass( dDevice, rpRenderPass, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			rpRenderPass = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkRenderPass										rpRenderPass			= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkSampler wrapper. */
struct LSN_SAMPLER {
	inline LSN_SAMPLER() :
		sSampler( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_SAMPLER( VkDevice _dDevice, const VkSamplerCreateInfo * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateSampler( _dDevice, _psciCreateInfo, _pacAllocator, &sSampler ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_SAMPLER( LSN_SAMPLER &&_sOther ) noexcept : LSN_SAMPLER() {
		(*this) = std::move( _sOther );
	}
	inline ~LSN_SAMPLER() {
		Reset();
	}

	// == Operators.
	inline LSN_SAMPLER &								operator = ( LSN_SAMPLER &&_sOther ) noexcept {
		if ( this != &_sOther ) {
			Reset();
			rRes = _sOther.rRes;
			sSampler = _sOther.sSampler;
			pacAllocCallbacks = _sOther.pacAllocCallbacks;
			dDevice = _sOther.dDevice;

			_sOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_sOther.sSampler = VK_NULL_HANDLE;
			_sOther.pacAllocCallbacks = nullptr;
			_sOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkSamplerCreateInfo * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		if ( !_psciCreateInfo ) { return false; }
		rRes = CVulkan::m_pfCreateSampler( _dDevice, _psciCreateInfo, _pacAllocator, &sSampler );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || sSampler != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && sSampler ) {
				CVulkan::m_pfDestroySampler( dDevice, sSampler, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			sSampler = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkSampler											sSampler				= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkSemaphore wrapper. */
struct LSN_SEMAPHORE {
    inline LSN_SEMAPHORE() :
        sSemaphore( VK_NULL_HANDLE ),
        rRes( VK_ERROR_INITIALIZATION_FAILED ),
        pacAllocCallbacks( nullptr ),
        dDevice( nullptr ) {
    }
    inline LSN_SEMAPHORE( VkDevice _dDevice, const VkSemaphoreCreateInfo * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
        rRes( CVulkan::m_pfCreateSemaphore( _dDevice, _psciCreateInfo, _pacAllocator, &sSemaphore ) ),
        pacAllocCallbacks( _pacAllocator ),
        dDevice( _dDevice ) {
    }
	inline LSN_SEMAPHORE( LSN_SEMAPHORE &&_sOther ) noexcept : LSN_SEMAPHORE() {
		(*this) = std::move( _sOther );
	}
    inline ~LSN_SEMAPHORE() {
        Reset();
    }

    // == Operators.
    LSN_SEMAPHORE &                                     operator = ( LSN_SEMAPHORE &&_sOther ) noexcept {
        if ( this != &_sOther ) {
            Reset();
            rRes = _sOther.rRes;
            sSemaphore = _sOther.sSemaphore;
            pacAllocCallbacks = _sOther.pacAllocCallbacks;

            _sOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
            _sOther.sSemaphore = VK_NULL_HANDLE;
            _sOther.pacAllocCallbacks = nullptr;
        }
        return (*this);
    };

    // == Functions.
    inline bool                                         Create( VkDevice _dDevice, const VkSemaphoreCreateInfo * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
        Reset();
        rRes = CVulkan::m_pfCreateSemaphore( _dDevice, _psciCreateInfo, _pacAllocator, &sSemaphore );
        dDevice = _dDevice;
        pacAllocCallbacks = _pacAllocator;
        return Valid();
    }

    inline bool                                         Valid() const { return VK_SUCCESS == rRes; }

    inline void                                         Reset() {
        if ( Valid() ) {
            rRes = VK_ERROR_INITIALIZATION_FAILED;
            CVulkan::m_pfDestroySemaphore( dDevice, sSemaphore, pacAllocCallbacks );
            pacAllocCallbacks = nullptr;
            sSemaphore = VK_NULL_HANDLE;
        }
    }

    inline VkResult                                     Result() const { return rRes; }

    // == Members.
    VkSemaphore                                         sSemaphore              = VK_NULL_HANDLE;

private :
    VkDevice                                            dDevice                 = nullptr;
    VkResult                                            rRes                    = VK_ERROR_INITIALIZATION_FAILED;
    const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkShaderModule wrapper. */
struct LSN_SHADER_MODULE {
	inline LSN_SHADER_MODULE() :
		smShaderModule( VK_NULL_HANDLE ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ),
		dDevice( nullptr ) {
	}
	inline LSN_SHADER_MODULE( VkDevice _dDevice, const VkShaderModuleCreateInfo * _psmciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateShaderModule( _dDevice, _psmciCreateInfo, _pacAllocator, &smShaderModule ) ),
		pacAllocCallbacks( _pacAllocator ),
		dDevice( _dDevice ) {
	}
	inline LSN_SHADER_MODULE( LSN_SHADER_MODULE &&_smOther ) noexcept : LSN_SHADER_MODULE() {
		(*this) = std::move( _smOther );
	}
	inline ~LSN_SHADER_MODULE() {
		Reset();
	}

	// == Operators.
	inline LSN_SHADER_MODULE &							operator = ( LSN_SHADER_MODULE &&_smOther ) noexcept {
		if ( this != &_smOther ) {
			Reset();
			rRes = _smOther.rRes;
			smShaderModule = _smOther.smShaderModule;
			pacAllocCallbacks = _smOther.pacAllocCallbacks;
			dDevice = _smOther.dDevice;

			_smOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_smOther.smShaderModule = VK_NULL_HANDLE;
			_smOther.pacAllocCallbacks = nullptr;
			_smOther.dDevice = nullptr;
		}
		return (*this);
	};

	// == Functions.
	inline bool											Create( VkDevice _dDevice, const VkShaderModuleCreateInfo * _psmciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		if ( !_psmciCreateInfo ) { return false; }
		rRes = CVulkan::m_pfCreateShaderModule( _dDevice, _psmciCreateInfo, _pacAllocator, &smShaderModule );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	inline void											Reset() {
		if ( Valid() || smShaderModule != VK_NULL_HANDLE ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			if ( dDevice && smShaderModule ) {
				CVulkan::m_pfDestroyShaderModule( dDevice, smShaderModule, pacAllocCallbacks );
			}
			pacAllocCallbacks = nullptr;
			smShaderModule = VK_NULL_HANDLE;
			dDevice = nullptr;
		}
	}

	inline VkResult										Result() const { return rRes; }

	// == Members.
	VkShaderModule										smShaderModule			= VK_NULL_HANDLE;

private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkSurfaceKHR wrapper. */
struct LSN_SURFACE {
	inline LSN_SURFACE() :
		sSurface( 0 ),
		iInstance( nullptr ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ) {
	}
#ifdef LSN_WINDOWS
	inline LSN_SURFACE( VkInstance _iInstance, const VkWin32SurfaceCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateWin32SurfaceKHR( _iInstance, _psciCreateInfo, _pacAllocator, &sSurface ) ),
		iInstance( _iInstance ),
		pacAllocCallbacks( _pacAllocator ) {
	}
#endif	// #ifdef LSN_WINDOWS
	inline LSN_SURFACE( LSN_SURFACE &&_iOther ) noexcept : LSN_SURFACE() {
		(*this) = std::move( _iOther );
	}
	inline ~LSN_SURFACE() {
		Reset();
	}


	// == Operators.
	/**
	 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
	 * 
	 * \param _iOther The object to move.
	 * \return Returns this object after the move.
	 **/
	LSN_SURFACE &										operator = ( LSN_SURFACE &&_iOther ) noexcept {
		if ( this != &_iOther ) {
			Reset();
			rRes = _iOther.rRes;
			sSurface = _iOther.sSurface;
			iInstance = _iOther.iInstance;
			pacAllocCallbacks = _iOther.pacAllocCallbacks;

			_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_iOther.sSurface = 0;
			_iOther.iInstance = nullptr;
			_iOther.pacAllocCallbacks = nullptr;
		}
		return (*this);
	};


	// == Functions.
	/**
	 * Create a new Vulkan surface.
	 * 
	 * \param _iInstance The instance to associate the surface with.
	 * \param _psciCreateInfo A pointer to a VkWin32SurfaceCreateInfoKHR structure containing parameters affecting the creation of the surface object.
	 * \param _pacAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
	 * \return Returns true if vkCreateWin32SurfaceKHR() returns VK_SUCCESS.
	 **/
	inline bool											Create( VkInstance _iInstance, const VkWin32SurfaceCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateWin32SurfaceKHR( _iInstance, _psciCreateInfo, _pacAllocator, &sSurface );
		iInstance = _iInstance;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	/**
	 * Determines if the instance is valid.
	 *
	 * \return Returns true if the initial call to vkCreateInstance() was successful.
	 **/
	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	/**
	 * Resets the object to scratch.
	 **/
	inline void											Reset() {
		if ( Valid() ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			CVulkan::m_pfDestroySurfaceKHR( iInstance, sSurface, pacAllocCallbacks );
			iInstance = nullptr;
			pacAllocCallbacks = nullptr;
			sSurface = 0;
		}
	}

	/**
	 * Gets the return code after creation.
	 *
	 * \return Returns the result of creation.
	 **/
	inline VkResult										Result() const { return rRes; }


	// == Members.
	VkSurfaceKHR										sSurface				= 0;


private :
	VkInstance											iInstance				= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};

/** A Vulkan VkSwapchainKHR wrapper. */
struct LSN_SWAPCHAIN {
	inline LSN_SWAPCHAIN() :
		sSwapchain( 0 ),
		dDevice( nullptr ),
		rRes( VK_ERROR_INITIALIZATION_FAILED ),
		pacAllocCallbacks( nullptr ) {
	}
#ifdef LSN_WINDOWS
	inline LSN_SWAPCHAIN( VkDevice _dDevice, const VkSwapchainCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
		rRes( CVulkan::m_pfCreateSwapchainKHR( _dDevice, _psciCreateInfo, _pacAllocator, &sSwapchain ) ),
		dDevice( _dDevice ),
		pacAllocCallbacks( _pacAllocator ) {
	}
#endif	// #ifdef LSN_WINDOWS
	inline LSN_SWAPCHAIN( LSN_SWAPCHAIN &&_scOther ) noexcept : LSN_SWAPCHAIN() {
		(*this) = std::move( _scOther );
	}
	inline ~LSN_SWAPCHAIN() {
		Reset();
	}


	// == Operators.
	/**
	 * Moves from _scOther to this object.  _scOther is left in a valid state and could be reused.
	 * 
	 * \param _scOther The object to move.
	 * \return Returns this object after the move.
	 **/
	LSN_SWAPCHAIN &										operator = ( LSN_SWAPCHAIN &&_scOther ) noexcept {
		if ( this != &_scOther ) {
			Reset();
			rRes = _scOther.rRes;
			sSwapchain = _scOther.sSwapchain;
			dDevice = _scOther.dDevice;
			pacAllocCallbacks = _scOther.pacAllocCallbacks;

			_scOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
			_scOther.sSwapchain = 0;
			_scOther.dDevice = nullptr;
			_scOther.pacAllocCallbacks = nullptr;
		}
		return (*this);
	};


	// == Functions.
	/**
	 * Create a new Vulkan swapchain.
	 * 
	 * \param _iInstance The device to create the swapchain for.
	 * \param _psciCreateInfo A pointer to a VkSwapchainCreateInfoKHR structure specifying the parameters of the created swapchain.
	 * \param _pacAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
	 * \return Returns true if vkCreateSwapchainKHR() returns VK_SUCCESS.
	 **/
	inline bool											Create( VkDevice _dDevice, const VkSwapchainCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
		Reset();
		rRes = CVulkan::m_pfCreateSwapchainKHR( _dDevice, _psciCreateInfo, _pacAllocator, &sSwapchain );
		dDevice = _dDevice;
		pacAllocCallbacks = _pacAllocator;
		return Valid();
	}

	/**
	 * Determines if the instance is valid.
	 *
	 * \return Returns true if the initial call to vkCreateInstance() was successful.
	 **/
	inline bool											Valid() const { return VK_SUCCESS == rRes; }

	/**
	 * Resets the object to scratch.
	 **/
	inline void											Reset() {
		if ( Valid() ) {
			rRes = VK_ERROR_INITIALIZATION_FAILED;
			CVulkan::m_pfDestroySwapchainKHR( dDevice, sSwapchain, pacAllocCallbacks );
			dDevice = nullptr;
			pacAllocCallbacks = nullptr;
			sSwapchain = 0;
		}
	}

	/**
	 * Gets the return code after creation.
	 *
	 * \return Returns the result of creation.
	 **/
	inline VkResult										Result() const { return rRes; }


	// == Members.
	VkSwapchainKHR										sSwapchain				= 0;


private :
	VkDevice											dDevice					= nullptr;
	VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
	const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
};