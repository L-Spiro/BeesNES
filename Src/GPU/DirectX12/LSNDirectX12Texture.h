/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 12 texture.
 */


#ifdef LSN_DX12

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../LSNTextureBase.h"
#include "LSNDirectX12Resource.h"
#include "LSNDirectX12Device.h"

namespace lsn {

	/**
	 * Class CDirectX12Texture
	 * \brief A Direct3D 12 texture.
	 *
	 * Description: A Direct3D 12 texture.
	 */
	class CDirectX12Texture : public CDirectX12Resource, public CTextureBase {
	public :
		// == Various constructors.
		CDirectX12Texture();
		virtual ~CDirectX12Texture();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		virtual void											Reset();

		/**
		 * Creates a texture.
		 *
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _phppHeapProperties A pointer to a D3D12_HEAP_PROPERTIES structure that provides properties for the resource's heap.
		 * \param _hfHeapFlags Heap options, as a bitwise-OR'd combination of D3D12_HEAP_FLAGS enumeration constants.
		 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
		 * \param _rsInitialResourceState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants. When you create a resource together with a D3D12_HEAP_TYPE_UPLOAD heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_GENERIC_READ. When you create a resource together with a D3D12_HEAP_TYPE_READBACK heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_COPY_DEST.
		 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE structure that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, you should choose the value with which the clear operation will most commonly be called. You can call the clear operation with other values, but those operations won't be as efficient as when the value matches the one passed in to resource creation. When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr
		 * \return Returns true if the comitted resource was created.
		 **/
		bool													CreateCommittedTexture(
			ID3D12Device * _pd12dDevice,
			const D3D12_HEAP_PROPERTIES * _phppHeapProperties,
			D3D12_HEAP_FLAGS _hfHeapFlags,
			const D3D12_RESOURCE_DESC * _prdDesc,
			D3D12_RESOURCE_STATES _rsInitialResourceState,
			const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue = nullptr );

		/**
		 * Creates a texture that is placed in a specific heap. Placed resources are the lightest weight resource objects available, and are the fastest to create and destroy.
		 *
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _phHeap A pointer to the ID3D12Heap interface that represents the heap in which the resource is placed.
		 * \param _ui64HeapOffset The offset, in bytes, to the resource. The HeapOffset must be a multiple of the resource's alignment, and HeapOffset plus the resource size must be smaller than or equal to the heap size. GetResourceAllocationInfo must be used to understand the sizes of texture resources.
		 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
		 * \param _rsInitialState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants. When a resource is created together with a D3D12_HEAP_TYPE_UPLOAD heap, InitialState must be D3D12_RESOURCE_STATE_GENERIC_READ. When a resource is created together with a D3D12_HEAP_TYPE_READBACK heap, InitialState must be D3D12_RESOURCE_STATE_COPY_DEST.
		 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, your application should choose the value that the clear operation will most commonly be called with. Clear operations can be called with other values, but those operations will not be as efficient as when the value matches the one passed into resource creation. pOptimizedClearValue must be NULL when used with D3D12_RESOURCE_DIMENSION_BUFFER.
		 * \return Returns true if the resource was created.
		 **/
		bool													CreatePlacedTexture(
			ID3D12Device * _pd12dDevice,
			ID3D12Heap * _phHeap,
			UINT64 _ui64HeapOffset,
			const D3D12_RESOURCE_DESC * _prdDesc,
			D3D12_RESOURCE_STATES _rsInitialState,
			const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue = nullptr );

		/**
		 * Creates a texture that is placed in a specific heap. Placed resources are the lightest weight resource objects available, and are the fastest to create and destroy.
		 *
		 * \param _pd12dDevice The DirectX 12 device.
		 * \param _prdDesc A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
		 * \param _rsInitialState The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants.
		 * \param _pcsOptimizedClearValue Specifies a D3D12_CLEAR_VALUE structure that describes the default value for a clear color. pOptimizedClearValue specifies a value for which clear operations are most optimal. When the created resource is a texture with either the D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET or D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL flags, you should choose the value with which the clear operation will most commonly be called. You can call the clear operation with other values, but those operations won't be as efficient as when the value matches the one passed in to resource creation. When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr.
		 * \return Returns true if the resource was created.
		 **/
		bool													CreateReservedTexture(
			ID3D12Device * _pd12dDevice,
			const D3D12_RESOURCE_DESC * _prdDesc,
			D3D12_RESOURCE_STATES _rsInitialState,
			const D3D12_CLEAR_VALUE * _pcsOptimizedClearValue = nullptr );


	protected :
		// == Members.

	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
