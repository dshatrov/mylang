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


#ifndef __MYLANG__FILE_TOKEN_STREAM_H__
#define __MYLANG__FILE_TOKEN_STREAM_H__


#include <mycpp/mycpp.h>
#include <mycpp/file.h>

#include <mylang/token_stream.h>


namespace MyLang {

using namespace MyCpp;

class FileTokenStream : public TokenStream
{
  // NOTE: _NOT_ MT-safe

protected:
    Ref<File> file;

    bool const report_newlines;
    bool const minus_is_alpha;

    unsigned long cur_line;
    Uint64 cur_line_start;
    Uint64 cur_line_pos;
    Uint64 cur_char_pos;

    Byte *token_buf;
    Size token_len;
    Size const max_token_len;

public:
  // TokenStream interface

    ConstMemoryDesc getNextToken ()
			   throw (InternalException);

    void getPosition (PositionMarker *ret_pmark /* non-null */)
	       throw (InternalException);

    void setPosition (PositionMarker const *pmark /* non-null */)
	       throw (InternalException);

    FilePosition getFilePosition ()
			   throw (InternalException);

    unsigned long getLine ();

    Ref<String> getLineStr ()
		     throw (InternalException);

  // (End of TokenStream interface)

    FileTokenStream (File *file,
		     bool report_newlines = false,
                     bool minus_is_alpha  = false,
                     Uint64 max_token_len = 4096);

    ~FileTokenStream ();
};

}


#endif /* __MYLANG__FILE_TOKEN_STREAM_H__ */

