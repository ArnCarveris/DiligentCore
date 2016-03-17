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

#include "Texture2D_OGL.h"
#include "RenderDeviceGLImpl.h"
#include "DeviceContextGLImpl.h"
#include "GLTypeConversions.h"
#include "GraphicsUtilities.h"

namespace Diligent
{

Texture2D_OGL::Texture2D_OGL( FixedBlockMemoryAllocator& TexObjAllocator, 
                              FixedBlockMemoryAllocator& TexViewObjAllocator,
                              class RenderDeviceGLImpl *pDeviceGL, 
                              class DeviceContextGLImpl *pDeviceContext, 
                              const TextureDesc& TexDesc, 
                              const TextureData &InitData /*= TextureData()*/,
							  bool bIsDeviceInternal /*= false*/) : 
    TextureBaseGL(TexObjAllocator, TexViewObjAllocator, pDeviceGL, TexDesc, InitData, bIsDeviceInternal)
{
    auto &ContextState = pDeviceContext->GetContextState();

    if( m_Desc.SampleCount > 1 )
    {
        m_BindTarget = GL_TEXTURE_2D_MULTISAMPLE;
        ContextState.BindTexture(-1, m_BindTarget, m_GlTexture);
        //                                               format          width          height         depth
        glTexStorage2DMultisample(m_BindTarget, m_Desc.SampleCount, m_GLTexFormat, m_Desc.Width, m_Desc.Height, GL_TRUE);
        // The last parameter specifies whether the image will use identical sample locations and the same number of 
        // samples for all texels in the image, and the sample locations will not depend on the internal format or size 
        // of the image.
        CHECK_GL_ERROR_AND_THROW("Failed to allocate storage for the 2D multisample texture");
        // * An INVALID_ENUM error is generated if sizedinternalformat is not colorrenderable,
        //   depth - renderable, or stencil - renderable
        // * An INVALID_OPERATION error is generated if samples is greater than the maximum number of samples 
        //   supported for this target and internalformat. The maximum number of samples supported can be 
        //   determined by calling glGetInternalformativ with a pname of GL_SAMPLES

        SetDefaultGLParameters();

        VERIFY( InitData.pSubResources == nullptr, "Multisampled textures cannot be modified directly" );
    }
    else
    {
        m_BindTarget = GL_TEXTURE_2D;
        ContextState.BindTexture(-1, m_BindTarget, m_GlTexture);
        //                             levels             format          width         height
        glTexStorage2D(m_BindTarget, m_Desc.MipLevels, m_GLTexFormat, m_Desc.Width, m_Desc.Height);
        CHECK_GL_ERROR_AND_THROW("Failed to allocate storage for the 2D texture");
        // When target is GL_TEXTURE_2D, calling glTexStorage2D is equivalent to the following pseudo-code:
        //for (i = 0; i < levels; i++)
        //{
        //    glTexImage2D(target, i, internalformat, width, height, 0, format, type, NULL);
        //    width = max(1, (width / 2));
        //    height = max(1, (height / 2));
        //}

        SetDefaultGLParameters();

        if( InitData.pSubResources )
        {
            if(  m_Desc.MipLevels == InitData.NumSubresources )
            {
                for(Uint32 Mip = 0; Mip < m_Desc.MipLevels; ++Mip)
                {
                    Box DstBox(0, std::max(m_Desc.Width >>Mip, 1U),
                                0, std::max(m_Desc.Height>>Mip, 1U) );
                    // UpdateData() is a virtual function. If we try to call it through vtbl from here,
                    // we will get into TextureBaseGL::UpdateData(), because instance of Texture2D_OGL
                    // is not fully constructed yet.
                    // To call the required function, we need to explicitly specify the class: 
                    Texture2D_OGL::UpdateData( pDeviceContext, Mip, 0, DstBox, InitData.pSubResources[Mip] );
                }
            }
            else
            {
                UNEXPECTED("Incorrect number of subresources");
            }
        }
    }

    ContextState.BindTexture( -1, m_BindTarget, GLObjectWrappers::GLTextureObj(false) );
}

Texture2D_OGL::~Texture2D_OGL()
{
}

void Texture2D_OGL::UpdateData( IDeviceContext *pContext, Uint32 MipLevel, Uint32 Slice, const Box &DstBox, const TextureSubResData &SubresData )
{
    TextureBaseGL::UpdateData(pContext, MipLevel, Slice, DstBox, SubresData);

    auto *pDeviceContextGL = ValidatedCast<DeviceContextGLImpl>(pContext);
    auto &ContextState = pDeviceContextGL->GetContextState();

    // GL_TEXTURE_UPDATE_BARRIER_BIT: 
    //      Writes to a texture via glTex( Sub )Image*, glCopyTex( Sub )Image*, glClearTex*Image, 
    //      glCompressedTex( Sub )Image*, and reads via glTexImage() after the barrier will reflect 
    //      data written by shaders prior to the barrier. Additionally, texture writes from these 
    //      commands issued after the barrier will not execute until all shader writes initiated prior 
    //      to the barrier complete
    TextureMemoryBarrier( GL_TEXTURE_UPDATE_BARRIER_BIT, ContextState );

    ContextState.BindTexture(-1, m_BindTarget, m_GlTexture);

    // Transfers to OpenGL memory are called unpack operations
    // If there is a buffer bound to GL_PIXEL_UNPACK_BUFFER target, then all the pixel transfer
    // operations will be performed from this buffer. We need to make sure none is bound
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    auto TransferAttribs = GetNativePixelTransferAttribs(m_Desc.Format);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if( TransferAttribs.IsCompressed )
    {
        VERIFY( (DstBox.MinX % 4) == 0 && (DstBox.MinY % 4) == 0 &&
                ((DstBox.MaxX % 4) == 0 || DstBox.MaxX == std::max(m_Desc.Width >>MipLevel, 1U)) && 
                ((DstBox.MaxY % 4) == 0 || DstBox.MaxY == std::max(m_Desc.Height>>MipLevel, 1U)), 
                "Compressed texture update region must be 4 pixel-aligned" );
        const auto &FmtAttribs = GetTextureFormatAttribs(m_Desc.Format);
        auto BlockBytesInRow = ((DstBox.MaxX - DstBox.MinX + 3)/4) * FmtAttribs.ComponentSize;
        VERIFY( SubresData.Stride == BlockBytesInRow, 
                "Compressed data stride (", SubresData.Stride, " must match the size of a row of compressed blocks (", BlockBytesInRow, ")" );
        
        //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        //glPixelStorei(GL_UNPACK_COMPRESSED_BLOCK_WIDTH, 0);
        glCompressedTexSubImage2D(m_BindTarget, MipLevel, 
                        DstBox.MinX, 
                        DstBox.MinY, 
                        DstBox.MaxX - DstBox.MinX, 
                        DstBox.MaxY - DstBox.MinY, 
                        // The format must be the same compressed-texture format previously 
                        // specified by glTexStorage2D() (thank you OpenGL for another useless 
                        // parameter that is nothing but the source of confusion), otherwise
                        // INVALID_OPERATION error is generated.
                        m_GLTexFormat, 
                        // An INVALID_VALUE error is generated if imageSize is not consistent with
                        // the format, dimensions, and contents of the compressed image( too little or
                        // too much data ),
                        ((DstBox.MaxY - DstBox.MinY + 3)/4) * SubresData.Stride,
                        SubresData.pData);
    }
    else
    {
        const auto& TexFmtInfo = GetTextureFormatAttribs(m_Desc.Format);
        const auto PixelSize = TexFmtInfo.NumComponents * TexFmtInfo.ComponentSize;
        VERIFY( (SubresData.Stride % PixelSize)==0, "Data stride is not multiple of pixel size" );
        glPixelStorei(GL_UNPACK_ROW_LENGTH, SubresData.Stride / PixelSize);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

        glTexSubImage2D(m_BindTarget, MipLevel, 
                        DstBox.MinX, 
                        DstBox.MinY, 
                        DstBox.MaxX - DstBox.MinX, 
                        DstBox.MaxY - DstBox.MinY, 
                        TransferAttribs.PixelFormat, TransferAttribs.DataType, 
                        SubresData.pData);
    }
    CHECK_GL_ERROR("Failed to update subimage data");

    ContextState.BindTexture( -1, m_BindTarget, GLObjectWrappers::GLTextureObj(false) );
}

void Texture2D_OGL::AttachToFramebuffer( const TextureViewDesc& ViewDesc, GLenum AttachmentPoint )
{
    // For glFramebufferTexture2D(), if texture name is not zero, then texture target must be GL_TEXTURE_2D, 
    // GL_TEXTURE_RECTANGLE or one of the 6 cubemap face targets GL_TEXTURE_CUBE_MAP_POSITIVE_X, ...
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, AttachmentPoint, m_BindTarget, m_GlTexture, ViewDesc.MostDetailedMip );
    CHECK_GL_ERROR( "Failed to attach texture 2D to draw framebuffer" );
    glFramebufferTexture2D( GL_READ_FRAMEBUFFER, AttachmentPoint, m_BindTarget, m_GlTexture, ViewDesc.MostDetailedMip );
    CHECK_GL_ERROR( "Failed to attach texture 2D to read framebuffer" );
}

}
