/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file src/modules/functions/graphix/graphix.cpp
///\brief Implementation of graphix functions

#include "serialize/struct/structDescription.hpp"
#include "graphix.hpp"
#include <string>
#include <vector>
#include <sstream>

#include "types/base64.hpp"

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif
#include "FreeImage.h"
#include "FreeImagePlus.h"

using namespace _Wolframe;
using namespace graphix;

namespace {

struct ImageDescription : public serialize::StructDescription<Image>
{
	ImageDescription( )
	{
		( *this )
		( "data", &Image::data );
	}
};

struct ImageInfoDescription : public serialize::StructDescription<ImageInfo>
{
	ImageInfoDescription( )
	{
		( *this )
		( "width", &ImageInfo::width )
		( "height", &ImageInfo::height );
	}
};

struct ImageThumbDescription : public serialize::StructDescription<ImageThumb>
{
	ImageThumbDescription( )
	{
		( *this )
		( "image", &ImageThumb::image )
		( "size", &ImageThumb::size );
	}
};

struct ImageRescaleDescription : public serialize::StructDescription<ImageRescale>
{
	ImageRescaleDescription( )
	{
		( *this )
		( "image", &ImageRescale::image )
		( "width", &ImageRescale::width )
		( "height", &ImageRescale::height );
	}
};

} // anonymous namespace

const serialize::StructDescriptionBase *Image::getStructDescription( )
{
	static ImageDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageInfo::getStructDescription( )
{
	static ImageInfoDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageThumb::getStructDescription( )
{
	static ImageThumbDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase *ImageRescale::getStructDescription( )
{
	static ImageRescaleDescription rt;
	return &rt;
}

std::string ImageImpl::decode( const std::string &data )
{
	base64::Decoder decoder;
	std::istringstream i( data );
	std::ostringstream o;
	decoder.decode( i, o );
	return o.str( );
}

std::string ImageImpl::encode( const std::string &data )
{
	base64::Encoder encoder;
	std::istringstream i( data );
	std::ostringstream o;
	encoder.encode( i, o );
	return o.str( );
}

int ImageImpl::info( proc::ExecContext*, ImageInfo &res, const Image &param )
{
// decode
	std::string raw;
	raw = decode( param.data );

// copy it into a buffer
	std::vector<char> buf;
	buf.assign( raw.begin( ), raw.end( ) );

// create freeimage memory handle
	fipMemoryIO memIO( (BYTE *)&buf[0], buf.size( ) );

// load image from buffer
	fipImage image;
	image.loadFromMemory( memIO );

// get info about the image
	res.width = image.getWidth( );
	res.height = image.getHeight( );

	return 0;
}

int ImageImpl::thumb( proc::ExecContext*, Image &res, const ImageThumb &param )
{
// decode
	std::string raw;
	raw = decode( param.image.data );

// copy it into a buffer
	std::vector<char> buf;
	buf.assign( raw.begin( ), raw.end( ) );

// create freeimage memory handle
	fipMemoryIO inIO( (BYTE *)&buf[0], buf.size( ) );

// load image from buffer
	fipImage image;
	image.loadFromMemory( inIO );

// make thumbnail
	fipImage thumb( image );
	thumb.makeThumbnail( param.size, true );

// create freeimage memory handle for result
	fipMemoryIO outIO;

// write thumb into the buffer
	thumb.saveToMemory( FIF_PNG, outIO );

// get buffer
	BYTE *thumbData = NULL;
	DWORD thumbSize = 0;
	outIO.acquire( &thumbData, &thumbSize );

// encode
	std::string rawRes( (char *)thumbData, thumbSize );
	res.data = encode( rawRes );

	return 0;
}

int ImageImpl::rescale( proc::ExecContext*, Image &res, const ImageRescale &param )
{
// decode
	std::string raw;
	raw = decode( param.image.data );

// copy it into a buffer
	std::vector<char> buf;
	buf.assign( raw.begin( ), raw.end( ) );

// create freeimage memory handle
	fipMemoryIO inIO( (BYTE *)&buf[0], buf.size( ) );

// load image from buffer
	fipImage image;
	image.loadFromMemory( inIO );

// make thumbnail
	fipImage thumb( image );
	thumb.rescale( param.width, param.height, FILTER_BOX );

// create freeimage memory handle for result
	fipMemoryIO outIO;

// write thumb into the buffer
	thumb.saveToMemory( FIF_PNG, outIO );

// get buffer
	BYTE *thumbData = NULL;
	DWORD thumbSize = 0;
	outIO.acquire( &thumbData, &thumbSize );

// encode
	std::string rawRes( (char *)thumbData, thumbSize );
	res.data = encode( rawRes );

	return 0;
}
