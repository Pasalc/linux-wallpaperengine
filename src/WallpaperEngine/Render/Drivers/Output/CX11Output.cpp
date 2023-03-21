#include "common.h"
#include "CX11Output.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

using namespace WallpaperEngine::Render::Drivers::Output;

XErrorHandler originalErrorHandler;

void CustomXIOErrorExitHandler (Display* dsp, void* userdata)
{
    auto context = static_cast <CX11Output*> (userdata);

#if !NDEBUG
    sLog.debugerror ("Critical XServer error detected. Attempting to recover...");
#endif /* DEBUG */

    // refetch all the resources
    context->reset ();
}

int CustomXErrorHandler (Display* dpy, XErrorEvent* event)
{
#if !NDEBUG
    sLog.debugerror ("Detected X error");
#endif /* DEBUG */

    // call the original handler so we can keep some information reporting
    originalErrorHandler (dpy, event);

    return 0;
}

int CustomXIOErrorHandler (Display* dsp)
{
#if !NDEBUG
    sLog.debugerror ("Detected X error");
#endif /* DEBUG */

    return 0;
}

CX11Output::CX11Output (CApplicationContext& context, CVideoDriver& driver) :
	COutput (context),
	m_driver (driver)
{
	this->m_display = XOpenDisplay (nullptr);
	this->loadScreenInfo ();
}
CX11Output::~CX11Output ()
{
	this->free ();
	XCloseDisplay (this->m_display);
}

void CX11Output::reset ()
{
	// first free whatever we have right now
	this->free ();
	// re-load screen info
	this->loadScreenInfo ();
}

void CX11Output::free ()
{
	// free all the resources we've got
	XDestroyImage (this->m_image);
	XFreeGC (this->m_display, this->m_gc);
	XFreePixmap (this->m_display, this->m_pixmap);
	delete this->m_imageData;
}

void* CX11Output::getImageBuffer () const
{
	return this->m_imageData;
}

bool CX11Output::renderVFlip () const
{
	return false;
}

bool CX11Output::renderMultiple () const
{
	return this->m_viewports.size () > 1;
}

bool CX11Output::haveImageBuffer () const
{
	return true;
}

void CX11Output::loadScreenInfo ()
{
	// reset the viewports
	this->m_viewports.clear ();

    // set the error handling to try and recover from X disconnections
#ifdef HAVE_XSETIOERROREXITHANDLER
    XSetIOErrorExitHandler (this->m_display, CustomXIOErrorExitHandler, this);
#endif /* HAVE_XSETIOERROREXITHANDLER */
    originalErrorHandler = XSetErrorHandler (CustomXErrorHandler);
    XSetIOErrorHandler (CustomXIOErrorHandler);

	int xrandr_result, xrandr_error;

	if (!XRRQueryExtension (this->m_display, &xrandr_result, &xrandr_error))
	{
		sLog.error ("XRandr is not present, cannot detect specified screens, running in window mode");
		return;
	}

	this->m_root = DefaultRootWindow (this->m_display);
	this->m_fullWidth = DisplayWidth (this->m_display, DefaultScreen (this->m_display));
	this->m_fullHeight = DisplayHeight (this->m_display, DefaultScreen (this->m_display));
	XRRScreenResources* screenResources = XRRGetScreenResources (this->m_display, DefaultRootWindow (this->m_display));

	if (screenResources == nullptr)
	{
		sLog.error ("Cannot detect screen sizes using xrandr, running in window mode");
		return;
	}

	for (int i = 0; i < screenResources->noutput; i ++)
	{
		XRROutputInfo* info = XRRGetOutputInfo (this->m_display, screenResources, screenResources->outputs [i]);

		// screen not in use, ignore it
		if (info == nullptr || info->connection != RR_Connected)
			continue;

		// only keep info of registered screens
		if (this->m_context.screenSettings.find (info->name) == this->m_context.screenSettings.end ())
			continue;

		XRRCrtcInfo* crtc = XRRGetCrtcInfo (this->m_display, screenResources, info->crtc);

		sLog.out ("Found requested screen: ", info->name, " -> ", crtc->x, "x", crtc->y, ":", crtc->width, "x", crtc->height);

		this->m_viewports [info->name] =
		{
			{crtc->x, crtc->y, crtc->width, crtc->height},
			info->name
		};

		XRRFreeCrtcInfo (crtc);
	}

	XRRFreeScreenResources (screenResources);

	// create pixmap so we can draw things in there
	this->m_pixmap = XCreatePixmap (this->m_display, this->m_root, this->m_fullWidth, this->m_fullHeight, 24);
	this->m_gc = XCreateGC (this->m_display, this->m_pixmap, 0, nullptr);
	// pre-fill it with black
	XFillRectangle (this->m_display, this->m_pixmap, this->m_gc, 0, 0, this->m_fullWidth, this->m_fullHeight);
	// set the window background as our pixmap
	XSetWindowBackgroundPixmap (this->m_display, this->m_root, this->m_pixmap);
	// allocate space for the image's data
	this->m_imageData = new char [this->m_fullWidth * this->m_fullHeight * 4];
	// create an image so we can copy it over
	this->m_image = XCreateImage (this->m_display, CopyFromParent, 24, ZPixmap, 0, this->m_imageData, this->m_fullWidth, this->m_fullHeight, 32, 0);
	// setup driver's render changing the window's size
	this->m_driver.resizeWindow ({this->m_fullWidth, this->m_fullHeight});
}

void CX11Output::updateRender () const
{
    // put the image back into the screen
    XPutImage (this->m_display, this->m_pixmap, this->m_gc, this->m_image, 0, 0, 0, 0, this->m_fullWidth, this->m_fullHeight);

    // _XROOTPMAP_ID & ESETROOT_PMAP_ID allow other programs (compositors) to
    // edit the background. Without these, other programs will clear the screen.
    // it also forces the compositor to refresh the background (tested with picom)
    Atom prop_root = XInternAtom(this->m_display, "_XROOTPMAP_ID", False);
    Atom prop_esetroot = XInternAtom(this->m_display, "ESETROOT_PMAP_ID", False);
    XChangeProperty(this->m_display, this->m_root, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &this->m_pixmap, 1);
    XChangeProperty(this->m_display, this->m_root, prop_esetroot, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &this->m_pixmap, 1);

    XClearWindow(this->m_display, this->m_root);
    XFlush(this->m_display);
}