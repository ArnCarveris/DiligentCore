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

#include <D3Dcompiler.h>

#include "ShaderD3D12Impl.h"
#include "RenderDeviceD3D12Impl.h"
#include "DataBlobImpl.h"
#include "D3DShaderResourceLoader.h"

using namespace Diligent;

namespace Diligent
{


ShaderD3D12Impl::ShaderD3D12Impl(FixedBlockMemoryAllocator& ShaderObjAllocator, RenderDeviceD3D12Impl *pRenderDeviceD3D12, const ShaderCreationAttribs &ShaderCreationAttribs) : 
    TShaderBase(ShaderObjAllocator, pRenderDeviceD3D12, ShaderCreationAttribs.Desc),
    ShaderD3DBase(ShaderCreationAttribs),
    m_ResourceLayout(this),
    m_StaticResLayout(this)
{
    // Load full shader resource layout
    m_ResourceLayout.ParseBytecode(m_pShaderByteCode, m_Desc);

    m_pBoundConstResources.reset( new ShaderResourceCacheD3D12() );

    // Clone only static resources that will be set directly in the shader
    SHADER_VARIABLE_TYPE VarTypes[] = {SHADER_VARIABLE_TYPE_STATIC};
    m_StaticResLayout.CloneLayout(m_ResourceLayout, VarTypes, _countof(VarTypes), m_pBoundConstResources, true);
}

ShaderD3D12Impl::~ShaderD3D12Impl()
{
}

void ShaderD3D12Impl::BindResources(IResourceMapping* pResourceMapping, Uint32 Flags)
{
   m_StaticResLayout.BindResources(pResourceMapping, Flags, m_pBoundConstResources);
}
    
IShaderVariable* ShaderD3D12Impl::GetShaderVariable(const Char* Name)
{
    return m_StaticResLayout.GetShaderVariable(Name);
}

#ifdef VERIFY_SHADER_BINDINGS
void ShaderD3D12Impl::DbgVerifyStaticResourceBindings()
{
    m_StaticResLayout.dbgVerifyBindings();
}
#endif

}
