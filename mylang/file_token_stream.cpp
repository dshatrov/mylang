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


#include <mycpp/util.h>
#include <mycpp/buffer.h>
#include <mycpp/io.h>

#include <mylang/file_token_stream.h>

#define DEBUG(a) ;
// Flow
#define DEBUG_FLO(a) ;
// Internal
#define DEBUG_INT(a) ;

#define MYLANG__MAX_TOKEN_LENGTH 1024

using namespace MyCpp;

namespace MyLang {

static bool
is_whitespace (unsigned char c,
	       CharacterRecognizer *char_recognizer)
{
    abortIf (char_recognizer == NULL);

    if (char_recognizer->isSpace ((Unichar) c))
	return true;

    return false;
}

static bool
is_newline (unsigned char c,
	    CharacterRecognizer *char_recognizer)
{
    abortIf (char_recognizer == NULL);

    // TODO Handle CRLF
    if (char_recognizer->isNewline ((Unichar) c))
	return true;

    return false;
}

static bool is_character (unsigned char c,
			  CharacterRecognizer *char_recognizer)
{
#if 0
    // NOTE: These checks are meant to be temporal as they don't
    // take encoding into account.
    if ((c >= '0' && c <= '9') ||
	(c >= 'a' && c <= 'z') ||
	(c >= 'A' && c <= 'Z') ||
	(c == '_'))
    {
	return true;
    }

    return false;
#endif

    abortIf (char_recognizer == NULL);

    if (c == '_')
	return true;

    return char_recognizer->isAlphanumeric ((Unichar) c);
}

// A token is one of the following:
// a) a string of consequtive alphanumeric characters;
// b) a single non-alphanumeric character that is not whitespace.
ConstMemoryDesc
FileTokenStream::getNextToken ()
    throw (InternalException)
try {
    // NOTE: Changing the size of 'buf' is useful for debugging.
    //
    // [10.09.07] I guess 4096 is way too much. Worse than that,
    //            I don't fill satisfied with the way this class
    //            is implemented. It seems to be _very_ ineffective.
    unsigned char buf [4096];
    Uint64 buf_start_pos;

    token = NULL;

    for (;;) {
	buf_start_pos = file->tell ();

	unsigned long nread;
	IOResult res = file->read (MemoryDesc::forObject (buf), &nread);
	if (res == IOResultEof) {
	    if (!token.isNull ())
		return token->getMemoryDesc ();

	    return ConstMemoryDesc ();
	}

	if (res != IOResultNormal)
	    abortIfReached ();

	if (nread == 0)
	    continue;

	DEBUG (
	    errf->print ("MyLang.FileTokenStream.getNextToken:\n");
	    hexDump (errf, ConstMemoryDesc::forObject (buf).getRegion (0, nread));
	)

	// start_offset is offset to the first non-whitespace character in the buffer.
	unsigned long start_offset = 0;
	if (token.isNull ()) {
	  // We've got no bytes for the token so far. That means that we're
	  // in the process of skipping leading whitespace. Let's skip some more.

	    bool got_newline = false;

	    for (start_offset = 0; start_offset < nread; start_offset++) {
		if (is_newline (buf [start_offset], char_recognizer)) {
		    cur_line ++;
		    cur_line_start = buf_start_pos + start_offset + 1;
		    got_newline = true;
		} else
		if (!is_whitespace (buf [start_offset], char_recognizer)) {
		  // This is where we detect the beginning of a new token.

		    abortIf (buf_start_pos + start_offset < cur_line_start);
//		    cur_line_pos = (buf_start_pos + start_offset) - cur_line_start;
		    cur_line_pos = cur_line_start;
		    cur_char_pos = buf_start_pos + start_offset;

		    break;
		}
	    }

	    abortIf (start_offset > nread);

	    if (got_newline && report_newlines) {
		file->seekSet (buf_start_pos + start_offset);

		DEBUG (
		    errf->print ("MyLang.FileTokenStream.getNextToken: returning a newline").pendl ();
		)
		return ConstMemoryDesc::forString ("\n");
	    }

	    if (start_offset == nread) {
	      // All of the bytes read are whitespace. We need to read some more.
		continue;
	    }
	}

      // Searching for the whitespace after the token, which marks token's end.

	// The naming is now a bit confusing.
	// 'got_whsp' means that we've got an end of the token in the buffer,
	// hence we know the whole token and it can be returned to the caller.
	// 'whsp' is the offset from the start of the buffer to the end of the token.
	unsigned long whsp;
	bool got_whsp = false;
	if (!is_character (buf [start_offset], char_recognizer)) {
	  // We've got a symbol that is not a character nor whitespace.
	  // This symbol should constitute a separate token.
	  // Hence, if the token has already started, then this symbol marks
	  // the end of the token. Otherwise, the symbol is the token.

	    if (token.isNull ()) {
		whsp = start_offset + 1;
		got_whsp = true;
	    } else {
		whsp = start_offset;
		got_whsp = true;
	    }
	} else {
	    for (whsp = start_offset + 1; whsp < nread; whsp++) {
		if (!is_character (buf [whsp], char_recognizer)) {
		    got_whsp = true;
		    break;
		}
	    }
	}

	if (whsp > start_offset) {
	    {
	      // Checking for token length integer overflow

		unsigned long new_len = whsp - start_offset;
		if (!token.isNull ()) {
		    if (new_len + token->getLength () <= new_len)
			throw InternalException (String::forData ("Token length overflow"));

		    new_len += token->getLength ();
		}

	      // Checking for token length limit

		if (new_len > MYLANG__MAX_TOKEN_LENGTH) {
		    errf->print ("MyLang.FileTokenStream.getNextToken: "
				 "token length limit exceeded (")
			 .print ((unsigned long) MYLANG__MAX_TOKEN_LENGTH)
			 .print (" bytes max)")
			 .pendl ();

		    throw InternalException (String::forData ("Token length limit exceeded"));
		}
	    }

	    if (token.isNull ()) {
		token = grab (new String ());
		token->allocate (whsp - start_offset);
		copyMemory (token->getMemoryDesc (), ConstMemoryDesc (buf + start_offset, whsp - start_offset));
		token->getData () [whsp - start_offset] = 0;
	    } else {
		Ref<String> new_token = grab (new String ());
		new_token->allocate (token->getLength () + (whsp - start_offset));
		copyMemory (new_token->getMemoryDesc (),
			    ConstMemoryDesc (token->getData (), token->getLength ()));
		copyMemory (new_token->getMemoryDesc ().getRegionOffset (token->getLength ()),
			    ConstMemoryDesc (buf + start_offset, whsp - start_offset));
		new_token->getData () [token->getLength () + (whsp - start_offset)] = 0;

		token = new_token;
	    }
	}

	if (got_whsp) {
	    file->seekSet (buf_start_pos + whsp);

	  DEBUG_INT (
	    errf->print ("MyLang.FileTokenStream.getNextToken: token: \"").print (token).print ("\"")
		 .pendl ();
	  )
	    return token->getMemoryDesc ();
	}
    } /* for (;;) */

    abortIfReached ();
    return ConstMemoryDesc ();
} catch (Exception &exc) {
    throw InternalException (String::nullString (), exc.clone ());
}

void
FileTokenStream::getPosition (PositionMarker * const ret_pmark /* non-null */)
    throw (InternalException)
try {
    ret_pmark->body.offset = file->tell ();
    ret_pmark->body.cur_line = cur_line;
    ret_pmark->body.cur_line_start = cur_line_start;
} catch (Exception &exc) {
    throw InternalException (String::nullString (), exc.clone ());
}

void
FileTokenStream::setPosition (PositionMarker const * const pmark /* non-null */)
    throw (InternalException)
{
  DEBUG_FLO (
    errf->print ("MyLang.FileTokenStream.setPosition")
	 .pendl ();
  )

    cur_line = pmark->body.cur_line;
    cur_line_start = pmark->body.cur_line_start;

    try {
	file->seekSet (pmark->body.offset);
    } catch (Exception &exc) {
	throw InternalException (String::nullString (), exc.clone ());
    }
}

FilePosition
FileTokenStream::getFilePosition ()
    throw (InternalException)
{
    return FilePosition (cur_line /* line */,
			 cur_line_pos /* line_pos */,
			 cur_char_pos /* char_pos */);
}

unsigned long
FileTokenStream::getLine ()
{
    return cur_line;
}

Ref<String>
FileTokenStream::getLineStr ()
    throw (InternalException)
try {
    Uint64 tmp_pos = file->tell ();
    file->seekSet (cur_line_start);

    Ref<Buffer> buf = grab (new Buffer (4096 + 1));
    unsigned long line_len = file->readLine (buf->getMemoryDesc (0, buf->getSize ()));

    file->seekSet (tmp_pos);

    return grab (new String (buf->getMemoryDesc (0, line_len)));
} catch (Exception &exc) {
    throw InternalException (String::nullString (), exc.clone ());
}

FileTokenStream::FileTokenStream (File *file,
				  CharacterRecognizer *char_recognizer,
				  bool report_newlines)
{
    if (file == NULL)
	abortIfReached ();

    this->file = file;

    if (char_recognizer == NULL)
	this->char_recognizer = grab (new CharacterRecognizer);
    else
	this->char_recognizer = char_recognizer;

    this->report_newlines = report_newlines;

    cur_line = 0;
    cur_line_start = 0;
    cur_line_pos = 0;
    cur_char_pos = 0;
}

}

