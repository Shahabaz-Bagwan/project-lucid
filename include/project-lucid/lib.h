#pragma once

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/mstream.h"
#include "wx/quantize.h"
#include "wx/stopwatch.h"
#include "wx/wfstream.h"

#if wxUSE_CLIPBOARD
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#endif // wxUSE_CLIPBOARD

#include <wx/artprov.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/quantize.h>
#include <wx/scopedptr.h>
#include <wx/spinctrl.h>
#include <wx/stopwatch.h>
#include <wx/versioninfo.h>
#include <wx/wfstream.h>

#include <wx/scrolwin.h>

#if wxUSE_CLIPBOARD
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#endif // wxUSE_CLIPBOARD

#if defined( __WXMSW__ )
#ifdef wxHAVE_RAW_BITMAP
#include "wx/rawbmp.h"
#endif
#endif

#if defined( __WXMAC__ ) || defined( __WXGTK__ )
#define wxHAVE_RAW_BITMAP
#include "wx/rawbmp.h"
#endif