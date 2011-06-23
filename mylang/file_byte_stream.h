/*  MyLang - Utility library for writing parsers
    Copyright (C) 2011 Dmitry Shatrov

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __MYLANG__FILE_BYTE_STREAM_H__
#define __MYLANG__FILE_BYTE_STREAM_H__

#include <mycpp/file.h>
#include <mycpp/internal_exception.h>

#include <mylang/byte_stream.h>

namespace MyLang {

using namespace MyCpp;

class FileByteStream : public ByteStream
{
protected:
    class PositionMarker : public ByteStream::PositionMarker
    {
    public:
	Uint64 offset;
    };

    Ref<File> in_file;
    Uint64 start_offset;

public:
  /* ByteStream interface */

    ByteResult getNextByte (char *c)
			    throw (InternalException);

    Ref<ByteStream::PositionMarker> getPosition ()
			    throw (InternalException);

    void setPosition (ByteStream::PositionMarker *pmark)
			    throw (InternalException);

  /* (End of ByteStream interface) */

    FileByteStream (File *in_file)
	     throw (InternalException);
};

}

#endif /* __MYLANG__FILE_BYTE_STREAM_H__ */

