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

namespace Diligent
{

inline GLenum PrimitiveTopologyToGLTopology(PRIMITIVE_TOPOLOGY PrimTopology)
{
    switch(PrimTopology)
    {
        case PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:   return GL_TRIANGLES;      break;
        case PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:  return GL_TRIANGLE_STRIP; break;
        case PRIMITIVE_TOPOLOGY_POINT_LIST:     return GL_POINTS;         break;
        case PRIMITIVE_TOPOLOGY_LINE_LIST:      return GL_LINES;          break;
        default: return 0;
    }
}

inline GLenum TypeToGLType(VALUE_TYPE Value)
{
    switch(Value)
    {
        case VT_INT8:    return GL_BYTE;           break;
        case VT_INT16:   return GL_SHORT;          break;
        case VT_INT32:   return GL_INT;            break;
        case VT_UINT8:   return GL_UNSIGNED_BYTE;  break;
        case VT_UINT16:  return GL_UNSIGNED_SHORT; break;
        case VT_UINT32:  return GL_UNSIGNED_INT;   break;
        case VT_FLOAT32: return GL_FLOAT;          break;
        default: return 0;
    }
}

inline GLenum UsageToGLUsage(USAGE Usage)
{
    // http://www.informit.com/articles/article.aspx?p=2033340&seqNum=2
    switch(Usage)
    {
        case USAGE_STATIC:       return GL_STATIC_DRAW;
        case USAGE_DEFAULT:      return GL_DYNAMIC_DRAW;
        case USAGE_DYNAMIC:      return GL_STREAM_DRAW;
        case USAGE_CPU_ACCESSIBLE: return GL_DYNAMIC_READ;
        default: UNEXPECTED( "Unknow usage" ); return 0;
    }
}

inline void FilterTypeToGLFilterType(FILTER_TYPE Filter, GLenum &GLFilter, Bool &bIsAnisotropic, Bool &bIsComparison)
{
    switch(Filter)
    {
        case FILTER_TYPE_UNKNOWN:
            UNEXPECTED( "Unspecified filter type" ); 
            bIsAnisotropic = false;
            bIsComparison = false;
            GLFilter = GL_NEAREST;
            break;

        case FILTER_TYPE_POINT:
            bIsAnisotropic = false;
            bIsComparison = false;
            GLFilter = GL_NEAREST;
            break;

        case FILTER_TYPE_LINEAR:
            bIsAnisotropic = false;
            bIsComparison = false;
            GLFilter = GL_LINEAR;
            break;

	    case FILTER_TYPE_ANISOTROPIC:
            bIsAnisotropic = true;
            bIsComparison = false;
            GLFilter = GL_LINEAR;
            break;

        case FILTER_TYPE_COMPARISON_POINT:
            bIsAnisotropic = false;
            bIsComparison = true;
            GLFilter = GL_NEAREST;
            break;

        case FILTER_TYPE_COMPARISON_LINEAR:
            bIsAnisotropic = false;
            bIsComparison = true;
            GLFilter = GL_LINEAR;
            break;

        case FILTER_TYPE_COMPARISON_ANISOTROPIC:
            bIsAnisotropic = true;
            bIsComparison = true;
            GLFilter = GL_LINEAR;
            break;

        default:
            bIsAnisotropic = false;
            bIsComparison = false;
            UNEXPECTED( "Unknown filter type" ); 
            GLFilter = GL_NEAREST;
            break;
    }
}

GLenum TexFormatToGLInternalTexFormat(TEXTURE_FORMAT TexFormat, Uint32 BindFlags = 0);
GLenum CorrectGLTexFormat(GLenum GLTexFormat, Uint32 BindFlags);

inline GLenum TexAddressModeToGLAddressMode(TEXTURE_ADDRESS_MODE Mode)
{
    switch(Mode)
    {
        case TEXTURE_ADDRESS_UNKNOWN: UNEXPECTED( "Texture address mode is not specified" ); return GL_CLAMP_TO_EDGE;
        case TEXTURE_ADDRESS_WRAP:          return GL_REPEAT;
        case TEXTURE_ADDRESS_MIRROR:        return GL_MIRRORED_REPEAT;
        case TEXTURE_ADDRESS_CLAMP:         return GL_CLAMP_TO_EDGE;
        case TEXTURE_ADDRESS_BORDER:        return GL_CLAMP_TO_BORDER;
        case TEXTURE_ADDRESS_MIRROR_ONCE:   return GL_MIRROR_CLAMP_TO_EDGE; // only available with OpenGL 4.4
                                            // This mode seems to be different from D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
                                            // The texture coord is clamped to the [-1, 1] range, but mirrors the 
                                            // negative direction with the positive. Basically, it acts as
                                            // GL_CLAMP_TO_EDGE except that it takes the absolute value of the texture 
                                            // coordinates before clamping.
        default: UNEXPECTED( "Unknown texture address mode" ); return GL_CLAMP_TO_EDGE;
    }
}

inline GLenum CompareFuncToGLCompareFunc(COMPARISON_FUNCTION Func)
{
    switch(Func)
    {
        case COMPARISON_FUNC_UNKNOWN: UNEXPECTED( "Comparison function is not specified" ); return GL_ALWAYS;
        case COMPARISON_FUNC_NEVER:         return GL_NEVER;
	    case COMPARISON_FUNC_LESS:          return GL_LESS;
	    case COMPARISON_FUNC_EQUAL:         return GL_EQUAL;
	    case COMPARISON_FUNC_LESS_EQUAL:    return GL_LEQUAL;
	    case COMPARISON_FUNC_GREATER:       return GL_GREATER;
	    case COMPARISON_FUNC_NOT_EQUAL:     return GL_NOTEQUAL;
	    case COMPARISON_FUNC_GREATER_EQUAL: return GL_GEQUAL;
	    case COMPARISON_FUNC_ALWAYS:        return GL_ALWAYS;
        default: UNEXPECTED( "Unknown comparison func" ); return GL_ALWAYS;
    }
}

struct NativePixelAttribs
{
    GLenum PixelFormat;
    GLenum DataType;
    Bool IsCompressed;
    explicit NativePixelAttribs(GLenum _PixelFormat = 0, GLenum _DataType = 0, Bool _IsCompressed = False) :
        PixelFormat(_PixelFormat),
        DataType(_DataType),
        IsCompressed(_IsCompressed)
    {}
};

inline Uint32 GetNumPixelFormatComponents(GLenum Format)
{
    switch(Format)
    {
        case GL_RGBA:
        case GL_RGBA_INTEGER:
            return 4;

        case GL_RGB:
        case GL_RGB_INTEGER:
            return 3;

        case GL_RG:
        case GL_RG_INTEGER:
            return 2;

        case GL_RED:
        case GL_RED_INTEGER:
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_STENCIL:
            return 1;

        default: UNEXPECTED( "Unknonw pixel format" ); return 0;
    };
}

inline Uint32 GetPixelTypeSize(GLenum Type)
{
    switch(Type)
    {
        case GL_FLOAT:          return sizeof(GLfloat);

        case GL_UNSIGNED_INT_10_10_10_2: 
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
        case GL_UNSIGNED_INT_24_8:
        case GL_UNSIGNED_INT_5_9_9_9_REV:
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        
        case GL_INT:            return sizeof(GLint);
        case GL_HALF_FLOAT:     return sizeof(GLhalf);

        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);

        case GL_SHORT:          return sizeof(GLshort);
        case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
        case GL_BYTE:           return sizeof(GLbyte);
        
        case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:return sizeof(GLfloat) + sizeof(GLuint);

        default: UNEXPECTED( "Unknonw pixel type" ); return 0;
    }
}

NativePixelAttribs GetNativePixelTransferAttribs(TEXTURE_FORMAT TexFormat);
GLenum AccessFlags2GLAccess( Uint32 UAVAccessFlags );
GLenum TypeToGLTexFormat( VALUE_TYPE ValType, Uint32 NumComponents, Bool bIsNormalized );
GLenum StencilOp2GlStencilOp( STENCIL_OP StencilOp );
GLenum BlendFactor2GLBlend( BLEND_FACTOR bf );
GLenum BlendOperation2GLBlendOp( BLEND_OPERATION BlendOp );

}
