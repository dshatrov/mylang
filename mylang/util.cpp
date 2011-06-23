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


#include <mycpp/utf8.h>
#include <mycpp/io.h>

#include <mylang/util.h>

using namespace MyCpp;

namespace MyLang {
    
bool
isNewline (Unichar uc)
    throw (InternalException)
{
  // FIXME This is ugly :) Just compare to the code point value for '\n'.

    const char *newline = "\n";
    if (!utf8_validate_sz (newline, NULL))
	throw InternalException ();

    Unichar newline_uc = utf8_valid_to_unichar (newline);
    if (uc == newline_uc)
	return true;

    return false;
}

// Returns 'true' if we've got more preceding lines.
static bool 
seek_to_line_start (File *file)
    throw (InternalException)
try {
    abortIf (file == NULL);

    bool first_read = true;
    for (;;) {
	Byte buf [4096];
	Size toread = sizeof (buf);

	Uint64 fpos = file->tell ();
	if (fpos == 0)
	    return false;

	if (fpos < toread)
	    toread = fpos;

	file->seekSet (fpos - toread);

	Size nread;
	IOResult res = file->fullRead (MemoryDesc::forObject (buf).getRegion (0, toread), &nread);
	abortIf (res == IOResultAgain);

	if (res == IOResultEof) {
	  // We expect that if we've seeked N bytes back, then we'll
	  // definitely get those N bytes from fullRead().
	    throw InternalException ();
	}

	abortIf (res != IOResultNormal);
	abortIf (nread == 0 || nread > toread);

	if (nread < toread) {
	  // We expect that if we've seeked N bytes back, then we'll
	  // definitely get those N bytes from fullRead().
	    throw InternalException ();
	}

	Size i = nread - 1;
	for (;;) {
	    if (buf [i] == '\n') {
		if (! (first_read && i == nread - 1)) {
		    file->seekSet (fpos - toread + i + 1);
		    return true;
		}
	    }

	    if (i == 0)
		break;
	    i --;
	}

	file->seekSet (fpos - toread);
	first_read = false;
    }

    abortIfReached ();
    return false;
} catch (Exception &exc) {
    throw InternalException (String::nullString (), exc.clone ());
}

// Note: File position for 'file' is changed after invocation of printErrorContext().
void
printErrorContext (File *out_file,
		   File *file,
		   FilePosition const &fpos)
    throw (InternalException)
try {
    abortIf (out_file == NULL ||
	     file == NULL);

  // 1. Find the beginning of a line, searching backwards;
  // 2. Scan back for N lines of context;
  // 3. Print the context and the failing line, indicating
  //    the exact position of the error;
  // 4. Print the next N lines of context.

    file->seekSet (fpos.char_pos);

    const Size lines_of_context = 3;
    Uint64 context_pos [lines_of_context * 2 + 1];
    Size num_lines = 0;

    if (fpos.line_pos != fpos.char_pos) {
      // Seeking to the start of the current line if we're not there yet.
	seek_to_line_start (file);
    }

    context_pos [num_lines] = fpos.line_pos;
    num_lines ++;

    if (fpos.line_pos > 0) {
      // There should be some preceeding lines.

	for (Size i = 0; i < lines_of_context; i++) {
	    bool got_more_backlines = seek_to_line_start (file);
	    context_pos [num_lines] = file->tell ();
	    num_lines ++;

	    if (!got_more_backlines)
		break;
	}
    }

    abortIf (num_lines <= 0);
    for (Size i = num_lines - 1; ; i--) {
	file->seekSet (context_pos [i]);
	Byte buf [4096];
	// TODO End of line symbols may be lost for long lines.
	Size line_len = file->readLine (MemoryDesc::forObject (buf));
	if (i > 0)
	    out_file->print ("---> ");
	else
	    out_file->print ("===> ");

	out_file->print (MemoryDesc::forObject (buf).getRegion (0, line_len));

	if (i == 0)
	    break;
    }

    out_file->print ("     ");
    abortIf (fpos.char_pos < fpos.line_pos);
    // TODO Honour tabs
    for (Size i = 0; i < fpos.char_pos - fpos.line_pos; i++)
	out_file->print (" ");
    out_file->print ("^").pendl ();

    // TODO Currently, if a line is longer than sizeof(buf), then we read it
    // incorrectly: the part that didn't fit into the buffer is treated as
    // the next line.
    for (Size i = 0; i < lines_of_context; i++) {
	Byte buf [4096];
	Size line_len = file->readLine (MemoryDesc::forObject (buf));
	if (line_len == 0)
	    break;

	out_file->print ("+++> ").print (MemoryDesc::forObject (buf).getRegion (0, line_len));
    }

} catch (Exception &exc) {
    throw InternalException (String::nullString (), exc.clone ());
}

}

