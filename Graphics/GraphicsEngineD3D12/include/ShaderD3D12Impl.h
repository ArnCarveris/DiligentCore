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
/// Declaration of Diligent::ShaderD3D12Impl class

#include "RenderDeviceD3D12.h"
#include "ShaderD3D12.h"
#include "ShaderBase.h"
#include "ShaderD3DBase.h"
#include "ShaderResourceLayoutD3D12.h"

#ifdef _DEBUG
#   define VERIFY_SHADER_BINDINGS
#endif

namespace Diligent
{

class ResourceMapping;
class FixedBlockMemoryAllocator;

/// Implementation of the Diligent::IShaderD3D12 interface
class ShaderD3D12Impl : public ShaderBase<IShaderD3D12, IRenderDeviceD3D12, FixedBlockMemoryAllocator>, public ShaderD3DBase
{
public:
    typedef ShaderBase<IShaderD3D12, IRenderDeviceD3D12, FixedBlockMemoryAllocator> TShaderBase;

    ShaderD3D12Impl(FixedBlockMemoryAllocator& ShaderObjAllocator, class RenderDeviceD3D12Impl *pRenderDeviceD3D12, const ShaderCreationAttribs &ShaderCreationAttribs);
    ~ShaderD3D12Impl();
    
    //virtual void QueryInterface( const Diligent::INTERFACE_ID &IID, IObject **ppInterface )override;

    virtual void BindResources( IResourceMapping* pResourceMapping, Uint32 Flags )override;
    
    virtual IShaderVariable* GetShaderVariable(const Char* Name)override;

    ID3DBlob* GetShaderByteCode(){return m_pShaderByteCode;}
    const ShaderResourceLayoutD3D12& GetResourceLayout()const{return m_ResourceLayout;}
    const ShaderResourceLayoutD3D12& GetConstResLayout()const{return m_StaticResLayout;}

#ifdef VERIFY_SHADER_BINDINGS
    void DbgVerifyStaticResourceBindings();
#endif

private:

    ShaderResourceLayoutD3D12 m_ResourceLayout;
    ShaderResourceLayoutD3D12 m_StaticResLayout;
    std::shared_ptr<ShaderResourceCacheD3D12> m_pBoundConstResources;
};

}
