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
#include "PipelineStateGLImpl.h"
#include "RenderDeviceGLImpl.h"
#include "ShaderGLImpl.h"
#include "ShaderResourceBindingGLImpl.h"
#include "EngineMemory.h"

namespace Diligent
{

PipelineStateGLImpl::PipelineStateGLImpl(FixedBlockMemoryAllocator &PSOAllocator, class RenderDeviceGLImpl *pDeviceGL, const PipelineStateDesc& PipelineDesc, bool bIsDeviceInternal) : 
    TPipelineStateBase(PSOAllocator, pDeviceGL, PipelineDesc, bIsDeviceInternal),
    m_GLProgram(false),
    m_GLProgPipeline(false)
{

    auto &DeviceCaps = pDeviceGL->GetDeviceCaps();
    VERIFY( DeviceCaps.DevType != DeviceType::Undefined, "Device caps are not initialized" );
    bool bIsProgramPipelineSupported = DeviceCaps.bSeparableProgramSupported;

    LinkGLProgram(bIsProgramPipelineSupported);
}

void PipelineStateGLImpl::LinkGLProgram(bool bIsProgramPipelineSupported)
{
    if( bIsProgramPipelineSupported )
    {
        // Create new progam pipeline
        m_GLProgPipeline.Create();
        for( Uint32 Shader = 0; Shader < m_NumShaders; ++Shader )
        {
            auto *pCurrShader = static_cast<ShaderGLImpl*>(m_ppShaders[Shader]);
            auto GLShaderBit = ShaderTypeToGLShaderBit( pCurrShader->GetDesc().ShaderType );
            // If the program has an active code for each stage mentioned in set flags,
            // then that code will be used by the pipeline. If program is 0, then the given
            // stages are cleared from the pipeline.
            glUseProgramStages( m_GLProgPipeline, GLShaderBit, pCurrShader->m_GlProgObj );
            CHECK_GL_ERROR( "glUseProgramStages() failed" );
        }
    }
    else
    {
        // Create new progam
        m_GLProgram.Create();
        for( Uint32 Shader = 0; Shader < m_NumShaders; ++Shader )
        {
            auto *pCurrShader = static_cast<ShaderGLImpl*>(m_ppShaders[Shader]);
            glAttachShader( m_GLProgram, pCurrShader->m_GLShaderObj );
            CHECK_GL_ERROR( "glAttachShader() failed" );
        }
        glLinkProgram( m_GLProgram );
        CHECK_GL_ERROR( "glLinkProgram() failed" );
        int IsLinked = GL_FALSE;
        glGetProgramiv( m_GLProgram, GL_LINK_STATUS, (int *)&IsLinked );
        CHECK_GL_ERROR( "glGetProgramiv() failed" );
        if( !IsLinked )
        {
            int LengthWithNull = 0, Length = 0;
            // Notice that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv.
            // The length of the info log includes a null terminator.
            glGetProgramiv( m_GLProgram, GL_INFO_LOG_LENGTH, &LengthWithNull );

            // The maxLength includes the NULL character
            std::vector<char> shaderProgramInfoLog( LengthWithNull );

            // Notice that glGetProgramInfoLog  is used, not glGetShaderInfoLog.
            glGetProgramInfoLog( m_GLProgram, LengthWithNull, &Length, &shaderProgramInfoLog[0] );
            VERIFY( Length == LengthWithNull-1, "Incorrect program info log len" );
            LOG_ERROR_MESSAGE( "Failed to link shader program:\n", &shaderProgramInfoLog[0], '\n');
            UNEXPECTED( "glLinkProgram failed" );
        }
            
        // Detach shaders from the program object
        for( Uint32 Shader = 0; Shader < m_NumShaders; ++Shader )
        {
            auto *pCurrShader = static_cast<ShaderGLImpl*>(m_ppShaders[Shader]);
            glDetachShader( m_GLProgram, pCurrShader->m_GLShaderObj );
            CHECK_GL_ERROR( "glDetachShader() failed" );
        }

        std::vector<ShaderVariableDesc> MergedVarTypesArray;
        SHADER_VARIABLE_TYPE DefaultVarType = SHADER_VARIABLE_TYPE_STATIC;
        for( Uint32 Shader = 0; Shader < m_NumShaders; ++Shader )
        {
            auto *pCurrShader = static_cast<ShaderGLImpl*>(m_ppShaders[Shader]);
            const auto& Desc = pCurrShader->GetDesc();
            if (Shader == 0)
            {
                DefaultVarType = Desc.DefaultVariableType;
                for (Uint32 v = 0; v < Desc.NumVariables; ++v)
                {
                    MergedVarTypesArray.push_back(Desc.VariableDesc[v]);
                }
            }
            else
            {
                if (DefaultVarType != Desc.DefaultVariableType)
                {
                    LOG_ERROR("Inconsistent default variable types for shaders in one program");
                }
            }
        }

        m_GLProgram.InitResources(DefaultVarType, MergedVarTypesArray.data(), static_cast<Uint32>(MergedVarTypesArray.size()), this);
    }   
}


PipelineStateGLImpl::~PipelineStateGLImpl()
{
    static_cast<RenderDeviceGLImpl*>( GetDevice() )->m_VAOCache.OnDestroyPSO(this);
}

IMPLEMENT_QUERY_INTERFACE( PipelineStateGLImpl, IID_PipelineStateGL, TPipelineStateBase )

void PipelineStateGLImpl::BindShaderResources(IResourceMapping *pResourceMapping, Uint32 Flags)
{
    if( GetDevice()->GetDeviceCaps().bSeparableProgramSupported )
    {
        for( Uint32 s = 0; s < m_NumShaders; ++s )
        {
            m_ppShaders[s]->BindResources( pResourceMapping, Flags );
        }
    }
    else
    {
        if( m_GLProgram )
            m_GLProgram.BindConstantResources( pResourceMapping, Flags );
    }
}

void PipelineStateGLImpl::CreateShaderResourceBinding(IShaderResourceBinding **ppShaderResourceBinding)
{
    auto *pRenderDeviceGL = ValidatedCast<RenderDeviceGLImpl>( GetDevice() );
    auto &SRBAllocator = pRenderDeviceGL->GetSRBAllocator();
    auto pResBinding = NEW( SRBAllocator, "ShaderResourceBindingGLImpl instance", ShaderResourceBindingGLImpl, this);
    pResBinding->QueryInterface(IID_ShaderResourceBinding, reinterpret_cast<IObject**>(ppShaderResourceBinding));
}

}
