# -*- coding: utf-8 -*-
#
# Copyright 2022 Johannes Demel.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
#

option(ENABLE_DOXYGEN_PDF "Build Doxygen PDF" OFF)
find_package(Doxygen)
if(DOXYGEN_FOUND)
message(STATUS "Doxygen found. Building docs ...")
find_package(LATEX COMPONENTS PDFLATEX)

if(ENABLE_DOXYGEN_PDF AND LATEX_FOUND)
    set(enable_pdf_docs YES)
    message(STATUS "latex found. Building PDF docs ...")
else()
    set(enable_pdf_docs NO)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_BINARY_DIR}/Doxyfile
@ONLY)

add_custom_target(volk_doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating documentation with Doxygen" VERBATIM
)

add_custom_target(volk_pdf_doc
    make
    DEPENDS volk_doc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/latex
    COMMENT "Generating PDF documentation with Doxygen" VERBATIM
)

endif(DOXYGEN_FOUND)
