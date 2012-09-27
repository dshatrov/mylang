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


#ifndef __MYLANG__UTF8_UNICHAR_STREAM__
#define __MYLANG__UTF8_UNICHAR_STREAM__

#include <mycpp/internal_exception.h>

#include <mylang/unichar_stream.h>
#include <mylang/byte_stream.h>

namespace MyLang {

using namespace MyCpp;

class Utf8UnicharStream : public UnicharStream
{
protected:
    class PositionMarker : public UnicharStream::PositionMarker
    {
    public:
	Ref<ByteStream::PositionMarker> byte_pmark;
	FilePosition fpos;
    };

    Ref<ByteStream> byte_stream;
    Ref<ByteStream::PositionMarker> cur_pmark;

    FilePosition fpos;

    UnicharResult doGetNextUnichar (Unichar *ret_uc);

    unsigned skipNewline ();

public:
  /* UnicharStream interface */

    UnicharResult getNextUnichar (Unichar *ret_uc)
			    throw (InternalException);

    Ref<UnicharStream::PositionMarker> getPosition ()
			    throw (InternalException);

    void setPosition (UnicharStream::PositionMarker *pmark)
			    throw (InternalException);

    FilePosition getFpos (UnicharStream::PositionMarker *_pmark);

    FilePosition getFpos ();

  /* (End of UnicharStream interface) */

    Utf8UnicharStream (ByteStream *byte_stream)
		throw (InternalException);
};

}

#endif /* __MYLANG__UTF8_UNICHAR_STREAM__ */

