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
/// Definition of the Diligent::ITexture interface and related data structures

#include "DeviceObject.h"

namespace Diligent
{

// {A64B0E60-1B5E-4CFD-B880-663A1ADCBE98}
static const Diligent::INTERFACE_ID IID_Texture =
{ 0xa64b0e60, 0x1b5e, 0x4cfd, { 0xb8, 0x80, 0x66, 0x3a, 0x1a, 0xdc, 0xbe, 0x98 } };

struct DepthStencilClearValue
{
    Float32 Depth;
    Uint8 Stencil;
    DepthStencilClearValue() : 
        Depth(1.f),
        Stencil(0)
    {}
};

struct OptimizedClearValue
{
    TEXTURE_FORMAT Format;
    Float32 Color[ 4 ];
    DepthStencilClearValue DepthStencil;
    OptimizedClearValue() : 
        Format(TEX_FORMAT_UNKNOWN)
    {
        Color[0] = 0;
        Color[1] = 0;
        Color[2] = 0;
        Color[3] = 0;
    }
};

/// Texture description
struct TextureDesc : DeviceObjectAttribs
{
    /// Texture type. See Diligent::RESOURCE_DIMENSION for details.
    RESOURCE_DIMENSION Type;

    /// Texture width, in pixels.
    Uint32 Width;

    /// Texture height, in pixels.
    Uint32 Height;
    union
    {
        /// For a 1D array or 2D array, number of array slices
        Uint32 ArraySize;

        /// For a 3D texture, number of depth slices
        Uint32 Depth;
    };

    /// Texture format, see Diligent::TEXTURE_FORMAT.
    TEXTURE_FORMAT Format;

    /// Number of Mip levels in the texture. Multisampled textures can only have 1 Mip level.
    /// Specify 0 to generate full mipmap chain.
    Uint32 MipLevels;

    /// Number of samples.\n
    /// Only 2D textures or 2D texture arrays can be multisampled.
    Uint32 SampleCount;

    /// Texture usage. See Diligent::USAGE for details.
    USAGE Usage;

    /// Bind flags, see Diligent::BIND_FLAGS for details. \n
    /// The following bind flags are allowed:
    /// Diligent::BIND_SHADER_RESOURCE, Diligent::BIND_RENDER_TARGET, Diligent::BIND_DEPTH_STENCIL,
    /// Diligent::and BIND_UNORDERED_ACCESS. \n
    /// Multisampled textures cannot have Diligent::BIND_UNORDERED_ACCESS flag set
    Uint32 BindFlags;

    /// CPU access flags or 0 if no CPU access is allowed, 
    /// see Diligent::CPU_ACCESS_FLAG for details.
    Uint32 CPUAccessFlags;
    

    /// Miscellaneous flags, see Diligent::MISC_TEXTURE_FLAG for details.
    Uint32 MiscFlags;
    
    /// Optimized clear value
    OptimizedClearValue ClearValue;

    /// Initializes the structure members with default values

    /// Default values:
    /// Member          | Default value
    /// ----------------|--------------
    /// Type            | RESOURCE_DIM_UNDEFINED
    /// Width           | 0
    /// Height          | 0
    /// ArraySize       | 1
    /// Format          | TEX_FORMAT_UNKNOWN
    /// MipLevels       | 1
    /// SampleCount     | 1
    /// Usage           | USAGE_DEFAULT
    /// BindFlags       | 0
    /// CPUAccessFlags  | 0
    /// MiscFlags       | 0
    TextureDesc() : 
        Type(RESOURCE_DIM_UNDEFINED),
        Width(0),
        Height(0),
        ArraySize(1),
        Format(TEX_FORMAT_UNKNOWN),
        MipLevels(1),
        SampleCount(1),
        Usage(USAGE_DEFAULT),
        BindFlags(0),
        CPUAccessFlags(0),
        MiscFlags(0)
    {
    }
};

/// Describes data for one subresource
struct TextureSubResData
{
    /// Pointer to the subresource data
    const void* pData;

    /// For 2D and 3D textures, row stride in bytes
    Uint32 Stride;

    /// For 3D textures, depth slice stride in bytes
    /// \note On OpenGL, this must be a mutliple of Stride
    Uint32 DepthStride;

    /// Initializes the structure members with default values

    /// Default values:
    /// Member          | Default value
    /// ----------------|--------------
    /// pData           | nullptr
    /// Stride          | 0
    /// DepthStride     | 0
    TextureSubResData():
        pData(nullptr),
        Stride(0),
        DepthStride(0)
    {}
    
    /// Initializes the structure members with provided values
    TextureSubResData(void *_pData, Uint32 _Stride, Uint32 _DepthStride=0):
        pData(_pData),
        Stride(_Stride),
        DepthStride(_DepthStride)
    {}
};

/// Describes the initial data to store in the texture
struct TextureData
{
    /// Pointer to the array of the TextureSubResData elements containing
    /// information about each subresource.
    TextureSubResData *pSubResources;

    /// Number of elements in pSubResources array.
    /// NumSubresources must exactly match the number
    /// of subresources in the texture. Otherwise an error
    /// occurs.
    Uint32 NumSubresources;

    /// Initializes the structure members with default values

    /// Default values:
    /// Member          | Default value
    /// ----------------|--------------
    /// pSubResources   | nullptr
    /// NumSubresources | 0
    TextureData() : 
        pSubResources(nullptr),
        NumSubresources(0)
    {}
};

/// Texture inteface
class ITexture : public IDeviceObject
{
public:
    /// Queries the specific interface, see IObject::QueryInterface() for details
    virtual void QueryInterface( const Diligent::INTERFACE_ID &IID, IObject **ppInterface ) = 0;

    /// Returns the texture description used to create the object
    virtual const TextureDesc& GetDesc()const = 0;
    
    /// Creates a new texture view

    /// \param [in] ViewDesc - View description. See Diligent::TextureViewDesc for details.
    /// \param [out] ppView - Address of the memory location where the pointer to the view interface will be written to.
    /// 
    /// \remarks To create a shader resource view addressing the entire texture, set only TextureViewDesc::ViewType 
    ///          member of the ViewDesc parameter to Diligent::TEXTURE_VIEW_SHADER_RESOURCE and leave all other 
    ///          members in their default values. Using the same method, you can create render target or depth stencil
    ///          view addressing the largest mip level.\n
    ///          If texture view format is Diligent::TEX_FORMAT_UNKNOWN, the view format will match the texture format.\n
    ///          If texture view type is Diligent::TEXTURE_VIEW_UNDEFINED, the type will match the texture type.\n
    ///          If the number of mip levels is 0, and the view type is shader resource, the view will address all mip levels.
    ///          For other view types it will address one mip level.\n
    ///          If the number of slices is 0, all slices from FirstArraySlice or FirstDepthSlice will be referenced by the view.
    ///          For non-array textures, the only allowed values for the number of slices are 0 and 1.\n
    ///          Texture view will contain strong reference to the texture, so the texture will not be destroyed
    ///          until all views are released.\n
    ///          The function calls AddRef() for the created interface, so it must be released by
    ///          a call to Release() when it is no longer needed.
    virtual void CreateView(const struct TextureViewDesc &ViewDesc, class ITextureView **ppView) = 0;

    /// Returns the pointer to the default view.
    
    /// \param [in] ViewType - Type of the requested view. See Diligent::TEXTURE_VIEW_TYPE.
    /// \return Pointer to the interface
    ///
    /// \note The function does not increase the reference counter for the returned interface, so
    ///       Release() must *NOT* be called.
    virtual ITextureView* GetDefaultView( TEXTURE_VIEW_TYPE ViewType ) = 0;

    /// Updates the data in the texture

    /// \param [in] pContext - Pointer to the device context interface to be used to perform the operation.
    /// \param [in] MipLevel - Mip level of the texture subresource to update.
    /// \param [in] Slice - Array slice. Should be 0 for non-array textures.
    /// \param [in] DstBox - Destination region on the texture to update.
    /// \param [in] SubresData - Source data to copy to the texture.
    virtual void UpdateData( class IDeviceContext *pContext, Uint32 MipLevel, Uint32 Slice, const Box &DstBox, const TextureSubResData &SubresData ) = 0;

    /// Copies data from another texture

    /// \param [in] pContext - Pointer to the device context interface to be used to perform the operation.
    /// \param [in] pSrcTexture - Source texture for the copy operation
    /// \param [in] SrcMipLevel - Mip level of the source texture to copy data from.
    /// \param [in] SrcSlice - Array slice of the source texture to copy data from. 
    ///                        Should be 0 for non-array textures.
    /// \param [in] pSrcBox - Source region to copy.
    ///                       Use nullptr to copy the entire subresource.
    /// \param [in] DstMipLevel - Mip level to copy data to.
    /// \param [in] DstSlice - Array slice to copy data to. 
    ///                        Must be 0 for non-array textures.
    /// \param [in] DstX - X offset on the destination subresource
    /// \param [in] DstY - Y offset on the destination subresource
    /// \param [in] DstZ - Z offset on the destination subresource
    virtual void CopyData(IDeviceContext *pContext, 
                          ITexture *pSrcTexture, 
                          Uint32 SrcMipLevel,
                          Uint32 SrcSlice,
                          const Box *pSrcBox,
                          Uint32 DstMipLevel,
                          Uint32 DstSlice,
                          Uint32 DstX,
                          Uint32 DstY,
                          Uint32 DstZ) = 0;

    /// Map the texture - not implemented yet
    virtual void Map( IDeviceContext *pContext, MAP_TYPE MapType, Uint32 MapFlags, PVoid &pMappedData ) = 0;
    /// Unmap the textute - not implemented yet
    virtual void Unmap( IDeviceContext *pContext ) = 0;
};

}
