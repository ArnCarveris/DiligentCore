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
/// Implementation of the Diligent::TextureViewBase template class

#include "TextureView.h"
#include "DeviceObjectBase.h"
#include "RefCntAutoPtr.h"
#include "GraphicsUtilities.h"

namespace Diligent
{

/// Template class implementing base functionality for a texture view interface

/// \tparam BaseInterface - base interface that this class will inheret 
///                         (Diligent::ITextureViewD3D11 or Diligent::ITextureViewGL).
template<class BaseInterface, class TexViewObjAllocator>
class TextureViewBase : public DeviceObjectBase<BaseInterface, TextureViewDesc, TexViewObjAllocator>
{
public:
    typedef DeviceObjectBase<BaseInterface, TextureViewDesc, TexViewObjAllocator> TDeviceObjectBase;

    TextureViewBase( TexViewObjAllocator &ObjAllocator,
                     IRenderDevice *pDevice,
                     const TextureViewDesc& ViewDesc, 
                     class ITexture *pTexture,
                     bool bIsDefaultView ) :
        // Default views are created as part of the texture, so we cannot not keep strong 
        // reference to the texture to avoid cyclic links. Instead, we will attach to the 
        // reference counters of the texture.
        TDeviceObjectBase( ObjAllocator, pDevice, ViewDesc, bIsDefaultView ? pTexture : nullptr ),
        m_pTexture( pTexture ),
        // For non-default view, we will keep strong reference to texture
        m_spTexture(bIsDefaultView ? nullptr : pTexture)
    {}

    IMPLEMENT_QUERY_INTERFACE_IN_PLACE( IID_TextureView, TDeviceObjectBase )

    virtual void SetSampler( class ISampler *pSampler )override final
    {
        VERIFY( this->m_Desc.ViewType == TEXTURE_VIEW_SHADER_RESOURCE, "Texture view \"", this->m_Desc.Name, "\": A sampler can be attached to a shader resource view only. The view type is ", GetTexViewTypeLiteralName(this->m_Desc.ViewType) );
        m_pSampler = pSampler;
    }

    virtual ISampler* GetSampler()override final
    { 
        return m_pSampler; 
    }
    
    virtual ITexture* GetTexture()override final
    {
        return m_pTexture;
    }

protected:
    /// Strong reference to the sampler
    Diligent::RefCntAutoPtr<ISampler> m_pSampler;

    /// Raw pointer to the texture
    ITexture *m_pTexture;

    /// Strong reference to the texture. Used for non-default views
    /// to keep the texture alive
    Diligent::RefCntAutoPtr<ITexture> m_spTexture;
};

}
