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

#include <mylang/character_recognizer.h>

#define DEBUG(a) ;

using namespace MyCpp;

namespace MyLang {

bool
CharacterRecognizer::isAlphanumeric (Unichar c)
{
    return unicode_isAlphanumeric (c);
}

bool
CharacterRecognizer::isAlpha (Unichar c)
{
    return unicode_isAlpha (c);
}

bool
CharacterRecognizer::isDigit (Unichar c)
{
    return unicode_isDigit (c);
}

bool
CharacterRecognizer::isSpace (Unichar c)
{
    return unicode_isSpace (c);
}

FuzzyResult
CharacterRecognizer::isNewline (Unichar c)
{
    DEBUG (
	errf->print ("MyLang.CharacterRecognizer.isNewline: ").print (unicode_isNewline (c) ? "true" : "false").pendl ();
    )
    return unicode_isNewline (c);
}

FuzzyResult
CharacterRecognizer::isNewline (Iterator<Unichar const &> &char_iter,
				Size *ret_num_chars)
{
    return unicode_isNewline (char_iter, ret_num_chars);
}

}

