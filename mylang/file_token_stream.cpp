/*  MyLang - Utility library for writing parsers
    Copyright (C) 2011, 2012 Dmitry Shatrov

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
#include <mycpp/unicode.h>

#include <mylang/file_token_stream.h>


#define DEBUG(a) ;
// Flow
#define DEBUG_FLO(a) ;
// Internal
#define DEBUG_INT(a) ;

#define MYLANG__MAX_TOKEN_LENGTH 1024


using namespace MyCpp;

namespace MyLang {

static inline bool
is_whitespace (unsigned char const c)
{
    return unicode_isSpace ((Unichar) c);
}

static inline bool
is_newline (unsigned char const c)
{
    // TODO Handle CRLF
    return unicode_isNewline ((Unichar) c) != FuzzyResult::No;
}

static bool is_character (unsigned char const c,
                          bool          const minus_is_alpha)
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

    if (c == '_' || (minus_is_alpha && c == '-'))
	return true;

    return unicode_isAlphanumeric ((Unichar) c);
}

// A token is one of the following:
// a) a string of consequtive alphanumeric characters;
// b) a single non-alphanumeric character that is not whitespace.
mt_throws M::Result
FileTokenStream::getNextToken (M::ConstMemory * const ret_mem)
try {
    // TODO This code is inefficient becasue it uses read(4Kb)+seek for
    //      every token. Using CachedFile is not enough because there's still
    //      a lot of unnecessary memory copying (bulk reads).
    //      Lexing logics should really be monolithic.

    // NOTE: Changing the size of 'buf' is useful for debugging.
    //
    // [10.09.07] I guess 4096 is way too much.
    unsigned char buf [4096];
    Uint64 buf_start_pos;

    token_len = 0;

    for (;;) {
	buf_start_pos = file->tell ();

	unsigned long nread;
	IOResult res = file->read (MemoryDesc::forObject (buf), &nread);
	if (res == IOResultEof) {
	    if (token_len > 0) {
                *ret_mem = M::ConstMemory (token_buf, token_len);
                return M::Result::Success;
            }

            *ret_mem = M::ConstMemory();
            return M::Result::Success;
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
	if (token_len == 0) {
	  // We've got no bytes for the token so far. That means that we're
	  // in the process of skipping leading whitespace. Let's skip some more.

	    bool got_newline = false;

	    for (start_offset = 0; start_offset < nread; start_offset++) {
		if (is_newline (buf [start_offset])) {
		    cur_line ++;
		    cur_line_start = buf_start_pos + start_offset + 1;
		    got_newline = true;
		} else
		if (!is_whitespace (buf [start_offset])) {
		  // This is where we detect the beginning of a new token.

		    abortIf (buf_start_pos + start_offset < cur_line_start);
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
                *ret_mem = M::ConstMemory ("\n");
                return M::Result::Success;
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
	if (!is_character (buf [start_offset], minus_is_alpha)) {
	  // We've got a symbol that is not a character nor whitespace.
	  // This symbol should constitute a separate token.
	  // Hence, if the token has already started, then this symbol marks
	  // the end of the token. Otherwise, the symbol is the token.

	    if (token_len == 0) {
		whsp = start_offset + 1;
		got_whsp = true;
	    } else {
		whsp = start_offset;
		got_whsp = true;
	    }
	} else {
	    for (whsp = start_offset + 1; whsp < nread; whsp++) {
		if (!is_character (buf [whsp], minus_is_alpha)) {
		    got_whsp = true;
		    break;
		}
	    }
	}

	if (whsp > start_offset) {
            if (token_len + (whsp - start_offset) > max_token_len)
                throw InternalException (String::forData ("Token length limit exceeded"));

            memcpy (token_buf + token_len, buf + start_offset, whsp - start_offset);
            token_len += (whsp - start_offset);
	}

	if (got_whsp) {
	    file->seekSet (buf_start_pos + whsp);

	  DEBUG_INT (
	    errf->print ("MyLang.FileTokenStream.getNextToken: token: \"").print (ConstMemoryDesc (token_buf, token_len)).print ("\"")
		 .pendl ();
	  )
            *ret_mem = M::ConstMemory (token_buf, token_len);
            return M::Result::Success;
	}
    } /* for (;;) */

    unreachable ();
    return M::Result::Failure;
} catch (Exception & /* exc */) {
    M::exc_throw <M::InternalException> (M::InternalException::BackendError);
    return M::Result::Failure;
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

FileTokenStream::FileTokenStream (File * const file,
				  bool   const report_newlines,
                                  bool   const minus_is_alpha,
                                  Size   const max_token_len)
    : file            (file),
      report_newlines (report_newlines),
      minus_is_alpha  (minus_is_alpha),
      cur_line        (0),
      cur_line_start  (0),
      cur_line_pos    (0),
      cur_char_pos    (0),
      token_len       (0),
      max_token_len  (max_token_len)
{
    abortIf (file == NULL);

    token_buf = new Byte [max_token_len];
}

FileTokenStream::~FileTokenStream ()
{
    delete token_buf;
}

}

