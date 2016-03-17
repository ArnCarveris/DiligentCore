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

#pragma once

/// \file
/// Implementation of the Diligent::PipelineStateBase template class

#include "PipelineState.h"
#include "DeviceObjectBase.h"
#include "STDAllocator.h"

namespace Diligent
{

/// Template class implementing base functionality for a blend state object.

/// \tparam BaseInterface - base interface that this class will inheret 
///                         (Diligent::IPipelineStateD3D11, Diligent::IPipelineStateD3D12 or Diligent::IPipelineStateGL).
/// \tparam RenderDeviceBaseInterface - base interface for the render device
///                                     (Diligent::IRenderDeviceD3D11, Diligent::IRenderDeviceD3D12, Diligent::IRenderDeviceGL,
///                                      or Diligent::IRenderDeviceGLES).
template<class BaseInterface, class RenderDeviceBaseInterface, class PSOAllocator>
class PipelineStateBase : public DeviceObjectBase<BaseInterface, PipelineStateDesc, PSOAllocator>
{
public:
    typedef DeviceObjectBase<BaseInterface, PipelineStateDesc, PSOAllocator> TDeviceObjectBase;
    typedef RenderDeviceBase < RenderDeviceBaseInterface > TRenderDeviceBase;

	/// \param pDevice - pointer to the device.
	/// \param BSDesc - blend state description.
	/// \param bIsDeviceInternal - flag indicating if the blend state is an internal device object and 
	///							   must not keep a strong reference to the device.
    PipelineStateBase( PSOAllocator &ObjAllocator, IRenderDevice *pDevice, const PipelineStateDesc& PSODesc, bool bIsDeviceInternal = false ) :
        TDeviceObjectBase( ObjAllocator, pDevice, PSODesc, nullptr, bIsDeviceInternal ),
        m_LayoutElements( PSODesc.GraphicsPipeline.InputLayout.NumElements, LayoutElement(), STD_ALLOCATOR_RAW_MEM(LayoutElement, GetRawAllocator(), "Allocator for vector<LayoutElement>" ) ),
        m_NumShaders(0)
    {
        memset(m_ppShaders, 0, sizeof(m_ppShaders));

        if (m_Desc.IsComputePipeline)
        {
            const auto &ComputePipeline = PSODesc.ComputePipeline;
            if (ComputePipeline.pCS == nullptr)
            {
                LOG_ERROR_AND_THROW( "Compute shader is not provided" );
            }

#define VALIDATE_SHADER_TYPE(Shader, ExpectedType, ShaderName)\
            if (Shader && Shader->GetDesc().ShaderType != ExpectedType)   \
            {                                                   \
                LOG_ERROR_AND_THROW( GetShaderTypeLiteralName(Shader->GetDesc().ShaderType), " is not valid type for ", ShaderName, " shader" );\
            }
            VALIDATE_SHADER_TYPE(ComputePipeline.pCS, SHADER_TYPE_COMPUTE, "compute")

            m_pCS = ComputePipeline.pCS;
            m_ppShaders[0] = ComputePipeline.pCS;
            m_NumShaders = 1;
        }
        else
        {
            const auto &GraphicsPipeline = PSODesc.GraphicsPipeline;

            VALIDATE_SHADER_TYPE(GraphicsPipeline.pVS, SHADER_TYPE_VERTEX,   "vertex")
            VALIDATE_SHADER_TYPE(GraphicsPipeline.pPS, SHADER_TYPE_PIXEL,    "pixel")
            VALIDATE_SHADER_TYPE(GraphicsPipeline.pGS, SHADER_TYPE_GEOMETRY, "geometry")
            VALIDATE_SHADER_TYPE(GraphicsPipeline.pHS, SHADER_TYPE_HULL,     "hull")
            VALIDATE_SHADER_TYPE(GraphicsPipeline.pDS, SHADER_TYPE_DOMAIN,   "domain")
#undef VALIDATE_SHADER_TYPE

            m_pVS = GraphicsPipeline.pVS;
            m_pPS = GraphicsPipeline.pPS;
            m_pGS = GraphicsPipeline.pGS;
            m_pDS = GraphicsPipeline.pDS;
            m_pHS = GraphicsPipeline.pHS;

            if( GraphicsPipeline.pVS )m_ppShaders[m_NumShaders++] = GraphicsPipeline.pVS;
            if( GraphicsPipeline.pPS )m_ppShaders[m_NumShaders++] = GraphicsPipeline.pPS;
            if( GraphicsPipeline.pGS )m_ppShaders[m_NumShaders++] = GraphicsPipeline.pGS;
            if( GraphicsPipeline.pHS )m_ppShaders[m_NumShaders++] = GraphicsPipeline.pHS;
            if( GraphicsPipeline.pDS )m_ppShaders[m_NumShaders++] = GraphicsPipeline.pDS;
        }

        const auto &InputLayout = PSODesc.GraphicsPipeline.InputLayout;
        for( size_t Elem = 0; Elem < InputLayout.NumElements; ++Elem )
            m_LayoutElements[Elem] = InputLayout.LayoutElements[Elem];
        this->m_Desc.GraphicsPipeline.InputLayout.LayoutElements = m_LayoutElements.data();
        
        // Correct description and compute offsets and tight strides
        for( auto It = m_LayoutElements.begin(); It != m_LayoutElements.end(); ++It )
        {
            if( It->ValueType == VT_FLOAT32 || It->ValueType == VT_FLOAT16 )
                It->IsNormalized = false; // Floating point values cannot be normalized

            auto BuffSlot = It->BufferSlot;
            if( BuffSlot >= _countof(m_TightStrides) )
            {
                UNEXPECTED("Buffer slot (", BuffSlot, ") exceeds the limit (", _countof(m_TightStrides), ")");
                continue;
            }

            auto &CurrStride = m_TightStrides[BuffSlot];
            if( It->RelativeOffset < CurrStride )
            {
                if( It->RelativeOffset == 0 )
                    It->RelativeOffset = CurrStride;
                else
                    UNEXPECTED( "Overlapping layout elements" );
            }

            CurrStride += It->NumComponents * GetValueSize( It->ValueType );
        }
    }

    ~PipelineStateBase()
    {
        /*
        /// \note Destructor cannot directly remove the object from the registry as this may cause a  
        ///       deadlock at the point where StateObjectsRegistry::Find() locks the weak pointer: if we
        ///       are in dtor, the object is locked by Diligent::RefCountedObject::Release() and 
        ///       StateObjectsRegistry::Find() will wait for that lock to be released.
        ///       A the same time this thread will be waiting for the other thread to unlock the registry.\n
        ///       Thus destructor only notifies the registry that there is a deleted object.
        ///       The reference to the object will be removed later.
        auto &PipelineStateRegistry = static_cast<TRenderDeviceBase*>(this->GetDevice())->GetBSRegistry();
        auto &RasterizerStateRegistry = static_cast<TRenderDeviceBase*>(this->GetDevice())->GetRSRegistry();
        auto &DSSRegistry = static_cast<TRenderDeviceBase*>(this->GetDevice())->GetDSSRegistry();
        // StateObjectsRegistry::ReportDeletedObject() does not lock the registry, but only 
        // atomically increments the outstanding deleted objects counter.
        PipelineStateRegistry.ReportDeletedObject();
        RasterizerStateRegistry.ReportDeletedObject();
        DSSRegistry.ReportDeletedObject();
        */
    }

    IMPLEMENT_QUERY_INTERFACE_IN_PLACE( IID_PipelineState, TDeviceObjectBase )

    virtual const Uint32* GetTightStrides()
    { 
        return m_TightStrides;
    }

    IShader* GetVS(){return m_pVS;}
    IShader* GetPS(){return m_pPS;}
    IShader* GetGS(){return m_pGS;}
    IShader* GetDS(){return m_pDS;}
    IShader* GetHS(){return m_pHS;}
    IShader* GetCS(){return m_pCS;}

    IShader **GetShaders(){return m_ppShaders;}
    Uint32 GetNumShaders(){return m_NumShaders;}

protected:
    std::vector<LayoutElement, STDAllocatorRawMem<LayoutElement> > m_LayoutElements;

    // The size of this array must be equal to the
    // maximum number of buffer slots, because a layout 
    // element can refer to any input slot
    Uint32 m_TightStrides[MaxBufferSlots] = {};

    RefCntAutoPtr<IShader> m_pVS;
    RefCntAutoPtr<IShader> m_pPS;
    RefCntAutoPtr<IShader> m_pGS;
    RefCntAutoPtr<IShader> m_pDS;
    RefCntAutoPtr<IShader> m_pHS;
    RefCntAutoPtr<IShader> m_pCS;
    IShader *m_ppShaders[5];
    Uint32 m_NumShaders;
};

}
