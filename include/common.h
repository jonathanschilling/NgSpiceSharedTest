/*
 * common.h
 *
 * extracted from the KiCad's common.h
 *
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2014-2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2008 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 *  Created on: 2018-10-14
 *      Author: Jonathan Schilling (jonathan.schilling@ipp.mpg.de)
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <atomic>
#include <string>

class LOCALE_IO {
public:
    LOCALE_IO();
    ~LOCALE_IO();

private:
    // allow for nesting of LOCALE_IO instantiations
    static std::atomic<unsigned int> m_c_count;

    // The locale in use before switching to the "C" locale
    // (the locale can be set by user, and is not always the system locale)
    std::string m_user_locale;
};


#endif /* COMMON_H_ */
