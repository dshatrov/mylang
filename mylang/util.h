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


#ifndef __MYLANG__UTIL_H__
#define __MYLANG__UTIL_H__

#include <mycpp/unicode.h>
#include <mycpp/file.h>
#include <mycpp/internal_exception.h>

#include <mylang/file_position.h>

namespace MyLang {

using namespace MyCpp;

void printErrorContext (File *out_file,
			File *file,
			FilePosition const &fpos)
		 throw (InternalException);

}

#endif /* __MYLANG__UTIL_H__ */

