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


#include <mycpp/io.h>

#include <mylang/file_byte_stream.h>

// Internal
#define DEBUG_INT(a)

using namespace MyCpp;

namespace MyLang {
    
ByteStream::ByteResult
FileByteStream::getNextByte (char *c_ret)
    throw (InternalException)
{
/*
    errf->print ("Scruffy.FileByteStream.getNextByte: "
		 "start_offset: ")
	 .print ((unsigned long long) start_offset)
	 .pendl ();
*/

    in_file->seekSet (start_offset);

    unsigned char c;
    IOResult res;
    res = in_file->fullRead (MemoryDesc (&c, 1), NULL);
    if (res == IOResultEof) {
      DEBUG_INT (
	errf->print ("Scruffy.FileByteStream.getNextByte: "
		     "returning ByteEof")
	     .pendl ();
      );

	return ByteEof;
    }

    if (res != IOResultNormal)
	throw InternalException ();

    start_offset = in_file->tell ();

    if (c_ret != NULL)
	*c_ret = (char) c;

/*
    errf->print ("Scruffy.FileByteStream.getNextByte: "
		 "retuning byte: ")
	 .print ((unsigned long) (unsigned char) c)
	 .pendl ();
*/

    return ByteNormal;
}

Ref<ByteStream::PositionMarker>
FileByteStream::getPosition ()
    throw (InternalException)
{
    Ref<ByteStream::PositionMarker> ret_pmark = grab (static_cast <ByteStream::PositionMarker*> (new PositionMarker));

    PositionMarker *pmark = static_cast <PositionMarker*> (ret_pmark.ptr ());
    pmark->offset = start_offset;

/*
    errf->print ("Scruffy.FileByteStream.getPosition: "
		 "returning ")
	 .print ((unsigned long long) start_offset)
	 .pendl ();
*/

    return ret_pmark;
}

void
FileByteStream::setPosition (ByteStream::PositionMarker *_pmark)
    throw (InternalException)
{
    PositionMarker *pmark = static_cast <PositionMarker*> (_pmark);
    start_offset = pmark->offset;

/*
    errf->print ("Scruffy.FileByteStream.setPosition: "
		 "position set to ")
	 .print ((unsigned long long) pmark->offset)
	 .pendl ();
*/
}

FileByteStream::FileByteStream (File *in_file)
    throw (InternalException)
{
    if (in_file == NULL)
	abortIfReached ();

    this->in_file = in_file;
    start_offset = in_file->tell ();
}

}

