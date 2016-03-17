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
#include "BasicFileStream.h"

namespace Diligent
{
    BasicFileStream::BasicFileStream(const Diligent::Char *Path, 
                                       EFileAccessMode Access/* = EFileAccessMode::Read*/) :
        m_FileWrpr(Path, Access)
    {
    }

    IMPLEMENT_QUERY_INTERFACE(BasicFileStream, IID_FileStream, TBase)

    bool BasicFileStream::Read(void *Data, size_t BufferSize)
    {
        return m_FileWrpr->Read( Data, BufferSize );
    }

    void BasicFileStream::Read( Diligent::IDataBlob *pData )
    {
        return m_FileWrpr->Read( pData );
    }

    bool BasicFileStream::Write(const void *Data, size_t Size)
    {
        return m_FileWrpr->Write( Data, Size );
    }

    bool BasicFileStream::IsValid()
    {
        return !!m_FileWrpr;
    }

    size_t BasicFileStream::GetSize()
    {
        return m_FileWrpr->GetSize();
    }
}
