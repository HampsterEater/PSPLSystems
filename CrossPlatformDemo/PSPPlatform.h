// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#ifndef _pspplatform_h_
#define _pspplatform_h_

// Include general libraries.
#include "main.h"
#include "Common.h"
#include "Cube.h"
//#include "Platform.h"

#ifdef PSP

// Include PSP Libs.
#include <psptypes.h>
#include <kernel.h>
#include <displaysvc.h>
#include <ctrlsvc.h>
#include <libgu.h>
#include <libgum.h>
#include <libwave.h>
#include <rtcsvc.h>
#include <libfpu.h>
#include <libsas.h>

int main();

#endif

#endif
