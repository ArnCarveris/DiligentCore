/*     Copyright 2015-2016 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//
// Adapted to Diligent Engine: Egor Yusov
//

#include "pch.h"
#include "CommandContext.h"
#include "TextureD3D12Impl.h"
#include "BufferD3D12Impl.h"
#include "CommandListManager.h"

namespace Diligent
{

CommandContext::CommandContext( CommandListManager &CmdListManager, 
                                LinearAllocatorPageManager &GPUPageManager, 
                                LinearAllocatorPageManager &CPUPageManager,
                                RefCntAutoPtr<DynamicDescriptorHeapManager> DynamicGPUDescriptorHeapManagers[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]   ) :
	//m_DynamicDescriptorHeap(*this),
	//m_CpuLinearAllocator(kCpuWritable), 
	//m_GpuLinearAllocator(kGpuExclusive),
	m_pCurGraphicsRootSignature( nullptr),
	m_pCurGraphicsPipelineState( nullptr),
	m_pCurComputeRootSignature( nullptr),
	m_pCurComputePipelineState( nullptr),
    m_GpuLinearAllocator(GPUPageManager),
    m_CpuLinearAllocator(CPUPageManager)
{
    for(size_t HeapType = 0; HeapType < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++HeapType)
        m_pDynamicGPUDescriptorAllocator[HeapType] = new DynamicDescriptorHeapAllocator(*DynamicGPUDescriptorHeapManagers[HeapType]);

    m_PendingResourceBarriers.reserve(MaxPendingBarriers);
    m_PendingBarrierObjects.reserve(MaxPendingBarriers);
	ZeroMemory(m_CurrentDescriptorHeaps, sizeof(m_CurrentDescriptorHeaps));

    CmdListManager.CreateNewCommandList(&m_pCommandList, &m_pCurrentAllocator);
}

CommandContext::~CommandContext( void )
{
}



void CommandContext::Reset( CommandListManager& CmdListManager )
{
	// We only call Reset() on previously freed contexts.  The command list persists, but we must
	// request a new allocator.
	VERIFY_EXPR(m_pCommandList != nullptr && m_pCurrentAllocator == nullptr);
    m_pCurrentAllocator.Release();
    CmdListManager.RequestAllocator(&m_pCurrentAllocator);
	m_pCommandList->Reset(m_pCurrentAllocator, nullptr);

	m_pCurGraphicsRootSignature = nullptr;
	m_pCurGraphicsPipelineState = nullptr;
	m_pCurComputeRootSignature = nullptr;
	m_pCurComputePipelineState = nullptr;
	m_PendingResourceBarriers.clear();
    m_PendingBarrierObjects.clear();
#if 0
	BindDescriptorHeaps();
#endif
}

ID3D12GraphicsCommandList* CommandContext::Close(ID3D12CommandAllocator **ppAllocator)
{
	FlushResourceBarriers();

	//if (m_ID.length() > 0)
	//	EngineProfiling::EndBlock(this);

	VERIFY_EXPR(m_pCurrentAllocator != nullptr);
	auto hr = m_pCommandList->Close();
    VERIFY(SUCCEEDED(hr), "Failed to close the command list");

    *ppAllocator = m_pCurrentAllocator.Detach();
    return m_pCommandList;
}


#if 0
void CommandContext::BindDescriptorHeaps( void )
{
	UINT NonNullHeaps = 0;
	ID3D12DescriptorHeap* HeapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		ID3D12DescriptorHeap* HeapIter = m_CurrentDescriptorHeaps[i];
		if (HeapIter != nullptr)
			HeapsToBind[NonNullHeaps++] = HeapIter;
	}

	if (NonNullHeaps > 0)
		m_pCommandList->SetDescriptorHeaps(NonNullHeaps, HeapsToBind);
}
#endif 

void GraphicsContext::SetRenderTargets( UINT NumRTVs, ITextureViewD3D12** ppRTVs, ITextureViewD3D12* pDSV )
{
    D3D12_CPU_DESCRIPTOR_HANDLE RTVHandles[8] = {};

	for (UINT i = 0; i < NumRTVs; ++i)
	{
        auto *pRTV = ppRTVs[i];
        if( pRTV )
        {
            auto *pTexture = ValidatedCast<TextureD3D12Impl>( pRTV->GetTexture() );
	        TransitionResource(pTexture, D3D12_RESOURCE_STATE_RENDER_TARGET);
		    RTVHandles[i] = pRTV->GetCPUDescriptorHandle();
        }
	}

	if (pDSV)
	{
        auto *pTexture = ValidatedCast<TextureD3D12Impl>( pDSV->GetTexture() );
		//if (bReadOnlyDepth)
		//{
		//	TransitionResource(*pTexture, D3D12_RESOURCE_STATE_DEPTH_READ);
		//	m_pCommandList->OMSetRenderTargets( NumRTVs, RTVHandles, FALSE, &DSV->GetDSV_DepthReadOnly() );
		//}
		//else
		{
			TransitionResource(pTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
            auto DSVHandle = pDSV->GetCPUDescriptorHandle();
			m_pCommandList->OMSetRenderTargets( NumRTVs, RTVHandles, FALSE, &DSVHandle );
		}
	}
	else
	{
		m_pCommandList->OMSetRenderTargets( NumRTVs, RTVHandles, FALSE, nullptr );
	}
}

void CommandContext::ClearUAVFloat( ITextureViewD3D12 *pTexView, const float* Color )
{
    auto *pTexture = ValidatedCast<TextureD3D12Impl>( pTexView->GetTexture() );
	TransitionResource(pTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

	// After binding a UAV, we can get a GPU handle that is required to clear it as a UAV (because it essentially runs
	// a shader to set all of the values).
    UNSUPPORTED("Not yet implemented");
    D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibleHandle = {};//m_DynamicDescriptorHeap.UploadDirect(Target.GetUAV());
	m_pCommandList->ClearUnorderedAccessViewFloat(GpuVisibleHandle, pTexView->GetCPUDescriptorHandle(), pTexture->GetD3D12Resource(), Color, 0, nullptr);
}

void CommandContext::ClearUAVUint( ITextureViewD3D12 *pTexView, const UINT *Color  )
{
    auto *pTexture = ValidatedCast<TextureD3D12Impl>( pTexView->GetTexture() );
	TransitionResource(pTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

	// After binding a UAV, we can get a GPU handle that is required to clear it as a UAV (because it essentially runs
	// a shader to set all of the values).
    UNSUPPORTED("Not yet implemented");
    D3D12_GPU_DESCRIPTOR_HANDLE GpuVisibleHandle = {};//m_DynamicDescriptorHeap.UploadDirect(Target.GetUAV());
	//CD3DX12_RECT ClearRect(0, 0, (LONG)Target.GetWidth(), (LONG)Target.GetHeight());

	//TODO: My Nvidia card is not clearing UAVs with either Float or Uint variants.
	m_pCommandList->ClearUnorderedAccessViewUint(GpuVisibleHandle, pTexView->GetCPUDescriptorHandle(), pTexture->GetD3D12Resource(), Color, 0, nullptr/*1, &ClearRect*/);
}


void GraphicsContext::ClearRenderTarget( ITextureViewD3D12 *pRTV, const float *Color )
{
    auto *pTexture = ValidatedCast<TextureD3D12Impl>( pRTV->GetTexture() );
	TransitionResource(pTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	m_pCommandList->ClearRenderTargetView(pRTV->GetCPUDescriptorHandle(), Color, 0, nullptr);
}

void GraphicsContext::ClearDepthStencil( ITextureViewD3D12 *pDSV, D3D12_CLEAR_FLAGS ClearFlags, float Depth, UINT8 Stencil )
{
    auto *pTexture = ValidatedCast<TextureD3D12Impl>( pDSV->GetTexture() );
	TransitionResource( pTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
	m_pCommandList->ClearDepthStencilView(pDSV->GetCPUDescriptorHandle(), ClearFlags, Depth, Stencil, 0, nullptr);
}


void CommandContext::TransitionResource(ITextureD3D12 *pTexture, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
{
    VERIFY_EXPR( pTexture != nullptr );
    auto *pTexD3D12 = ValidatedCast<TextureD3D12Impl>(pTexture);
    TransitionResource(*pTexD3D12, *pTexD3D12, NewState, FlushImmediate);
}

void CommandContext::TransitionResource(IBufferD3D12 *pBuffer, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
{
    VERIFY_EXPR( pBuffer != nullptr );
    auto *pBuffD3D12 = ValidatedCast<BufferD3D12Impl>(pBuffer);
    TransitionResource(*pBuffD3D12, *pBuffD3D12, NewState, FlushImmediate);
}

void CommandContext::TransitionResource(D3D12ResourceBase& Resource, IDeviceObject &Object, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
{
	D3D12_RESOURCE_STATES OldState = Resource.GetState();

	if (OldState != NewState)
	{
        m_PendingResourceBarriers.emplace_back();
        m_PendingBarrierObjects.emplace_back(&Object);
		D3D12_RESOURCE_BARRIER& BarrierDesc = m_PendingResourceBarriers.back();

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		BarrierDesc.Transition.pResource = Resource.GetD3D12Resource();
		BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		BarrierDesc.Transition.StateBefore = OldState;
		BarrierDesc.Transition.StateAfter = NewState;

		// Check to see if we already started the transition
#if 0
		if (NewState == Resource.m_TransitioningState)
		{
			BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
			Resource.m_TransitioningState = (D3D12_RESOURCE_STATES)-1;
		}
		else
#endif
			BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		Resource.SetState( NewState );
	}
	else if (NewState == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		InsertUAVBarrier(Resource, Object, FlushImmediate);

	if (FlushImmediate || m_PendingResourceBarriers.size() >= MaxPendingBarriers)
        FlushResourceBarriers();
}

#if 0
void CommandContext::BeginResourceTransition(GpuResource& Resource, D3D12_RESOURCE_STATES NewState, bool FlushImmediate)
{
	// If it's already transitioning, finish that transition
	if (Resource.m_TransitioningState != (D3D12_RESOURCE_STATES)-1)
		TransitionResource(Resource, Resource.m_TransitioningState);

	D3D12_RESOURCE_STATES OldState = Resource.m_UsageState;

	if (OldState != NewState)
	{
		ASSERT(m_NumBarriersToFlush < 16, "Exceeded arbitrary limit on buffered barriers");
		D3D12_RESOURCE_BARRIER& BarrierDesc = m_ResourceBarrierBuffer[m_NumBarriersToFlush++];

		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		BarrierDesc.Transition.pResource = Resource.GetResource();
		BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		BarrierDesc.Transition.StateBefore = OldState;
		BarrierDesc.Transition.StateAfter = NewState;

		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;

		Resource.m_TransitioningState = NewState;
	}

	if (FlushImmediate || m_NumBarriersToFlush == 16)
	{
		m_pCommandList->ResourceBarrier(m_NumBarriersToFlush, m_ResourceBarrierBuffer);
		m_NumBarriersToFlush = 0;
	}
}
#endif

void CommandContext::FlushResourceBarriers()
{
	if (m_PendingResourceBarriers.empty())
    {
        VERIFY_EXPR(m_PendingBarrierObjects.empty());
		return;
    }

	m_pCommandList->ResourceBarrier(static_cast<UINT>(m_PendingResourceBarriers.size()), m_PendingResourceBarriers.data());
	m_PendingResourceBarriers.clear();
    m_PendingBarrierObjects.clear();
}


void CommandContext::InsertUAVBarrier(D3D12ResourceBase& Resource, IDeviceObject &Object, bool FlushImmediate)
{
    m_PendingResourceBarriers.emplace_back();
    m_PendingBarrierObjects.emplace_back(&Object);
	D3D12_RESOURCE_BARRIER& BarrierDesc = m_PendingResourceBarriers.back();

	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.UAV.pResource = Resource.GetD3D12Resource();

	if (FlushImmediate)
        FlushResourceBarriers();
}

void CommandContext::InsertAliasBarrier(D3D12ResourceBase& Before, D3D12ResourceBase& After, IDeviceObject &BeforeObj, IDeviceObject &AfterObj, bool FlushImmediate)
{
    m_PendingResourceBarriers.emplace_back();
    m_PendingBarrierObjects.emplace_back(&BeforeObj);
    m_PendingBarrierObjects.emplace_back(&AfterObj);
	D3D12_RESOURCE_BARRIER& BarrierDesc = m_PendingResourceBarriers.back();

	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Aliasing.pResourceBefore = Before.GetD3D12Resource();
	BarrierDesc.Aliasing.pResourceAfter = After.GetD3D12Resource();

	if (FlushImmediate)
        FlushResourceBarriers();
}

void CommandContext::DiscardUsedPages(Uint64 FenceValue)
{
	m_CpuLinearAllocator.DiscardUsedPages(FenceValue);
	m_GpuLinearAllocator.DiscardUsedPages(FenceValue);
}

void CommandContext::DiscardUsedDescriptorHeaps(Uint64 FenceValue)
{
    for(size_t HeapType = 0; HeapType < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++HeapType)
        m_pDynamicGPUDescriptorAllocator[HeapType]->DiscardHeaps(FenceValue);
}

DescriptorHeapAllocation CommandContext::AllocateGPUVisibleDescriptor( D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT Count )
{
    VERIFY(Type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && Type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES, "Invalid heap type");
    return m_pDynamicGPUDescriptorAllocator[Type]->Allocate(Count);
}

#if 0

void CommandContext::FillBuffer( GpuResource& Dest, size_t DestOffset, DWParam Value, size_t NumBytes )
{
	DynAlloc TempSpace = m_CpuLinearAllocator.Allocate( NumBytes, 512 );
	__m128 VectorValue = _mm_set1_ps(Value.Float);
	SIMDMemFill(TempSpace.DataPtr, VectorValue, Math::DivideByMultiple(NumBytes, 16));
	CopyBufferRegion(Dest, DestOffset, TempSpace.Buffer, TempSpace.Offset, NumBytes );
}

void CommandContext::CopySubresource(GpuResource& Dest, UINT DestSubIndex, GpuResource& Src, UINT SrcSubIndex)
{
	// TODO:  Add a TransitionSubresource()?
	TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	FlushResourceBarriers();

	D3D12_TEXTURE_COPY_LOCATION DestLocation =
	{
		Dest.GetResource(),
		D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
		DestSubIndex
	};

	D3D12_TEXTURE_COPY_LOCATION SrcLocation =
	{
		Src.GetResource(),
		D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
		SrcSubIndex
	};

	m_pCommandList->CopyTextureRegion(&DestLocation, 0, 0, 0, &SrcLocation, nullptr);
}

void CommandContext::InitializeTextureArraySlice(GpuResource& Dest, UINT SliceIndex, GpuResource& Src)
{
	CommandContext& Context = CommandContext::Begin();

	Context.TransitionResource(Dest, D3D12_RESOURCE_STATE_COPY_DEST);
	Context.TransitionResource(Src, D3D12_RESOURCE_STATE_COPY_SOURCE);
	Context.FlushResourceBarriers();

	const D3D12_RESOURCE_DESC& DestDesc = Dest.GetResource()->GetDesc();
	const D3D12_RESOURCE_DESC& SrcDesc = Src.GetResource()->GetDesc();

	ASSERT(SliceIndex < DestDesc.DepthOrArraySize &&
		SrcDesc.DepthOrArraySize == 1 &&
		DestDesc.Width == SrcDesc.Width &&
		DestDesc.Height == SrcDesc.Height &&
		DestDesc.MipLevels <= SrcDesc.MipLevels
		);

	UINT SubResourceIndex = SliceIndex * DestDesc.MipLevels;

	for (UINT i = 0; i < DestDesc.MipLevels; ++i)
	{
		D3D12_TEXTURE_COPY_LOCATION destCopyLocation =
		{
			Dest.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			SubResourceIndex + i
		};

		D3D12_TEXTURE_COPY_LOCATION srcCopyLocation =
		{
			Src.GetResource(),
			D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			i
		};

		Context.m_pCommandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &srcCopyLocation, nullptr);
	}

	Context.CloseAndExecute();
}
#endif

}
