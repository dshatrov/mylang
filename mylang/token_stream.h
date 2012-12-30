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


#ifndef __MYLANG__TOKEN_STREAM_H__
#define __MYLANG__TOKEN_STREAM_H__

#include <cstring>

#include <mycpp/object.h>
#include <mycpp/string.h>
#include <mycpp/util.h>
#include <mycpp/internal_exception.h>

#include <mylang/file_position.h>

namespace MyLang {

using namespace MyCpp;

class TokenStream : public virtual SimplyReferenced
{
public:
    // Simplicity is sacrificed for efficiency here.
    // The point is to be able to use PositionMarkers as stack-allocated objects
    // and pass them around (almost) by value. But, in some cases, position
    // markers need to be more complex than the generalized form, hence we
    // provide a mechanism for implementing such complex markers (ptr,
    // copy_func, release_func).
    class PositionMarker
    {
    public:
	// Called for copying (copy constructor and assignment operator).
	// 'from->body.copy_func' is called for this purpose.
	typedef void (*CopyFunc) (PositionMarker       *to   /* non-null */,
				  PositionMarker const *from /* non-null */);

	// Destructor callback.
	typedef void (*ReleaseFunc) (PositionMarker *pmark /* non-null */);

	// POD type for memcpy()
	struct Body {
	    CopyFunc copy_func;
	    ReleaseFunc release_func;

	    Uint64 offset;
	    unsigned long cur_line;
	    Uint64 cur_line_start;

	    void *ptr;
	};

	Body body;

	PositionMarker& operator = (PositionMarker const &pmark)
	{
	    if (this == &pmark)
		return *this;

	    if (body.release_func != NULL)
		body.release_func (this);

	    if (pmark.body.copy_func != NULL)
		pmark.body.copy_func (this, &pmark);
	    else
		memcpy (&this->body, &pmark.body, sizeof (body));

	    return *this;
	}

	PositionMarker (PositionMarker const &pmark)
	{
	    if (pmark.body.copy_func != NULL) {
		pmark.body.copy_func (this, &pmark);
	    } else
		memcpy (&this->body, &pmark.body, sizeof (body));
	}

	PositionMarker ()
	{
	    body.copy_func = NULL;
	    body.release_func = NULL;
	}

	~PositionMarker ()
	{
	    if (body.release_func != NULL)
		body.release_func (this);
	}
    };

    // If newlines are reported, then "\n" string will be returned for any whitespace
    // which contains one or more newlines (see newline(), isNewline() methods).
//    virtual Ref<String> getNextToken ()
//			       throw (InternalException) = 0;
    // The memory returned stays valid till the next call to getNextToken().
    virtual ConstMemoryDesc getNextToken () = 0;

    // User-defined objects may be associated with tokens by lower-level tokenizers.
//    virtual Ref<String> getNextToken (Ref<SimplyReferenced> *ret_user_obj,
//				      void **ret_user_ptr)
//			       throw (InternalException);
    virtual ConstMemoryDesc getNextToken (Ref<SimplyReferenced> *ret_user_obj,
					  void **ret_user_ptr)
				   throw (InternalException);

    virtual void getPosition (PositionMarker *ret_pmark /* non-null */)
		       throw (InternalException) = 0;

    virtual void setPosition (PositionMarker const *pmark /* non-null */)
		       throw (InternalException) = 0;

    virtual FilePosition getFilePosition ()
				   throw (InternalException) = 0;

    // TODO ?
    virtual unsigned long getLine ()
    {
	return 0;
    }

    // TODO ?
    virtual Ref<String> getLineStr ()
			     throw (InternalException)
    {
	return String::nullString ();
    }

  // Non-virtual methods

    static Ref<String> newline ()
    {
	return String::forData ("\n");
    }

    static bool isNewline (ConstMemoryDesc const &token)
    {
	return compareByteArrayToString (token, "\n") == ComparisonEqual;
    }

    static bool isNewline (String *str)
    {
	abortIf (str == NULL);
	return isNewline (str->getMemoryDesc ());
    }
};

}

#endif /* __MYLANG__TOKEN_STREAM_H__ */

