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


#ifndef __MYLANG__CHARACTER_RECOGNIZER_H__
#define __MYLANG__CHARACTER_RECOGNIZER_H__

#include <mycpp/object.h>
#include <mycpp/iterator.h>
#include <mycpp/unicode.h>

namespace MyLang {

using namespace MyCpp;

class CharacterRecognizer : public virtual SimplyReferenced
{
public:
    virtual bool isAlphanumeric (Unichar c);

    virtual bool isAlpha (Unichar c);

    virtual bool isDigit (Unichar c);

    virtual bool isSpace (Unichar c);

    virtual FuzzyResult isNewline (Unichar c);

    virtual FuzzyResult isNewline (Iterator<Unichar const &> &char_iter,
				   Size *ret_num_chars);
};

}

#endif /* __MYLANG__CHARACTER_RECOGNIZER_H__ */

