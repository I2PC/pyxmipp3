#***************************************************************************
# Authors:     Oier Lauzirika Zarrabeitia (oierlauzi@bizkaia.eu)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
# 02111-1307  USA
#
#  All comments concerning this program package may be sent to the
#  e-mail address 'xmipp@cnb.csic.es'
# ***************************************************************************

include(ExternalProject)

function(fetch_libsvm)
	cmake_policy(SET CMP0135 NEW) # To avoid warnings
	FetchContent_Declare(
  		libsvm
  		URL https://github.com/cossorzano/libsvm/archive/refs/heads/master.tar.gz
	)
	FetchContent_MakeAvailable(libsvm)

	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	add_library(libsvm STATIC ${libsvm_SOURCE_DIR}/svm.cpp)
	target_include_directories(libsvm PUBLIC ${libsvm_SOURCE_DIR})
endfunction()