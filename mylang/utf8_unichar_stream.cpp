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

#include <mylang/utf8_unichar_stream.h>
#include <mylang/util.h>

using namespace MyCpp;

namespace MyLang {
    
UnicharStream::UnicharResult
Utf8UnicharStream::getNextUnichar (Unichar *uc)
    throw (InternalException)
{
    char cbuf [7];
    zeroMemory (MemoryDesc (cbuf, sizeof cbuf));

    byte_stream->setPosition (cur_pmark);

    unsigned long i = 0;
    for (i = 0; i < 6; i++) {
	char c;
	ByteStream::ByteResult bres;
	bres = byte_stream->getNextByte (&c);
	if (bres == ByteStream::ByteEof) {
	    if (i != 0)
		break;

	    return UnicharEof;
	}

	if (bres != ByteStream::ByteNormal)
	    abortIfReached ();

	cbuf [i] = c;
	if (utf8_validate_sz (cbuf, NULL)) {
	    if (uc != NULL)
		*uc = utf8_valid_to_unichar (cbuf);

	    cur_pmark = byte_stream->getPosition ();

	    fpos.char_pos ++;
	    if (isNewline (utf8_valid_to_unichar (cbuf))) {
		fpos.line ++;
		fpos.line_pos = 0;
	    } else
		fpos.line_pos ++;

	    return UnicharNormal;
	}
    }

    errf->print ("Scruffy.Utf8UnicharStream: "
		 "input is not valid UTF-8")
	 .pendl ();

//    throw ParsingException (fpos);
    throw InternalException ();
}

Ref<UnicharStream::PositionMarker>
Utf8UnicharStream::getPosition ()
    throw (InternalException)
{
    Ref<UnicharStream::PositionMarker> ret_pmark = grab (static_cast <UnicharStream::PositionMarker*> (new PositionMarker));

    PositionMarker *pmark = static_cast <PositionMarker*> (ret_pmark.ptr ());
    pmark->byte_pmark = cur_pmark;
    pmark->fpos = fpos;

    return ret_pmark;
}

void
Utf8UnicharStream::setPosition (UnicharStream::PositionMarker *_pmark)
    throw (InternalException)
{
    if (_pmark == NULL)
	abortIfReached ();

    PositionMarker *pmark = static_cast <PositionMarker*> (_pmark);
    cur_pmark = pmark->byte_pmark;
    fpos = pmark->fpos;
}

FilePosition
Utf8UnicharStream::getFpos (UnicharStream::PositionMarker *_pmark)
{
    if (_pmark == NULL)
	abortIfReached ();

    PositionMarker *pmark = static_cast <PositionMarker*> (_pmark);
    return pmark->fpos;
}

FilePosition
Utf8UnicharStream::getFpos ()
{
    return fpos;
}

Utf8UnicharStream::Utf8UnicharStream (ByteStream *byte_stream)
    throw (InternalException)
{
    if (byte_stream == NULL)
	abortIfReached ();

    this->byte_stream = byte_stream;
    cur_pmark = byte_stream->getPosition ();
}

}

