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
/** \file MatlabCaller.hxx
 *  Implementation of the parameter class MatlabCaller.
 *  \author <a href="mailto:e.koenigs@stud.uni-heidelberg.de">
 *      Eric Koenigs</a>
 *  \date 20.09.2011
 */

#ifndef _MATLABCALLER_HXX_
#define _MATLABCALLER_HXX_

#include <charon-core/ParameteredObject.hxx>
#include "MatlabCaller.h"
#include <iostream>
#include "converter.hxx"

template <typename T>
MatlabCaller<T>::MatlabCaller(const std::string& name) :
		TemplatedParameteredObject<T>(
			"MatlabCaller", name,
			"<h2>Uses the Matlab engine and a converter to use Matlab "
			"functions for image processing</h2><br>"
			"This plugin uses the Matlab API and the CImg2mx converter to "
			"use Matlab scripts for image processing. It takes a CImgList, a "
			"path to a .m file and a vector of arguments for the Matlab "
			"function, then converts all CImg objects in the CImgList to "
			"mxArrays and tries to call the function on them inside the "
			"Matlab engine. Afterwards it attempts to convert the mxArrays "
			"back into CImg objects and return them as a CImgList."
		)
{
	ParameteredObject::_addInputSlot(
		imgIn, "imgIn",
		"A CImgList",
		"CImgList<T>");

	ParameteredObject::_addInputSlot(
			engine, "engine",
			"A Matlab Engine pointer",
			"Engine*");

	ParameteredObject::_addOutputSlot(
		imgOut, "imgOut",
		"A CImgList",
		"CImgList<T>");

	ParameteredObject::_addParameter(
		functionFile, "functionFile",
		"A .m file",
		"fileopen");
	ParameteredObject::_addParameter(
		functionParams, "functionParams",
		"Parameters for the Matlab function",
		"string");

}

template <typename T>
void MatlabCaller<T>::execute() {
	PARAMETEREDOBJECT_AVOID_REEXECUTION;
	ParameteredObject::execute();

	const cimg_library::CImgList<T> in = imgIn();
	cimg_library::CImgList<T> out(in.size());

	Engine* ep = engine();

	// Catch Segfaults before they happen.
	if (ep == NULL) {
		throw std::runtime_error("Matlab Caller received a NULL pointer "
				"instead of an Engine pointer. Aborting.");
	}

	// get the path and the filename from the full path
	std::string fullPath = functionFile();
	size_t lastSep = fullPath.rfind("/");
	std::string mFile = fullPath.substr(
		lastSep + 1
	);
	mFile = mFile.substr(0, mFile.size() - 2);
	// The filepath
	std::string mPath = fullPath.substr(
		0,
		lastSep
	);

	// Get the parameters from the paramater list
	// and format them into a comma seperated string.
	std::stringstream params;
	params << "";
	if (functionParams.size() > 0) {
		params	<< ", "
				<< functionParams[0];
	};
	if (functionParams.size() > 1) {
		for (unsigned int i = 1; i < functionParams.size(); i++) {
			params	<< ", "
					<< functionParams[i];
		};
	};

	// Try to change directory to the Matlab script.
	std::stringstream cmd;
	cmd	<< "cd "
		<< mPath;
	if (engEvalString(ep, cmd.str().c_str()) != 0) {
		std::stringstream err;
		err	<< "Matlab engine failed to change into directory "
			<< mPath
			<< ". Aborting.";
		// Try to close Matlab engine before bailing out.
		engClose(ep);
		// Bai.
		throw std::runtime_error(err.str());
	};

	
	// Try conversion and execution.
	mxArray* mxA;
	cimglist_for(in, l) {
		mxA = CImg2mx<T>(in[l]);
		engPutVariable(ep, "img", mxA);

		std::stringstream cmd;
		cmd	<< "img = "
			<< mFile
			<< "(img"
			<< params.str()
			<< ")";
		sout	<< "Executing the following command in Matlab: \""
				<< cmd.str()
				<< "\""
				<< std::endl;
		engEvalString(ep, cmd.str().c_str());

		mxA = engGetVariable(ep, "img");
		
		out[l] = mx2CImg<T>(mxA);
	};


	imgOut() = out;

}

#endif /* _MATLABCALLER_HXX_ */
