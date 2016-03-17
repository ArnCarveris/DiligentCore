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

#include "ShaderD3D11Impl.h"
#include "RenderDeviceD3D11Impl.h"
#include "ResourceMapping.h"

namespace Diligent
{

ShaderD3D11Impl::ShaderD3D11Impl(FixedBlockMemoryAllocator &ShaderObjAllocator, RenderDeviceD3D11Impl *pRenderDeviceD3D11, const ShaderCreationAttribs &CreationAttribs) : 
    TShaderBase(ShaderObjAllocator, pRenderDeviceD3D11, CreationAttribs.Desc),
    ShaderD3DBase(CreationAttribs),
    m_ResourceLayout(this),
    m_StaticResLayout(this),
    m_ShaderTypeIndex(Diligent::GetShaderTypeIndex(CreationAttribs.Desc.ShaderType))
{
    auto *pDeviceD3D11 = pRenderDeviceD3D11->GetD3D11Device();
    switch(CreationAttribs.Desc.ShaderType)
    {

#define CREATE_SHADER(SHADER_NAME, ShaderName)\
        case SHADER_TYPE_##SHADER_NAME:         \
        {                                       \
            ID3D11##ShaderName##Shader *pShader;    \
            HRESULT hr = pDeviceD3D11->Create##ShaderName##Shader( m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), NULL, &pShader ); \
            CHECK_D3D_RESULT_THROW( hr, "Failed to create D3D11 shader" );      \
            if( SUCCEEDED(hr) )                     \
            {                                       \
                pShader->QueryInterface( __uuidof(ID3D11DeviceChild), reinterpret_cast<void**>( static_cast<ID3D11DeviceChild**>(&m_pShader) ) ); \
                pShader->Release();                 \
            }                                       \
            break;                                  \
        }

        CREATE_SHADER(VERTEX,   Vertex)
        CREATE_SHADER(PIXEL,    Pixel)
        CREATE_SHADER(GEOMETRY, Geometry)
        CREATE_SHADER(DOMAIN,   Domain)
        CREATE_SHADER(HULL,     Hull)
        CREATE_SHADER(COMPUTE,  Compute)

        default: UNEXPECTED( "Unknown shader type" );
    }
    
    if(!m_pShader)
        LOG_ERROR_AND_THROW("Failed to create the shader from the byte code");

    // Load full shader resource layout
    m_ResourceLayout.ParseBytecode(m_pShaderByteCode, m_Desc);

    // Clone only static resources that will be set directly in the shader
    SHADER_VARIABLE_TYPE VarTypes[] = {SHADER_VARIABLE_TYPE_STATIC};
    m_StaticResLayout.CloneLayout(m_ResourceLayout, VarTypes, _countof(VarTypes), m_StaticResCache, GetRawAllocator());

    // Byte code is only required for the vertex shader to create input layout
    if( CreationAttribs.Desc.ShaderType != SHADER_TYPE_VERTEX )
        m_pShaderByteCode.Release();
}

ShaderD3D11Impl::~ShaderD3D11Impl()
{
    m_StaticResCache.Destroy(GetRawAllocator());
}

IMPLEMENT_QUERY_INTERFACE( ShaderD3D11Impl, IID_ShaderD3D11, TShaderBase )

IShaderVariable* ShaderD3D11Impl::GetShaderVariable( const Char* Name )
{
    return m_StaticResLayout.GetShaderVariable(Name);
}

void ShaderD3D11Impl::BindResources(IResourceMapping* pResourceMapping, Uint32 Flags)
{
    m_StaticResLayout.BindResources(pResourceMapping, Flags, m_StaticResCache);
}

}
