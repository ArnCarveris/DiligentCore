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
#include "PipelineStateD3D11Impl.h"
#include "RenderDeviceD3D11Impl.h"
#include "ShaderResourceBindingD3D11Impl.h"
#include "EngineMemory.h"

namespace Diligent
{

PipelineStateD3D11Impl::PipelineStateD3D11Impl(FixedBlockMemoryAllocator &PSOAllocator, RenderDeviceD3D11Impl *pRenderDeviceD3D11, const PipelineStateDesc& PipelineDesc) : 
    TPipelineStateBase(PSOAllocator, pRenderDeviceD3D11, PipelineDesc),
    m_pDefaultShaderResBinding( nullptr, STDDeleter<ShaderResourceBindingD3D11Impl, FixedBlockMemoryAllocator>(pRenderDeviceD3D11->GetSRBAllocator()) ),
    m_ResourceCacheDataAllocators
    {
        {STDDeleterRawMem<FixedBlockMemoryAllocator>(GetRawAllocator()), PipelineDesc.SRBAllocationGranularity},
        {STDDeleterRawMem<FixedBlockMemoryAllocator>(GetRawAllocator()), PipelineDesc.SRBAllocationGranularity},
        {STDDeleterRawMem<FixedBlockMemoryAllocator>(GetRawAllocator()), PipelineDesc.SRBAllocationGranularity},
        {STDDeleterRawMem<FixedBlockMemoryAllocator>(GetRawAllocator()), PipelineDesc.SRBAllocationGranularity},
        {STDDeleterRawMem<FixedBlockMemoryAllocator>(GetRawAllocator()), PipelineDesc.SRBAllocationGranularity}
    }
{
    if (PipelineDesc.IsComputePipeline)
    {
        m_pCS = ValidatedCast<ShaderD3D11Impl>( PipelineDesc.ComputePipeline.pCS );
        if (m_pCS == nullptr)
        {
            LOG_ERROR_AND_THROW("Compute shader is null");
        }

        if (m_pCS && m_pCS->GetDesc().ShaderType != SHADER_TYPE_COMPUTE)
        { 
            LOG_ERROR_AND_THROW( GetShaderTypeLiteralName(SHADER_TYPE_COMPUTE), " shader is expeceted while ", GetShaderTypeLiteralName(m_pCS->GetDesc().ShaderType)," provided" );
        } 
    }
    else
    {

#define INIT_SHADER(ShortName, ExpectedType)\
        {                                   \
            m_p##ShortName = ValidatedCast<ShaderD3D11Impl>( PipelineDesc.GraphicsPipeline.p##ShortName );  \
            if (m_p##ShortName && m_p##ShortName->GetDesc().ShaderType != ExpectedType)   \
            {   \
                LOG_ERROR_AND_THROW( GetShaderTypeLiteralName(ExpectedType), " shader is expeceted while ", GetShaderTypeLiteralName(m_p##ShortName->GetDesc().ShaderType)," provided" );   \
            }   \
        }

        INIT_SHADER(VS, SHADER_TYPE_VERTEX);
        INIT_SHADER(PS, SHADER_TYPE_PIXEL);
        INIT_SHADER(GS, SHADER_TYPE_GEOMETRY);
        INIT_SHADER(DS, SHADER_TYPE_DOMAIN);
        INIT_SHADER(HS, SHADER_TYPE_HULL);
#undef INIT_SHADER

        if (m_pVS == nullptr)
        {
            LOG_ERROR_AND_THROW("Vertex shader is null");
        }

        auto *pDeviceD3D11 = pRenderDeviceD3D11->GetD3D11Device();
        D3D11_BLEND_DESC D3D11BSDesc = {};
        BlendStateDesc_To_D3D11_BLEND_DESC(PipelineDesc.GraphicsPipeline.BlendDesc, D3D11BSDesc);
        CHECK_D3D_RESULT_THROW( pDeviceD3D11->CreateBlendState( &D3D11BSDesc, &m_pd3d11BlendState ), 
                                "Failed to create D3D11 blend state object" );

        D3D11_RASTERIZER_DESC D3D11RSDesc = {};
        RasterizerStateDesc_To_D3D11_RASTERIZER_DESC(PipelineDesc.GraphicsPipeline.RasterizerDesc, D3D11RSDesc);
        CHECK_D3D_RESULT_THROW( pDeviceD3D11->CreateRasterizerState( &D3D11RSDesc, &m_pd3d11RasterizerState ),
                                "Failed to create D3D11 rasterizer state" );

        D3D11_DEPTH_STENCIL_DESC D3D11DSSDesc = {};
        DepthStencilStateDesc_To_D3D11_DEPTH_STENCIL_DESC(PipelineDesc.GraphicsPipeline.DepthStencilDesc, D3D11DSSDesc);
        CHECK_D3D_RESULT_THROW( pDeviceD3D11->CreateDepthStencilState( &D3D11DSSDesc, &m_pd3d11DepthStencilState ),
                                "Failed to create D3D11 depth stencil state" );

   
        // Create input layout
        if( m_LayoutElements.size() > 0 ) 
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC, STDAllocatorRawMem<D3D11_INPUT_ELEMENT_DESC> > d311InputElements(STD_ALLOCATOR_RAW_MEM(D3D11_INPUT_ELEMENT_DESC, GetRawAllocator(), "Allocator for vector<D3D11_INPUT_ELEMENT_DESC>") );
            LayoutElements_To_D3D11_INPUT_ELEMENT_DESCs(m_LayoutElements, d311InputElements);

            ID3DBlob *pVSByteCode = ValidatedCast<ShaderD3D11Impl>(m_pVS.RawPtr())->GetBytecode();
            if( !pVSByteCode )
                LOG_ERROR_AND_THROW( "Vertex Shader byte code does not exist" );

            CHECK_D3D_RESULT_THROW( pDeviceD3D11->CreateInputLayout(d311InputElements.data(), static_cast<UINT>(d311InputElements.size()), pVSByteCode->GetBufferPointer(), pVSByteCode->GetBufferSize(), &m_pd3d11InputLayout ),
                                    "Failed to create the Direct3D11 input layout");
        }
    }

    auto &SRBAllocator = pRenderDeviceD3D11->GetSRBAllocator();
    m_pDefaultShaderResBinding.reset( NEW(SRBAllocator, "ShaderResourceBindingD3D11Impl instance", ShaderResourceBindingD3D11Impl, this, true) );
}

PipelineStateD3D11Impl::~PipelineStateD3D11Impl()
{

}

IMPLEMENT_QUERY_INTERFACE( PipelineStateD3D11Impl, IID_PipelineStateD3D11, TPipelineStateBase )


ID3D11BlendState* PipelineStateD3D11Impl::GetD3D11BlendState()
{
    return m_pd3d11BlendState;
}

ID3D11RasterizerState* PipelineStateD3D11Impl::GetD3D11RasterizerState()
{
    return m_pd3d11RasterizerState;
}

ID3D11DepthStencilState* PipelineStateD3D11Impl::GetD3D11DepthStencilState()
{
    return m_pd3d11DepthStencilState;
}

ID3D11InputLayout* PipelineStateD3D11Impl::GetD3D11InputLayout()
{
    return m_pd3d11InputLayout;
}

void PipelineStateD3D11Impl::BindShaderResources(IResourceMapping *pResourceMapping, Uint32 Flags)
{
    if( m_Desc.IsComputePipeline )
    { 
        if(m_pCS)m_pCS->BindResources(pResourceMapping, Flags);
    }
    else
    {
        if(m_pVS)m_pVS->BindResources(pResourceMapping, Flags);
        if(m_pPS)m_pPS->BindResources(pResourceMapping, Flags);
        if(m_pGS)m_pGS->BindResources(pResourceMapping, Flags);
        if(m_pDS)m_pDS->BindResources(pResourceMapping, Flags);
        if(m_pHS)m_pHS->BindResources(pResourceMapping, Flags);
    }
}

void PipelineStateD3D11Impl::CreateShaderResourceBinding(IShaderResourceBinding **ppShaderResourceBinding)
{
    auto *pRenderDeviceD3D11 = ValidatedCast<RenderDeviceD3D11Impl>( GetDevice() );
    auto &SRBAllocator = pRenderDeviceD3D11->GetSRBAllocator();
    auto pShaderResBinding = NEW(SRBAllocator, "ShaderResourceBindingD3D11Impl instance", ShaderResourceBindingD3D11Impl, this, false);
    pShaderResBinding->QueryInterface(IID_ShaderResourceBinding, reinterpret_cast<IObject**>(static_cast<IShaderResourceBinding**>(ppShaderResourceBinding)));
}


ID3D11VertexShader* PipelineStateD3D11Impl::GetD3D11VertexShader()
{
    if(!m_pVS)return nullptr;
    auto *pVSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pVS.RawPtr());
    return static_cast<ID3D11VertexShader*>(pVSD3D11->GetD3D11Shader());
}

ID3D11PixelShader* PipelineStateD3D11Impl::GetD3D11PixelShader()
{
    if(!m_pPS)return nullptr;
    auto *pPSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pPS.RawPtr());
    return static_cast<ID3D11PixelShader*>(pPSD3D11->GetD3D11Shader());
}

ID3D11GeometryShader* PipelineStateD3D11Impl::GetD3D11GeometryShader()
{
    if(!m_pGS)return nullptr;
    auto *pGSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pGS.RawPtr());
    return static_cast<ID3D11GeometryShader*>(pGSD3D11->GetD3D11Shader());
}

ID3D11DomainShader* PipelineStateD3D11Impl::GetD3D11DomainShader()
{
    if(!m_pDS)return nullptr;
    auto *pDSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pDS.RawPtr());
    return static_cast<ID3D11DomainShader*>(pDSD3D11->GetD3D11Shader());
}

ID3D11HullShader* PipelineStateD3D11Impl::GetD3D11HullShader()
{
    if(!m_pHS)return nullptr;
    auto *pHSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pHS.RawPtr());
    return static_cast<ID3D11HullShader*>(pHSD3D11->GetD3D11Shader());
}

ID3D11ComputeShader* PipelineStateD3D11Impl::GetD3D11ComputeShader()
{
    if(!m_pCS)return nullptr;
    auto *pCSD3D11 = ValidatedCast<ShaderD3D11Impl>(m_pCS.RawPtr());
    return static_cast<ID3D11ComputeShader*>(pCSD3D11->GetD3D11Shader());
}

}
