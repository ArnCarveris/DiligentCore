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

#include "pch.h"
#include "SwapChainD3D12Impl.h"
#include "RenderDeviceD3D12Impl.h"
#include "DeviceContextD3D12Impl.h"
#include "DXGITypeConversions.h"
#include "TextureD3D12Impl.h"
#include "EngineMemory.h"

namespace Diligent
{

SwapChainD3D12Impl::SwapChainD3D12Impl(IMemoryAllocator &Allocator,
                                       const SwapChainDesc& SCDesc, 
                                       RenderDeviceD3D12Impl* pRenderDeviceD3D12, 
                                       DeviceContextD3D12Impl* pDeviceContextD3D12, 
                                       void* pNativeWndHandle) : 
    TSwapChainBase(Allocator, pRenderDeviceD3D12, pDeviceContextD3D12, SCDesc)
{

#ifdef PLATFORM_WINDOWS
    auto hWnd = reinterpret_cast<HWND>(pNativeWndHandle);

    if( m_SwapChainDesc.Width == 0 || m_SwapChainDesc.Height == 0 )
    {
        RECT rc;
        GetClientRect( hWnd, &rc );
        m_SwapChainDesc.Width = rc.right - rc.left;
        m_SwapChainDesc.Height = rc.bottom - rc.top;
    }
#endif

    auto DXGIColorBuffFmt = TexFormatToDXGI_Format(m_SwapChainDesc.ColorBufferFormat);

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_SwapChainDesc.Width;
    swapChainDesc.Height = m_SwapChainDesc.Height;
    //  Flip model swapchains (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD) only support the following Formats: 
    //  - DXGI_FORMAT_R16G16B16A16_FLOAT 
    //  - DXGI_FORMAT_B8G8R8A8_UNORM
    //  - DXGI_FORMAT_R8G8B8A8_UNORM
    //  - DXGI_FORMAT_R10G10B10A2_UNORM
    // If RGBA8_UNORM_SRGB swap chain is required, we will create RGBA8_UNORM swap chain, but
    // create RGBA8_UNORM_SRGB render target view
    swapChainDesc.Format = DXGIColorBuffFmt == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGIColorBuffFmt;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = m_SwapChainDesc.BufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // Not used
    swapChainDesc.Flags = 0;

    CComPtr<IDXGISwapChain1> pSwapChain1;
#if defined( PLATFORM_WINDOWS )

	CComPtr<IDXGIFactory4> factory;
    HRESULT hr = CreateDXGIFactory1(__uuidof(factory), reinterpret_cast<void**>(static_cast<IDXGIFactory4**>(&factory)) );
    CHECK_D3D_RESULT_THROW(hr, "Failed to create DXGI factory")

    auto *pd3d12CmdQueue = pRenderDeviceD3D12->GetCmdQueue();
	hr = factory->CreateSwapChainForHwnd(pd3d12CmdQueue, hWnd, &swapChainDesc, nullptr, nullptr, &pSwapChain1);
    CHECK_D3D_RESULT_THROW( hr, "Failed to create Swap Chain" );

	// This sample does not support fullscreen transitions.
	hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

#elif defined( PLATFORM_WINDOWS_STORE )

	// Describe and create the swap chain.
    auto *pDevice = pRenderDeviceD3D12->GetD3D12Device();

	CComPtr<IDXGIDevice3> pDXGIDevice;
	pDevice->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(static_cast<IDXGIDevice3**>(&pDXGIDevice)));
    CComPtr<IDXGIAdapter> pDXGIAdapter;
    pDXGIDevice->GetAdapter(&pDXGIAdapter);
	CComPtr<IDXGIFactory2> pDXGIFactory;
    pDXGIAdapter->GetParent(__uuidof(pDXGIFactory), reinterpret_cast<void**>(static_cast<IDXGIFactory2**>(&pDXGIFactory)));

    HRESULT hr = pDXGIFactory->CreateSwapChainForCoreWindow(
		pDevice,
		reinterpret_cast<IUnknown*>(pNativeWndHandle),
		&swapChainDesc,
		nullptr,
		&pSwapChain1);
    CHECK_D3D_RESULT_THROW( hr, "Failed to create DXGI swap chain" );

	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
	// ensures that the application will only render after each VSync, minimizing power consumption.
    pDXGIDevice->SetMaximumFrameLatency( 1 );

#endif

    pSwapChain1->QueryInterface(__uuidof(m_pSwapChain), reinterpret_cast<void**>( static_cast<IDXGISwapChain3**>(&m_pSwapChain) ));

    InitBuffersAndViews();
}

SwapChainD3D12Impl::~SwapChainD3D12Impl()
{

}

void SwapChainD3D12Impl::InitBuffersAndViews()
{
    m_pBackBufferRTV.resize(m_SwapChainDesc.BufferCount);
    for(Uint32 backbuff = 0; backbuff < m_SwapChainDesc.BufferCount; ++backbuff)
    {
		CComPtr<ID3D12Resource> pBackBuffer;
        auto hr = m_pSwapChain->GetBuffer(backbuff, __uuidof(pBackBuffer), reinterpret_cast<void**>( static_cast<ID3D12Resource**>(&pBackBuffer) ));
        if(FAILED(hr))
            LOG_ERROR_AND_THROW("Failed to get back buffer ", backbuff," from the swap chain");

        TextureDesc BackBufferDesc;
        BackBufferDesc.Format = m_SwapChainDesc.ColorBufferFormat;
        String Name = "Main back buffer ";
        Name += std::to_string(backbuff);
        BackBufferDesc.Name = Name.c_str();

        RefCntAutoPtr<TextureD3D12Impl> pBackBufferTex;
        ValidatedCast<RenderDeviceD3D12Impl>(m_pRenderDevice.RawPtr())->CreateTexture(BackBufferDesc, pBackBuffer, &pBackBufferTex);
        TextureViewDesc RTVDesc;
        RTVDesc.ViewType = TEXTURE_VIEW_RENDER_TARGET;
        RefCntAutoPtr<ITextureView> pRTV;
        pBackBufferTex->CreateView(RTVDesc, &pRTV);
        m_pBackBufferRTV[backbuff] = pRTV;
    }

    TextureDesc DepthBufferDesc;
    DepthBufferDesc.Type = RESOURCE_DIM_TEX_2D;
    DepthBufferDesc.Width = m_SwapChainDesc.Width;
    DepthBufferDesc.Height = m_SwapChainDesc.Height;
    DepthBufferDesc.Format = m_SwapChainDesc.DepthBufferFormat;
    DepthBufferDesc.SampleCount = m_SwapChainDesc.SamplesCount;
    DepthBufferDesc.Usage = USAGE_DEFAULT;
    DepthBufferDesc.BindFlags = BIND_DEPTH_STENCIL;

    DepthBufferDesc.ClearValue.Format = DepthBufferDesc.Format;
    DepthBufferDesc.ClearValue.DepthStencil.Depth = m_SwapChainDesc.DefaultDepthValue;
    DepthBufferDesc.ClearValue.DepthStencil.Stencil = m_SwapChainDesc.DefaultStencilValue;
    String Name = "Main depth buffer";
    RefCntAutoPtr<ITexture> pDepthBufferTex;
    m_pRenderDevice->CreateTexture(DepthBufferDesc, TextureData(), static_cast<ITexture**>(&pDepthBufferTex) );
    m_pDepthBufferDSV = pDepthBufferTex->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);
}

IMPLEMENT_QUERY_INTERFACE( SwapChainD3D12Impl, IID_SwapChainD3D12, TSwapChainBase )


void SwapChainD3D12Impl::Present()
{
    UINT SyncInterval = 0;
#ifdef PLATFORM_WINDOWS_STORE
    SyncInterval = 1; // Interval 0 is not supported on Windows Phone 
#endif

    auto pDeviceContext = m_wpDeviceContext.Lock();
    if( !pDeviceContext )
    {
        LOG_ERROR_MESSAGE( "Immediate context has been released" );
        return;
    }

    auto *pImmediateCtx = pDeviceContext.RawPtr();
    auto *pImmediateCtxD3D12 = ValidatedCast<DeviceContextD3D12Impl>( pImmediateCtx );

    auto *pCmdCtx = pImmediateCtxD3D12->RequestCmdContext();
    auto *pBackBuffer = ValidatedCast<TextureD3D12Impl>( GetCurrentBackBufferRTV()->GetTexture() );
    pCmdCtx->TransitionResource( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT);

    pImmediateCtxD3D12->Flush();

    auto hr = m_pSwapChain->Present( SyncInterval, 0 );
    VERIFY(SUCCEEDED(hr), "Present failed");

#if 0
#ifdef PLATFORM_WINDOWS_STORE
    // A successful Present call for DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL SwapChains unbinds 
    // backbuffer 0 from all GPU writeable bind points.
    // We need to rebind all render targets to make sure that
    // the back buffer is not unbound
    pImmediateCtxD3D12->RebindRenderTargets();
#endif
#endif
}

void SwapChainD3D12Impl::Resize( Uint32 NewWidth, Uint32 NewHeight )
{
    if( TSwapChainBase::Resize(NewWidth, NewHeight) )
    {
        auto pDeviceContext = m_wpDeviceContext.Lock();
        VERIFY( pDeviceContext, "Immediate context has been released" );
        if( pDeviceContext )
        {
            RenderDeviceD3D12Impl *pDeviceD3D12 = ValidatedCast<RenderDeviceD3D12Impl>(m_pRenderDevice.RawPtr());
            pDeviceContext->Flush();

            try
            {
                auto *pImmediateCtxD3D12 = ValidatedCast<DeviceContextD3D12Impl>(pDeviceContext.RawPtr());
                bool bIsDefaultFBBound = pImmediateCtxD3D12->IsDefaultFBBound();

                // All references to the swap chain must be released before it can be resized
                m_pBackBufferRTV.clear();
                m_pDepthBufferDSV.Release();

                // This will release references to D3D12 swap chain buffers hold by
                // m_pBackBufferRTV[]
                pDeviceD3D12->IdleGPU();

                DXGI_SWAP_CHAIN_DESC SCDes;
                memset( &SCDes, 0, sizeof( SCDes ) );
                m_pSwapChain->GetDesc( &SCDes );
                CHECK_D3D_RESULT_THROW( m_pSwapChain->ResizeBuffers(SCDes.BufferCount, m_SwapChainDesc.Width, 
                                                                    m_SwapChainDesc.Height, SCDes.BufferDesc.Format, 
                                                                    SCDes.Flags),
                                        "Failed to resize the DXGI swap chain" );


                InitBuffersAndViews();
                
                if( bIsDefaultFBBound )
                {
                    // Set default render target and viewport
                    pDeviceContext->SetRenderTargets( 0, nullptr, nullptr );
                    pDeviceContext->SetViewports( 1, nullptr, 0, 0 );
                }
            }
            catch( const std::runtime_error & )
            {
                LOG_ERROR( "Failed to resize the swap chain" );
            }
        }
    }
}

ITextureView *SwapChainD3D12Impl::GetCurrentBackBufferRTV()
{
    auto CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
    VERIFY_EXPR(CurrentBackBufferIndex>=0 && CurrentBackBufferIndex < m_SwapChainDesc.BufferCount);
    return m_pBackBufferRTV[CurrentBackBufferIndex];
}

}
