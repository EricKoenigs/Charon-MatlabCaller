/*  Copyright (C) 2011 Eric Koenigs

	This file is part of Charon.

	Charon is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Charon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with Charon.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file MatlabCaller.h
 *  Declaration of the parameter class MatlabCaller.
 *  \author <a href="mailto:e.koenigs@stud.uni-heidelberg.de">
 *      Eric Koenigs</a>
 *  \date 20.09.2011
 */

#ifndef _MATLABCALLER_H_
#define _MATLABCALLER_H_

#ifdef _MSC_VER
#ifdef matlabcaller_EXPORTS
/// Visual C++ specific code
#define matlabcaller_DECLDIR __declspec(dllexport)
#else
#define matlabcaller_DECLDIR __declspec(dllimport)
#endif /*Export or import*/
#else
/// Not needed without MSVC
#define matlabcaller_DECLDIR
#endif


#include <charon-core/ParameteredObject.h>
#include <charon-utils/CImg.h>
#include "engine.h"
#include "matrix.h"

/// Uses the Matlab engine and a converter to use Matlab functions for image
/// processing
/** This plugin uses the Matlab API and the CImg2mx converter to use Matlab
 *  scripts for image processing. It takes a CImgList, a path to a .m file and
 *  a vector of arguments for the Matlab function, then converts all CImg
 *  objects in the CImgList to mxArrays and tries to call the function on them
 *  inside the Matlab engine. Afterwards it attempts to convert the mxArrays
 *  back into CImg objects and return them as a CImgList.
 */
template <typename T>
class matlabcaller_DECLDIR MatlabCaller :
		public TemplatedParameteredObject<T> {
public:
	/// default constructor
	/// \param name          Instance name
	MatlabCaller(const std::string& name = "");

	/// A CImgList
	InputSlot< cimg_library::CImgList<T> > imgIn;

	/// An Engine pointer
	InputSlot< Engine* > engine;

	/// A CImgList
	OutputSlot< cimg_library::CImgList<T> > imgOut;

	/// A .m file
	Parameter< std::string > functionFile;
	/// Parameters for the Matlab function
	ParameterList< std::string > functionParams;

	/// Update object.
	virtual void execute();
};

#endif // _MATLABCALLER_H_
