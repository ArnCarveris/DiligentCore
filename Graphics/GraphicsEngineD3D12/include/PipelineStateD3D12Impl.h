/*     Copyright 2015-2017 Egor Yusov
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

#pragma once

/// \file
/// Declaration of Diligent::PipelineStateD3D12Impl class

#include "RenderDeviceD3D12.h"
#include "PipelineStateD3D12.h"
#include "PipelineStateBase.h"
#include "RootSignature.h"
#include "ShaderResourceLayoutD3D12.h"
#include "AdaptiveFixedBlockAllocator.h"

/// Namespace for the Direct3D11 implementation of the graphics engine
namespace Diligent
{

class FixedBlockMemoryAllocator;
/// Implementation of the Diligent::IRenderDeviceD3D12 interface
class PipelineStateD3D12Impl : public PipelineStateBase<IPipelineStateD3D12, IRenderDeviceD3D12>
{
public:
    typedef PipelineStateBase<IPipelineStateD3D12, IRenderDeviceD3D12> TPipelineStateBase;

    PipelineStateD3D12Impl( IReferenceCounters *pRefCounters, class RenderDeviceD3D12Impl *pDeviceD3D12, const PipelineStateDesc &PipelineDesc );
    ~PipelineStateD3D12Impl();

    virtual void QueryInterface( const Diligent::INTERFACE_ID &IID, IObject **ppInterface );
   
    virtual ID3D12PipelineState *GetD3D12PipelineState()const override final{return m_pd3d12PSO;}
    
    virtual void BindShaderResources( IResourceMapping *pResourceMapping, Uint32 Flags )override;

    virtual void CreateShaderResourceBinding( IShaderResourceBinding **ppShaderResourceBinding )override;

    virtual ID3D12RootSignature *GetD3D12RootSignature()const override final{return m_RootSig.GetD3D12RootSignature(); }

    ShaderResourceCacheD3D12* CommitAndTransitionShaderResources(IShaderResourceBinding *pShaderResourceBinding, 
                                                                 class CommandContext &Ctx,
                                                                 bool CommitResources,
                                                                 bool TransitionResources)const;
    
    const RootSignature& GetRootSignature()const{return m_RootSig;}
    
    const ShaderResourceLayoutD3D12& GetShaderResLayout(SHADER_TYPE ShaderType)const;
    
    bool dbgContainsShaderResources()const;

    IMemoryAllocator &GetResourceCacheDataAllocator(){return m_ResourceCacheDataAllocator;}
    IMemoryAllocator &GetShaderResourceLayoutDataAllocator(Uint32 ActiveShaderInd)
    {
        VERIFY_EXPR(ActiveShaderInd < m_NumShaders);
        auto *pAllocator = m_ResLayoutDataAllocators.GetAllocator(ActiveShaderInd);
        return pAllocator != nullptr ? *pAllocator : GetRawAllocator();
    }

    IShaderVariable *GetDummyShaderVar(){return &m_DummyVar;}

private:

    void ParseShaderResourceLayout(IShader *pShader);

    /// D3D12 device
    CComPtr<ID3D12PipelineState> m_pd3d12PSO;
    RootSignature m_RootSig;
    DummyShaderVariable m_DummyVar;
    
    // Looks like there may be a bug in msvc: when allocators are declared as 
    // an array and if an exception is thrown from constructor, the app crashes
    class ResLayoutDataAllocators
    {
    public:
        ~ResLayoutDataAllocators()
        {
            for(size_t i=0; i < _countof(m_pAllocators); ++i)
                if(m_pAllocators[i] != nullptr)
                    DESTROY_POOL_OBJECT(m_pAllocators[i]);
        }
        void Init(Uint32 NumActiveShaders, Uint32 SRBAllocationGranularity)
        {
            VERIFY_EXPR(NumActiveShaders <= _countof(m_pAllocators) );
            for(Uint32 i=0; i < NumActiveShaders; ++i)
                m_pAllocators[i] = NEW_POOL_OBJECT(AdaptiveFixedBlockAllocator, "Shader resource layout data allocator", GetRawAllocator(), SRBAllocationGranularity);
        }
        AdaptiveFixedBlockAllocator *GetAllocator(Uint32 ActiveShaderInd)
        {
            VERIFY_EXPR(ActiveShaderInd < _countof(m_pAllocators) );
            return m_pAllocators[ActiveShaderInd];
        }
    private:
        AdaptiveFixedBlockAllocator *m_pAllocators[5] = {};
    }m_ResLayoutDataAllocators; // Allocators must be defined before default SRB

    ShaderResourceLayoutD3D12* m_pShaderResourceLayouts[6] = {};
    AdaptiveFixedBlockAllocator m_ResourceCacheDataAllocator; // Use separate allocator for every shader stage

    // Do not use strong reference to avoid cyclic references
    // Default SRB must be defined after allocators
    std::unique_ptr<class ShaderResourceBindingD3D12Impl, STDDeleter<ShaderResourceBindingD3D12Impl, FixedBlockMemoryAllocator> > m_pDefaultShaderResBinding;
};

}
