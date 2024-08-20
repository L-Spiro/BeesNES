/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 resource.
 */


#ifdef LSN_DX12

#include "LSNDirectX12Resource.h"

namespace lsn {

	// == Various constructors.
	CDirectX12Resource::CDirectX12Resource() {
	}
	CDirectX12Resource::~CDirectX12Resource() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CDirectX12Resource::Reset() {
		if ( m_pd12rResource ) {
			m_pd12rResource->Release();
			m_pd12rResource = nullptr;
		}
	}

	/**
	 * Creates both a resource and an implicit heap, such that the heap is big enough to contain the entire resource, and the resource is mapped to the heap.
	 *
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _phppHeapProperties A pointer to a D3D12_HEAP_PROPERTIES structure that provides properties for the resource's heap.
	 * \param _hfHeapFlags Heap options, as a bitwise-OR'd combination of D3D12_HEAP_FLAGS enumeration constants.
	 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
	 * \param _rsInitialResourceState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants. When you create a resource together with a D3D12_HEAP_TYPE_UPLOAD heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_GENERIC_READ. When you create a resource together with a D3D12_HEAP_TYPE_READBACK heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_COPY_DEST.
	 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE structure that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, you should choose the value with which the clear operation will most commonly be called. You can call the clear operation with other values, but those operations won't be as efficient as when the value matches the one passed in to resource creation. When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr
	 * \return Returns true if the resource was created.
	 **/
	bool CDirectX12Resource::CreateCommittedResource(
		ID3D12Device * _pd12dDevice,
		const D3D12_HEAP_PROPERTIES * _phppHeapProperties,
		D3D12_HEAP_FLAGS _hfHeapFlags,
		const D3D12_RESOURCE_DESC * _prdDesc,
		D3D12_RESOURCE_STATES _rsInitialResourceState,
		const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		m_rsResourceState = _rsInitialResourceState;
		return SUCCEEDED( _pd12dDevice->CreateCommittedResource( _phppHeapProperties, _hfHeapFlags, _prdDesc, _rsInitialResourceState, _pcsOptimizedClearValue,
			IID_PPV_ARGS( &m_pd12rResource ) ) );
	}

	/**
	 * Creates a resource that is placed in a specific heap. Placed resources are the lightest weight resource objects available, and are the fastest to create and destroy.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _phHeap A pointer to the ID3D12Heap interface that represents the heap in which the resource is placed.
	 * \param _ui64HeapOffset The offset, in bytes, to the resource. The HeapOffset must be a multiple of the resource's alignment, and HeapOffset plus the resource size must be smaller than or equal to the heap size. GetResourceAllocationInfo must be used to understand the sizes of texture resources.
	 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
	 * \param _rsInitialState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants. When a resource is created together with a D3D12_HEAP_TYPE_UPLOAD heap, InitialState must be D3D12_RESOURCE_STATE_GENERIC_READ. When a resource is created together with a D3D12_HEAP_TYPE_READBACK heap, InitialState must be D3D12_RESOURCE_STATE_COPY_DEST.
	 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, your application should choose the value that the clear operation will most commonly be called with. Clear operations can be called with other values, but those operations will not be as efficient as when the value matches the one passed into resource creation. pOptimizedClearValue must be NULL when used with D3D12_RESOURCE_DIMENSION_BUFFER.
	 * \return Returns true if the resource was created.
	 **/
	bool CDirectX12Resource::CreatePlacedResource(
		ID3D12Device * _pd12dDevice,
		ID3D12Heap * _phHeap,
		UINT64 _ui64HeapOffset,
		const D3D12_RESOURCE_DESC * _prdDesc,
		D3D12_RESOURCE_STATES _rsInitialState,
		const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		m_rsResourceState = _rsInitialState;
		return SUCCEEDED( _pd12dDevice->CreatePlacedResource( _phHeap, _ui64HeapOffset, _prdDesc, _rsInitialState, _pcsOptimizedClearValue,
			IID_PPV_ARGS( &m_pd12rResource ) ) );
	}

	/**
	 * Creates a resource that is placed in a specific heap. Placed resources are the lightest weight resource objects available, and are the fastest to create and destroy.
	 *
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
	 * \param _rsInitialState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants.
	 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE structure that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, you should choose the value with which the clear operation will most commonly be called. You can call the clear operation with other values, but those operations won't be as efficient as when the value matches the one passed in to resource creation. When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr.
	 * \return Returns true if the resource was created.
	 **/
	bool CDirectX12Resource::CreateReservedResource(
		ID3D12Device * _pd12dDevice,
		const D3D12_RESOURCE_DESC * _prdDesc,
		D3D12_RESOURCE_STATES _rsInitialState,
		const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue ) {
		if ( !_pd12dDevice ) { return false; }
		Reset();
		m_rsResourceState = _rsInitialState;
		return SUCCEEDED( _pd12dDevice->CreateReservedResource( _prdDesc, _rsInitialState, _pcsOptimizedClearValue,
			IID_PPV_ARGS( &m_pd12rResource ) ) );
	}

	/**
	 * Creates a shader-resource view for accessing data in a resource.
	 * 
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _psrvdDesc A pointer to a D3D12_SHADER_RESOURCE_VIEW_DESC structure that describes the shader-resource view. A null pDesc is used to initialize a default descriptor, if possible. This behavior is identical to the D3D11 null descriptor behavior, where defaults are filled in. This behavior inherits the resource format and dimension (if not typeless) and for buffers SRVs target a full buffer and are typed (not raw or structured), and for textures SRVs target a full texture, all mips and all array slices. Not all resources support null descriptor initialization.
	 * \param _cdhDestDescriptor Describes the CPU descriptor handle that represents the shader-resource view. This handle can be created in a shader-visible or non-shader-visible descriptor heap.
	 * \return Returns true if the resource has already been allocated.
	 **/
	bool CDirectX12Resource::CreateShaderResourceView(
		ID3D12Device * _pd12dDevice,
		const D3D12_SHADER_RESOURCE_VIEW_DESC * _psrvdDesc,
		D3D12_CPU_DESCRIPTOR_HANDLE _cdhDestDescriptor ) {
		if ( !m_pd12rResource || !_pd12dDevice ) { return false; }
		_pd12dDevice->CreateShaderResourceView( m_pd12rResource, _psrvdDesc, _cdhDestDescriptor );
		return true;
	}

	/**
	 * Creates a render-target view for accessing resource data.
	 *
	 * \param _prtvdDesc A pointer to a D3D12_RENDER_TARGET_VIEW_DESC structure that describes the render-target view. A null pDesc is used to initialize a default descriptor, if possible. This behavior is identical to the D3D11 null descriptor behavior, where defaults are filled in. This behavior inherits the resource format and dimension (if not typeless) and RTVs target the first mip and all array slices. Not all resources support null descriptor initialization.
	 * \param _cdhDestDescriptor Describes the CPU descriptor handle that represents the destination where the newly-created render target view will reside.
	 * \return Returns true if the comitted resource was created.
	 **/
	bool CDirectX12Resource::CreateRenderTargetView(
		ID3D12Device * _pd12dDevice,
		const D3D12_RENDER_TARGET_VIEW_DESC * _prtvdDesc,
		D3D12_CPU_DESCRIPTOR_HANDLE _cdhDestDescriptor ) {
		if ( !m_pd12rResource || !_pd12dDevice ) { return false; }
		_pd12dDevice->CreateRenderTargetView( m_pd12rResource, _prtvdDesc, _cdhDestDescriptor );
		return true;
	}

	/**
	 * Creates a constant-buffer view for accessing resource data.
	 *
	 * \param _pd12dDevice The DirectX 12 device.
	 * \param _prtvdDesc A pointer to a D3D12_CONSTANT_BUFFER_VIEW_DESC structure that describes the constant-buffer view.
	 * \param _cdhDestDescriptor Describes the CPU descriptor handle that represents the start of the heap that holds the constant-buffer view.
	 * \return Returns true if the comitted resource was created.
	 **/
	bool CDirectX12Resource::CreateConstantBufferView(
		ID3D12Device * _pd12dDevice,
		const D3D12_CONSTANT_BUFFER_VIEW_DESC * _pcbvdDesc,
		D3D12_CPU_DESCRIPTOR_HANDLE _cdhDestDescriptor ) {
		if ( !_pd12dDevice ) { return false; }
		_pd12dDevice->CreateConstantBufferView( _pcbvdDesc, _cdhDestDescriptor );
		return true;
	}

}	// namespace lsn


#endif	// #ifdef LSN_DX12
