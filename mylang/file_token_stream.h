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
#include <mylang/character_recognizer.h>

namespace MyLang {

using namespace MyCpp;

class FileTokenStream : public TokenStream
{
  // NOTE: _NOT_ MT-safe

protected:
    Ref<File> file;
    Ref<CharacterRecognizer> char_recognizer;

    bool report_newlines;

    unsigned long cur_line;
    Uint64 cur_line_start;
    Uint64 cur_line_pos;
    Uint64 cur_char_pos;

    Ref<String> token;

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
		     CharacterRecognizer *char_recognizer = NULL,
		     bool report_newlines = false);
};

}

#endif /* __MYLANG__FILE_TOKEN_STREAM_H__ */

