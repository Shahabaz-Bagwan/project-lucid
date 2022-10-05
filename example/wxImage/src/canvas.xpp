///////////////////////////////////////////////////////////////////////////////
// Name:        samples/image/canvas.cpp
// Purpose:     sample showing operations with wxImage
// Author:      Robert Roebling
// Modified by: Francesco Montorsi
// Created:     1998
// Copyright:   (c) 1998-2005 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".

#include "cursor_png.c"

#include "canvas.h"

//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( MyCanvas, wxScrolledWindow ) EVT_PAINT( MyCanvas::OnPaint )
  wxEND_EVENT_TABLE()

    MyCanvas::MyCanvas( wxWindow* parent, wxWindowID id, const wxPoint& pos,
                        const wxSize& size )
  : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER )
{
  my_horse_ani = NULL;
  m_ani_images = 0;

  SetBackgroundColour( *wxWHITE );

  wxBitmap bitmap( 100, 100 );

  wxMemoryDC dc;
  dc.SelectObject( bitmap );
  dc.SetBrush( wxBrush( "orange" ) );
  dc.SetPen( *wxBLACK_PEN );
  dc.DrawRectangle( 0, 0, 100, 100 );
  dc.SetBrush( *wxWHITE_BRUSH );
  dc.DrawRectangle( 20, 20, 60, 60 );
  dc.SelectObject( wxNullBitmap );

  // try to find the directory with our images
  wxString dir;
  wxImage image = bitmap.ConvertToImage();
#if wxUSE_LIBPNG
  if( !image.SaveFile( dir + "test.png", wxBITMAP_TYPE_PNG ) ) {
    wxLogError( "Can't save file" );
  }

  image.Destroy();

  if( image.LoadFile( dir + "test.png" ) )
    my_square = wxBitmap( image );

  image.Destroy();

  if( !image.LoadFile( dir + "horse.png" ) ) {
    wxLogError( "Can't load PNG image" );
  } else {
    my_horse_png = wxBitmap( image );
  }

  if( !image.LoadFile( dir + "toucan.png" ) ) {
    wxLogError( "Can't load PNG image" );
  } else {
    my_toucan = wxBitmap( image );
  }

  my_toucan_flipped_horiz = wxBitmap( image.Mirror( true ) );
  my_toucan_flipped_vert  = wxBitmap( image.Mirror( false ) );
  my_toucan_flipped_both  = wxBitmap( image.Mirror( true ).Mirror( false ) );
  my_toucan_grey          = wxBitmap( image.ConvertToGreyscale() );
  my_toucan_head = wxBitmap( image.GetSubImage( wxRect( 40, 7, 80, 60 ) ) );
  my_toucan_scaled_normal =
    wxBitmap( image.Scale( 110, 90, wxIMAGE_QUALITY_NORMAL ) );
  my_toucan_scaled_high =
    wxBitmap( image.Scale( 110, 90, wxIMAGE_QUALITY_HIGH ) );
  my_toucan_blur = wxBitmap( image.Blur( 10 ) );

#endif // wxUSE_LIBPNG

#if wxUSE_LIBJPEG
  image.Destroy();

  if( !image.LoadFile( dir + "horse.jpg" ) ) {
    wxLogError( "Can't load JPG image" );
  } else {
    my_horse_jpeg = wxBitmap( image );

    // Colorize by rotating green hue to red
    wxImage::HSVValue greenHSV =
      wxImage::RGBtoHSV( wxImage::RGBValue( 0, 255, 0 ) );
    wxImage::HSVValue redHSV =
      wxImage::RGBtoHSV( wxImage::RGBValue( 255, 0, 0 ) );
    image.RotateHue( redHSV.hue - greenHSV.hue );
    colorized_horse_jpeg = wxBitmap( image );
  }

  if( !image.LoadFile( dir + "cmyk.jpg" ) ) {
    wxLogError( "Can't load CMYK JPG image" );
  } else {
    my_cmyk_jpeg = wxBitmap( image );
  }
#endif // wxUSE_LIBJPEG

  image.Destroy();

  // test image loading from stream
  // test image loading from stream
  wxFile file( dir + "horse.bmp" );
  if( file.IsOpened() ) {
    wxFileOffset len = file.Length();
    size_t dataSize  = (size_t)len;
    void* data       = malloc( dataSize );
    if( file.Read( data, dataSize ) != len ) {
      wxLogError( "Reading bitmap file failed" );
    } else {
      wxMemoryInputStream mis( data, dataSize );
      if( !image.LoadFile( mis ) ) {
        wxLogError( "Can't load BMP image from stream" );
      } else {
        my_horse_bmp2 = wxBitmap( image );
      }
    }

    free( data );
  }
  // This macro loads PNG from either resources on the platforms that support
  // this (Windows and OS X) or from in-memory data (coming from cursor_png.c
  // included above in our case).
  my_png_from_res = wxBITMAP_PNG( cursor );

  // This one always loads PNG from memory but exists for consistency with
  // the above one and also because it frees you from the need to specify the
  // length explicitly, without it you'd have to do it and also spell the
  // array name in full, like this:
  //
  // my_png_from_mem = wxBitmap::NewFromPNGData(cursor_png,
  // WXSIZEOF(cursor_png));
  my_png_from_mem = wxBITMAP_PNG_FROM_DATA( cursor );

  // prevent -Wunused-const-variable when compiler fails to detect its usage
  wxUnusedVar( cursor_png );
}

MyCanvas::~MyCanvas() { delete[] my_horse_ani; }

void MyCanvas::OnPaint( wxPaintEvent& WXUNUSED( event ) )
{
  wxPaintDC dc( this );
  PrepareDC( dc );

  dc.DrawText( "Loaded image", 30, 10 );
  if( my_square.IsOk() )
    dc.DrawBitmap( my_square, 30, 30 );

  dc.DrawText( "Drawn directly", 150, 10 );
  dc.SetBrush( wxBrush( "orange" ) );
  dc.SetPen( *wxBLACK_PEN );
  dc.DrawRectangle( 150, 30, 100, 100 );
  dc.SetBrush( *wxWHITE_BRUSH );
  dc.DrawRectangle( 170, 50, 60, 60 );

  if( my_anti.IsOk() )
    dc.DrawBitmap( my_anti, 280, 30 );

  dc.DrawText( "PNG handler", 30, 135 );
  if( my_horse_png.IsOk() ) {
    dc.DrawBitmap( my_horse_png, 30, 150 );
    wxRect rect( 0, 0, 100, 100 );
    wxBitmap sub( my_horse_png.GetSubBitmap( rect ) );
    dc.DrawText( "GetSubBitmap()", 280, 175 );
    dc.DrawBitmap( sub, 280, 195 );
  }

  dc.DrawText( "JPEG handler", 30, 365 );
  if( my_horse_jpeg.IsOk() )
    dc.DrawBitmap( my_horse_jpeg, 30, 380 );

  dc.DrawText( "Green rotated to red", 280, 365 );
  if( colorized_horse_jpeg.IsOk() )
    dc.DrawBitmap( colorized_horse_jpeg, 280, 380 );

  dc.DrawText( "CMYK JPEG image", 530, 365 );
  if( my_cmyk_jpeg.IsOk() )
    dc.DrawBitmap( my_cmyk_jpeg, 530, 380 );

  dc.DrawText( "BMP handler", 30, 1055 );
  if( my_horse_bmp.IsOk() )
    dc.DrawBitmap( my_horse_bmp, 30, 1070 );

  dc.DrawText( "BMP read from memory", 280, 1055 );
  if( my_horse_bmp2.IsOk() )
    dc.DrawBitmap( my_horse_bmp2, 280, 1070 );

  // toucans
  {
    int x = 750, y = 10, yy = 170;

    dc.DrawText( "Original toucan", x + 50, y );
    dc.DrawBitmap( my_toucan, x, y + 15, true );
    y += yy;
    dc.DrawText( "Flipped horizontally", x + 50, y );
    dc.DrawBitmap( my_toucan_flipped_horiz, x, y + 15, true );
    y += yy;
    dc.DrawText( "Flipped vertically", x + 50, y );
    dc.DrawBitmap( my_toucan_flipped_vert, x, y + 15, true );
    y += yy;
    dc.DrawText( "Flipped both h&v", x + 50, y );
    dc.DrawBitmap( my_toucan_flipped_both, x, y + 15, true );

    y += yy;
    dc.DrawText( "In greyscale", x + 50, y );
    dc.DrawBitmap( my_toucan_grey, x, y + 15, true );

    y += yy;
    dc.DrawText( "Toucan's head", x + 50, y );
    dc.DrawBitmap( my_toucan_head, x, y + 15, true );

    y += yy;
    dc.DrawText( "Scaled with normal quality", x + 50, y );
    dc.DrawBitmap( my_toucan_scaled_normal, x, y + 15, true );

    y += yy;
    dc.DrawText( "Scaled with high quality", x + 50, y );
    dc.DrawBitmap( my_toucan_scaled_high, x, y + 15, true );

    y += yy;
    dc.DrawText( "Blured", x + 50, y );
    dc.DrawBitmap( my_toucan_blur, x, y + 15, true );
  }

  if( my_smile_xbm.IsOk() ) {
    int x = 300, y = 1800;

    dc.DrawText( "XBM bitmap", x, y );
    dc.DrawText( "(green on red)", x, y + 15 );
    dc.SetTextForeground( "GREEN" );
    dc.SetTextBackground( "RED" );
    dc.DrawBitmap( my_smile_xbm, x, y + 30 );

    dc.SetTextForeground( *wxBLACK );
    dc.DrawText( "After wxImage conversion", x + 120, y );
    dc.DrawText( "(red on white)", x + 120, y + 15 );
    dc.SetTextForeground( "RED" );
    wxImage i = my_smile_xbm.ConvertToImage();
    i.SetMaskColour( 255, 255, 255 );
    i.Replace( 0, 0, 0, wxRED_PEN->GetColour().Red(),
               wxRED_PEN->GetColour().Green(), wxRED_PEN->GetColour().Blue() );
    dc.DrawBitmap( wxBitmap( i ), x + 120, y + 30, true );
    dc.SetTextForeground( *wxBLACK );
  }

  wxBitmap mono( 60, 50, 1 );
  wxMemoryDC memdc;
  memdc.SelectObject( mono );
  memdc.SetPen( *wxBLACK_PEN );
  memdc.SetBrush( *wxWHITE_BRUSH );
  memdc.DrawRectangle( 0, 0, 60, 50 );
  memdc.SetTextForeground( *wxBLACK );
#ifndef __WXGTK20__
  // I cannot convince GTK2 to draw into mono bitmaps
  memdc.DrawText( "Hi!", 5, 5 );
#endif
  memdc.SetBrush( *wxBLACK_BRUSH );
  memdc.DrawRectangle( 33, 5, 20, 20 );
  memdc.SetPen( *wxRED_PEN );
  memdc.DrawLine( 5, 42, 50, 42 );
  memdc.SelectObject( wxNullBitmap );

  if( mono.IsOk() ) {
    int x = 300, y = 1900;

    dc.DrawText( "Mono bitmap", x, y );
    dc.DrawText( "(red on green)", x, y + 15 );
    dc.SetTextForeground( "RED" );
    dc.SetTextBackground( "GREEN" );
    dc.DrawBitmap( mono, x, y + 30 );

    dc.SetTextForeground( *wxBLACK );
    dc.DrawText( "After wxImage conversion", x + 120, y );
    dc.DrawText( "(red on white)", x + 120, y + 15 );
    dc.SetTextForeground( "RED" );
    wxImage i = mono.ConvertToImage();
    i.SetMaskColour( 255, 255, 255 );
    i.Replace( 0, 0, 0, wxRED_PEN->GetColour().Red(),
               wxRED_PEN->GetColour().Green(), wxRED_PEN->GetColour().Blue() );
    dc.DrawBitmap( wxBitmap( i ), x + 120, y + 30, true );
    dc.SetTextForeground( *wxBLACK );
  }

  // For testing transparency
  dc.SetBrush( *wxRED_BRUSH );
  dc.DrawRectangle( 20, 2220, 560, 68 );

  dc.DrawText( "PNG from resources", 30, 2460 );
  if( my_png_from_res.IsOk() )
    dc.DrawBitmap( my_png_from_res, 30, 2480, true );
  dc.DrawText( "PNG from memory", 230, 2460 );
  if( my_png_from_mem.IsOk() )
    dc.DrawBitmap( my_png_from_mem, 230, 2480, true );
}

void MyCanvas::CreateAntiAliasedBitmap()
{
  wxBitmap bitmap( 300, 300 );

  {
    wxMemoryDC dc( bitmap );

    dc.Clear();

    dc.SetFont( wxFontInfo( 24 ).Family( wxFONTFAMILY_DECORATIVE ) );
    dc.SetTextForeground( "RED" );
    dc.DrawText( "This is anti-aliased Text.", 20, 5 );
    dc.DrawText( "And a Rectangle.", 20, 45 );

    dc.SetBrush( *wxRED_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRoundedRectangle( 20, 85, 200, 180, 20 );
  }

  wxImage original = bitmap.ConvertToImage();
  wxImage anti( 150, 150 );

  /* This is quite slow, but safe. Use wxImage::GetData() for speed instead. */

  for( int y = 1; y < 149; y++ )
    for( int x = 1; x < 149; x++ ) {
      int red = original.GetRed( x * 2, y * 2 ) +
                original.GetRed( x * 2 - 1, y * 2 ) +
                original.GetRed( x * 2, y * 2 + 1 ) +
                original.GetRed( x * 2 + 1, y * 2 + 1 );
      red = red / 4;

      int green = original.GetGreen( x * 2, y * 2 ) +
                  original.GetGreen( x * 2 - 1, y * 2 ) +
                  original.GetGreen( x * 2, y * 2 + 1 ) +
                  original.GetGreen( x * 2 + 1, y * 2 + 1 );
      green = green / 4;

      int blue = original.GetBlue( x * 2, y * 2 ) +
                 original.GetBlue( x * 2 - 1, y * 2 ) +
                 original.GetBlue( x * 2, y * 2 + 1 ) +
                 original.GetBlue( x * 2 + 1, y * 2 + 1 );
      blue = blue / 4;
      anti.SetRGB( x, y, (unsigned char)red, (unsigned char)green,
                   (unsigned char)blue );
    }

  my_anti = wxBitmap( anti );
}
