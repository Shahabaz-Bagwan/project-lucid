#pragma once
#include <project-lucid/lib.hpp>
enum
{
  ID_ROTATE_LEFT = wxID_HIGHEST + 1,
  ID_ROTATE_RIGHT,
  ID_RESIZE,
  ID_ZOOM_x2,
  ID_ZOOM_DC,
  ID_ZOOM_NEAREST,
  ID_ZOOM_BILINEAR,
  ID_ZOOM_BICUBIC,
  ID_ZOOM_BOX_AVERAGE,
  ID_PAINT_BG
};

class MyImageFrame : public wxFrame
{
public:
  MyImageFrame( wxFrame* parent, const wxString& desc, const wxImage& image,
                double scale = 1.0 )
  {
    // Retrieve image info
    wxString info;
    int xres, yres;
    switch( GetResolutionFromOptions( image, &xres, &yres ) ) {
      case wxIMAGE_RESOLUTION_NONE:
        break;

      case wxIMAGE_RESOLUTION_CM:
        // convert to DPI
        xres = wxRound( xres / 10.0 * inches2mm );
        yres = wxRound( yres / 10.0 * inches2mm );
        wxFALLTHROUGH;

      case wxIMAGE_RESOLUTION_INCHES:
        info = wxString::Format( "DPI %i x %i", xres, yres );
        break;

      default:
        wxFAIL_MSG( "unexpected image resolution units" );
        break;
    }

    int numImages =
      desc.StartsWith( "Clipboard" ) ? 1 : image.GetImageCount( desc );
    if( numImages > 1 ) {
      if( !info.empty() )
        info += ", ";

      info += wxString::Format( "%d images", numImages );
    }

    Create( parent, desc, wxBitmap( image, wxBITMAP_SCREEN_DEPTH, scale ),
            info );
  }

  MyImageFrame( wxFrame* parent, const wxString& desc, const wxBitmap& bitmap )
  {
    Create( parent, desc, bitmap );
  }

private:
  bool Create( wxFrame* parent, const wxString& desc, const wxBitmap& bitmap,
               wxString info = wxString() )
  {
    if( !wxFrame::Create( parent, wxID_ANY,
                          wxString::Format( "Image from %s", desc ),
                          wxDefaultPosition, wxDefaultSize,
                          wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE ) )
      return false;

    m_bitmap          = bitmap;
    m_zoom            = 1.;
    m_useImageForZoom = false;

    wxMenu* menu = new wxMenu;
    menu->Append( wxID_SAVEAS );
    menu->AppendSeparator();
    menu->AppendCheckItem( ID_PAINT_BG, "&Paint background",
                           "Uncheck this for transparent images" );
    menu->AppendSeparator();
    menu->Append( ID_RESIZE, "&Fit to window\tCtrl-F" );
    menu->AppendSeparator();
    menu->Append( wxID_ZOOM_IN, "Zoom &in\tCtrl-+" );
    menu->Append( wxID_ZOOM_OUT, "Zoom &out\tCtrl--" );
    menu->Append( wxID_ZOOM_100, "Reset zoom to &100%\tCtrl-1" );
    menu->Append( ID_ZOOM_x2, "Double zoom level\tCtrl-2" );
    menu->AppendSeparator();
    menu->AppendRadioItem( ID_ZOOM_DC, "Use wx&DC for zoomin\tShift-Ctrl-D" );
    menu->AppendRadioItem( ID_ZOOM_NEAREST,
                           "Use rescale nearest\tShift-Ctrl-N" );
    menu->AppendRadioItem( ID_ZOOM_BILINEAR,
                           "Use rescale bilinear\tShift-Ctrl-L" );
    menu->AppendRadioItem( ID_ZOOM_BICUBIC,
                           "Use rescale bicubic\tShift-Ctrl-C" );
    menu->AppendRadioItem( ID_ZOOM_BOX_AVERAGE,
                           "Use rescale box average\tShift-Ctrl-B" );
    menu->AppendSeparator();
    menu->Append( ID_ROTATE_LEFT, "Rotate &left\tCtrl-L" );
    menu->Append( ID_ROTATE_RIGHT, "Rotate &right\tCtrl-R" );

    wxMenuBar* mbar = new wxMenuBar;
    mbar->Append( menu, "&Image" );
    SetMenuBar( mbar );

    mbar->Check( ID_PAINT_BG, true );

    CreateStatusBar( 2 );
    SetStatusText( info, 1 );

    SetClientSize( bitmap.GetWidth(), bitmap.GetHeight() );

    UpdateStatusBar();

    Show();

    return true;
  }

  void OnEraseBackground( wxEraseEvent& WXUNUSED( event ) )
  {
    // do nothing here to be able to see how transparent images are shown
  }

  void OnPaint( wxPaintEvent& WXUNUSED( event ) )
  {
    wxPaintDC dc( this );

    if( GetMenuBar()->IsChecked( ID_PAINT_BG ) )
      dc.Clear();

    const int width  = int( m_zoom * m_bitmap.GetWidth() );
    const int height = int( m_zoom * m_bitmap.GetHeight() );

    wxBitmap bitmap;
    if( m_useImageForZoom ) {
      bitmap =
        m_bitmap.ConvertToImage().Scale( width, height, m_resizeQuality );
    } else {
      dc.SetUserScale( m_zoom, m_zoom );
      bitmap = m_bitmap;
    }

    const wxSize size = GetClientSize();
    dc.DrawBitmap( bitmap, dc.DeviceToLogicalX( ( size.x - width ) / 2 ),
                   dc.DeviceToLogicalY( ( size.y - height ) / 2 ),
                   true /* use mask */
    );
  }

  void OnSave( wxCommandEvent& WXUNUSED( event ) )
  {
#if wxUSE_FILEDLG
    wxImage image = m_bitmap.ConvertToImage();

    wxString savefilename =
      wxFileSelector( "Save Image", wxEmptyString, wxEmptyString, wxEmptyString,
                      "BMP files (*.bmp)|*.bmp|"
#if wxUSE_LIBPNG
                      "PNG files (*.png)|*.png|"
#endif
#if wxUSE_LIBJPEG
                      "JPEG files (*.jpg)|*.jpg|",
#endif

                      wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this );

    if( savefilename.empty() )
      return;

    wxString extension;
    wxFileName::SplitPath( savefilename, NULL, NULL, &extension );

    bool saved = false;
    if( extension == "bmp" ) {
      static const int bppvalues[] = {
        wxBMP_1BPP,
        wxBMP_1BPP_BW,
        wxBMP_4BPP,
        wxBMP_8BPP,
        wxBMP_8BPP_GREY,
        wxBMP_8BPP_RED,
#if wxUSE_PALETTE
        wxBMP_8BPP_PALETTE,
#endif // wxUSE_PALETTE
        wxBMP_24BPP
      };

      const wxString bppchoices[] = {
        "1 bpp color",
        "1 bpp B&W",
        "4 bpp color",
        "8 bpp color",
        "8 bpp greyscale",
        "8 bpp red",
#if wxUSE_PALETTE
        "8 bpp own palette",
#endif // wxUSE_PALETTE
        "24 bpp"
      };

      int bppselection =
        wxGetSingleChoiceIndex( "Set BMP BPP", "Image sample: save file",
                                WXSIZEOF( bppchoices ), bppchoices, this );
      if( bppselection != -1 ) {
        int format = bppvalues[ bppselection ];
        image.SetOption( wxIMAGE_OPTION_BMP_FORMAT, format );
#if wxUSE_PALETTE
        if( format == wxBMP_8BPP_PALETTE ) {
          unsigned char* cmap = new unsigned char[ 256 ];
          for( int i = 0; i < 256; i++ )
            cmap[ i ] = (unsigned char)i;
          image.SetPalette( wxPalette( 256, cmap, cmap, cmap ) );

          delete[] cmap;
        }
#endif // wxUSE_PALETTE
      }
    }
#if wxUSE_LIBPNG
    else if( extension == "png" ) {
      static const int pngvalues[] = {
        wxPNG_TYPE_COLOUR, wxPNG_TYPE_COLOUR,   wxPNG_TYPE_GREY,
        wxPNG_TYPE_GREY,   wxPNG_TYPE_GREY_RED, wxPNG_TYPE_GREY_RED,
      };

      const wxString pngchoices[] = {
        "Colour 8bpp", "Colour 16bpp",  "Grey 8bpp",
        "Grey 16bpp",  "Grey red 8bpp", "Grey red 16bpp",
      };

      int sel =
        wxGetSingleChoiceIndex( "Set PNG format", "Image sample: save file",
                                WXSIZEOF( pngchoices ), pngchoices, this );
      if( sel != -1 ) {
        image.SetOption( wxIMAGE_OPTION_PNG_FORMAT, pngvalues[ sel ] );
        image.SetOption( wxIMAGE_OPTION_PNG_BITDEPTH, sel % 2 ? 16 : 8 );

        // these values are taken from OptiPNG with -o3 switch
        const wxString compressionChoices[] = {
          "compression = 9, memory = 8, strategy = 0, filter = 0",
          "compression = 9, memory = 9, strategy = 0, filter = 0",
          "compression = 9, memory = 8, strategy = 1, filter = 0",
          "compression = 9, memory = 9, strategy = 1, filter = 0",
          "compression = 1, memory = 8, strategy = 2, filter = 0",
          "compression = 1, memory = 9, strategy = 2, filter = 0",
          "compression = 9, memory = 8, strategy = 0, filter = 5",
          "compression = 9, memory = 9, strategy = 0, filter = 5",
          "compression = 9, memory = 8, strategy = 1, filter = 5",
          "compression = 9, memory = 9, strategy = 1, filter = 5",
          "compression = 1, memory = 8, strategy = 2, filter = 5",
          "compression = 1, memory = 9, strategy = 2, filter = 5",
        };

        sel = wxGetSingleChoiceIndex(
          "Select compression option (Cancel to use default)\n",
          "PNG Compression Options", WXSIZEOF( compressionChoices ),
          compressionChoices, this );
        if( sel != -1 ) {
          const int zc[] = { 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 1, 1 };
          const int zm[] = { 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9 };
          const int zs[] = { 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2 };
          const int f[]  = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                            0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8 };

          image.SetOption( wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, zc[ sel ] );
          image.SetOption( wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL,
                           zm[ sel ] );
          image.SetOption( wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY, zs[ sel ] );
          image.SetOption( wxIMAGE_OPTION_PNG_FILTER, f[ sel ] );
          image.SetOption( wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE,
                           1048576 ); // 1 MB
        }
      }
    }
#endif // wxUSE_LIBPNG
    else if( extension == "cur" ) {
      image.Rescale( 32, 32 );
      image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
      image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
      // This shows how you can save an image with explicitly
      // specified image format:
      saved = image.SaveFile( savefilename, wxBITMAP_TYPE_CUR );
    }

    if( !saved ) {
      // This one guesses image format from filename extension
      // (it may fail if the extension is not recognized):
      image.SaveFile( savefilename );
    }
#endif // wxUSE_FILEDLG
  }

  void OnResize( wxCommandEvent& WXUNUSED( event ) )
  {
    wxImage img( m_bitmap.ConvertToImage() );

    const wxSize size = GetClientSize();
    img.Rescale( size.x, size.y, wxIMAGE_QUALITY_HIGH );
    m_bitmap = wxBitmap( img );

    UpdateStatusBar();
  }

  void OnZoom( wxCommandEvent& event )
  {
    switch( event.GetId() ) {
      case wxID_ZOOM_IN:
        m_zoom *= 1.2;
        break;

      case wxID_ZOOM_OUT:
        m_zoom /= 1.2;
        break;

      case wxID_ZOOM_100:
        m_zoom = 1.;
        break;

      case ID_ZOOM_x2:
        m_zoom *= 2.;
        break;

      default:
        wxFAIL_MSG( "unknown zoom command" );
        return;
    }

    UpdateStatusBar();
  }

  void OnUseZoom( wxCommandEvent& event )
  {
    bool useImageForZoom = true;

    switch( event.GetId() ) {
      case ID_ZOOM_DC:
        useImageForZoom = false;
        break;

      case ID_ZOOM_NEAREST:
        m_resizeQuality = wxIMAGE_QUALITY_NEAREST;
        break;

      case ID_ZOOM_BILINEAR:
        m_resizeQuality = wxIMAGE_QUALITY_BILINEAR;
        break;

      case ID_ZOOM_BICUBIC:
        m_resizeQuality = wxIMAGE_QUALITY_BICUBIC;
        break;

      case ID_ZOOM_BOX_AVERAGE:
        m_resizeQuality = wxIMAGE_QUALITY_BOX_AVERAGE;
        break;

      default:
        wxFAIL_MSG( "unknown use for zoom command" );
        return;
    }

    m_useImageForZoom = useImageForZoom;

    Refresh();
  }

  void OnRotate( wxCommandEvent& event )
  {
    double angle = 5;
    if( event.GetId() == ID_ROTATE_LEFT )
      angle = -angle;

    wxImage img( m_bitmap.ConvertToImage() );
    img =
      img.Rotate( angle, wxPoint( img.GetWidth() / 2, img.GetHeight() / 2 ) );
    if( !img.IsOk() ) {
      wxLogWarning( "Rotation failed" );
      return;
    }

    m_bitmap = wxBitmap( img );

    UpdateStatusBar();
  }

  void UpdateStatusBar()
  {
    wxLogStatus( this, "Image size: (%d, %d), zoom %.2f", m_bitmap.GetWidth(),
                 m_bitmap.GetHeight(), m_zoom );
    Refresh();
  }

  // This is a copy of protected wxImageHandler::GetResolutionFromOptions()
  static wxImageResolution GetResolutionFromOptions( const wxImage& image,
                                                     int* x, int* y )
  {
    wxCHECK_MSG( x && y, wxIMAGE_RESOLUTION_NONE, wxT( "NULL pointer" ) );

    if( image.HasOption( wxIMAGE_OPTION_RESOLUTIONX ) &&
        image.HasOption( wxIMAGE_OPTION_RESOLUTIONY ) ) {
      *x = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONX );
      *y = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONY );
    } else if( image.HasOption( wxIMAGE_OPTION_RESOLUTION ) ) {
      *x = *y = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTION );
    } else // no resolution options specified
    {
      *x = *y = 0;

      return wxIMAGE_RESOLUTION_NONE;
    }

    // get the resolution unit too
    int resUnit = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONUNIT );
    if( !resUnit ) {
      // this is the default
      resUnit = wxIMAGE_RESOLUTION_INCHES;
    }

    return (wxImageResolution)resUnit;
  }

  wxBitmap m_bitmap;
  double m_zoom;

  // If false, then wxDC is used for zooming. If true, then m_resizeQuality
  // is used with wxImage::Scale() for zooming.
  bool m_useImageForZoom;
  wxImageResizeQuality m_resizeQuality;

  wxDECLARE_EVENT_TABLE();
};
wxBEGIN_EVENT_TABLE( MyImageFrame, wxFrame ) EVT_ERASE_BACKGROUND(
  MyImageFrame::OnEraseBackground ) EVT_PAINT( MyImageFrame::OnPaint )

  EVT_MENU( wxID_SAVEAS, MyImageFrame::OnSave )
    EVT_MENU_RANGE( ID_ROTATE_LEFT, ID_ROTATE_RIGHT, MyImageFrame::OnRotate )
      EVT_MENU( ID_RESIZE, MyImageFrame::OnResize )

        EVT_MENU( wxID_ZOOM_IN, MyImageFrame::OnZoom )
          EVT_MENU( wxID_ZOOM_OUT, MyImageFrame::OnZoom )
            EVT_MENU( wxID_ZOOM_100, MyImageFrame::OnZoom )
              EVT_MENU( ID_ZOOM_x2, MyImageFrame::OnZoom )

                EVT_MENU( ID_ZOOM_DC, MyImageFrame::OnUseZoom )
                  EVT_MENU( ID_ZOOM_NEAREST, MyImageFrame::OnUseZoom )
                    EVT_MENU( ID_ZOOM_BILINEAR, MyImageFrame::OnUseZoom )
                      EVT_MENU( ID_ZOOM_BICUBIC, MyImageFrame::OnUseZoom )
                        EVT_MENU( ID_ZOOM_BOX_AVERAGE, MyImageFrame::OnUseZoom )
                          wxEND_EVENT_TABLE()

                            class MyFiltersFrame : public wxFrame
{
public:
  MyFiltersFrame( wxWindow* parent )
    : wxFrame( parent, wxID_ANY, "Image filters test" )
  {
    wxMenu* menuImage = new wxMenu;

    menuImage->Append( wxID_OPEN, "&Open...\tCtrl-O",
                       "Load a user defined image" );
    menuImage->Append( wxID_RESET, "&Reset\tCtrl-R",
                       "Reset all the image filters" );
    menuImage->Append( wxID_CLOSE, "&Close\tCtrl-Q", "Close this frame" );

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append( menuImage, "&Image" );
    SetMenuBar( menuBar );

    wxSizerFlags sizerFlags1( 1 );
    sizerFlags1.Border().Expand();

    wxSizerFlags sizerFlags2;
    sizerFlags2.Border().Expand();

    wxSizerFlags sizerFlags3;
    sizerFlags3.Border().Center();

    wxSizerFlags sizerFlags4;
    sizerFlags4.Border();

    wxStaticBoxSizer* sizerHue = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Hue (°)" ) ), wxVERTICAL );
    m_sliderHue = new wxSlider( sizerHue->GetStaticBox(), wxID_ANY, 0, -360,
                                360, wxDefaultPosition, wxDefaultSize,
                                wxSL_HORIZONTAL | wxSL_LABELS );
    sizerHue->Add( m_sliderHue, sizerFlags2 );

    wxStaticBoxSizer* sizerSaturation = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Saturation (%)" ) ), wxVERTICAL );
    m_sliderSaturation = new wxSlider(
      sizerSaturation->GetStaticBox(), wxID_ANY, 0, -100, 100,
      wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS );
    sizerSaturation->Add( m_sliderSaturation, sizerFlags2 );

    wxStaticBoxSizer* sizerBrightness = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Brightness (value) (%)" ) ),
      wxVERTICAL );
    m_sliderBrightness = new wxSlider(
      sizerBrightness->GetStaticBox(), wxID_ANY, 0, -100, 100,
      wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS );
    sizerBrightness->Add( m_sliderBrightness, sizerFlags2 );

    wxStaticBoxSizer* sizerLightness = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Lightness" ) ), wxVERTICAL );
    m_sliderLightness = new wxSlider(
      sizerLightness->GetStaticBox(), wxID_ANY, 100, 0, 200, wxDefaultPosition,
      wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS );
    sizerLightness->Add( m_sliderLightness, sizerFlags2 );

    wxStaticBoxSizer* sizerDisabled = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Disabled" ) ), wxVERTICAL );
    m_checkDisabled  = new wxCheckBox( sizerDisabled->GetStaticBox(), wxID_ANY,
                                       wxS( "Convert to disabled" ) );
    m_sliderDisabled = new wxSlider(
      sizerDisabled->GetStaticBox(), wxID_ANY, 255, 0, 255, wxDefaultPosition,
      wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS );
    sizerDisabled->Add( m_checkDisabled, sizerFlags4 );
    sizerDisabled->Add( m_sliderDisabled, sizerFlags2 );

    wxStaticBoxSizer* sizerGrey = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Greyscale" ) ), wxVERTICAL );
    m_checkGrey        = new wxCheckBox( sizerGrey->GetStaticBox(), wxID_ANY,
                                         wxS( "Convert to greyscale" ) );
    wxBoxSizer* sizer1 = new wxBoxSizer( wxHORIZONTAL );
    m_spinGreyWeightR  = new wxSpinCtrlDouble(
       sizerGrey->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition,
       wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.299, 0.001 );
    m_spinGreyWeightG = new wxSpinCtrlDouble(
      sizerGrey->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition,
      wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.587, 0.001 );
    m_spinGreyWeightB = new wxSpinCtrlDouble(
      sizerGrey->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition,
      wxDefaultSize, wxSP_ARROW_KEYS, 0, 1, 0.114, 0.001 );
    sizer1->AddStretchSpacer();
    sizer1->Add( new wxStaticText( sizerGrey->GetStaticBox(), wxID_ANY,
                                   wxS( "Red weight:" ) ),
                 sizerFlags3 );
    sizer1->Add( m_spinGreyWeightR, sizerFlags3 );
    sizer1->Add( new wxStaticText( sizerGrey->GetStaticBox(), wxID_ANY,
                                   wxS( "Green weight:" ) ),
                 sizerFlags3 );
    sizer1->Add( m_spinGreyWeightG, sizerFlags3 );
    sizer1->Add( new wxStaticText( sizerGrey->GetStaticBox(), wxID_ANY,
                                   wxS( "Blue weight:" ) ),
                 sizerFlags3 );
    sizer1->Add( m_spinGreyWeightB, sizerFlags3 );
    sizer1->AddStretchSpacer();
    sizerGrey->Add( m_checkGrey, sizerFlags4 );
    sizerGrey->Add( sizer1, sizerFlags2 );

    wxStaticBoxSizer* sizerMono = new wxStaticBoxSizer(
      new wxStaticBox( this, wxID_ANY, wxS( "Monochrome" ) ), wxVERTICAL );
    m_checkMono        = new wxCheckBox( sizerMono->GetStaticBox(), wxID_ANY,
                                         wxS( "Convert to monochrome" ) );
    wxBoxSizer* sizer2 = new wxBoxSizer( wxHORIZONTAL );
    m_spinMonoR        = new wxSpinCtrl( sizerMono->GetStaticBox(), wxID_ANY,
                                         wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
    m_spinMonoG        = new wxSpinCtrl( sizerMono->GetStaticBox(), wxID_ANY,
                                         wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
    m_spinMonoB        = new wxSpinCtrl( sizerMono->GetStaticBox(), wxID_ANY,
                                         wxEmptyString, wxDefaultPosition,
                                         wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
    sizer2->AddStretchSpacer();
    sizer2->Add(
      new wxStaticText( sizerMono->GetStaticBox(), wxID_ANY, wxS( "Red:" ) ),
      sizerFlags3 );
    sizer2->Add( m_spinMonoR, sizerFlags3 );
    sizer2->Add(
      new wxStaticText( sizerMono->GetStaticBox(), wxID_ANY, wxS( "Green:" ) ),
      sizerFlags3 );
    sizer2->Add( m_spinMonoG, sizerFlags3 );
    sizer2->Add(
      new wxStaticText( sizerMono->GetStaticBox(), wxID_ANY, wxS( "Blue:" ) ),
      sizerFlags3 );
    sizer2->Add( m_spinMonoB, sizerFlags3 );
    sizer2->AddStretchSpacer();
    sizerMono->Add( m_checkMono, sizerFlags4 );
    sizerMono->Add( sizer2, sizerFlags2 );

    wxBoxSizer* sizerLeft = new wxBoxSizer( wxVERTICAL );
    sizerLeft->Add( sizerHue, sizerFlags2 );
    sizerLeft->Add( sizerSaturation, sizerFlags2 );
    sizerLeft->Add( sizerBrightness, sizerFlags2 );
    sizerLeft->Add( sizerLightness, sizerFlags2 );

    wxBoxSizer* sizerRight = new wxBoxSizer( wxVERTICAL );
    sizerRight->Add( sizerDisabled, sizerFlags2 );
    sizerRight->Add( sizerGrey, sizerFlags2 );
    sizerRight->Add( sizerMono, sizerFlags2 );

    wxBitmap bitmap = wxArtProvider::GetBitmap( wxART_INFORMATION, wxART_BUTTON,
                                                wxSize( 256, 256 ) );
    m_image         = bitmap.ConvertToImage();

    m_stcBitmap          = new wxStaticBitmap( this, wxID_ANY, bitmap );
    wxBoxSizer* sizerTop = new wxBoxSizer( wxHORIZONTAL );
    sizerTop->AddStretchSpacer();
    sizerTop->Add( m_stcBitmap, sizerFlags1 );
    sizerTop->AddStretchSpacer();

    sizerFlags1.Border( 0 );

    wxBoxSizer* sizerBottom = new wxBoxSizer( wxHORIZONTAL );
    sizerBottom->Add( sizerLeft, sizerFlags1 );
    sizerBottom->Add( sizerRight, sizerFlags1 );

    wxBoxSizer* sizerMain = new wxBoxSizer( wxVERTICAL );
    sizerMain->Add( sizerTop, sizerFlags1 );
    sizerMain->Add( sizerBottom, sizerFlags2 );

    SetSizer( sizerMain );
    CreateStatusBar();

    // Bind Events
    Bind( wxEVT_MENU, &MyFiltersFrame::OnNewImage, this, wxID_OPEN );
    Bind( wxEVT_MENU, &MyFiltersFrame::OnReset, this, wxID_RESET );
    Bind( wxEVT_MENU, &MyFiltersFrame::OnClose, this, wxID_CLOSE );
    m_sliderHue->Bind( wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this );
    m_sliderSaturation->Bind( wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this );
    m_sliderBrightness->Bind( wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this );
    m_sliderLightness->Bind( wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this );
    m_checkDisabled->Bind( wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this );
    m_sliderDisabled->Bind( wxEVT_SLIDER, &MyFiltersFrame::OnFilter, this );
    m_checkGrey->Bind( wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this );
    m_spinGreyWeightR->Bind( wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter,
                             this );
    m_spinGreyWeightG->Bind( wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter,
                             this );
    m_spinGreyWeightB->Bind( wxEVT_SPINCTRLDOUBLE, &MyFiltersFrame::OnFilter,
                             this );
    m_checkMono->Bind( wxEVT_CHECKBOX, &MyFiltersFrame::OnFilter, this );
    m_spinMonoR->Bind( wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this );
    m_spinMonoG->Bind( wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this );
    m_spinMonoB->Bind( wxEVT_SPINCTRL, &MyFiltersFrame::OnFilter, this );
  }

private:
  wxStaticBitmap* m_stcBitmap;
  wxSlider* m_sliderHue;
  wxSlider* m_sliderSaturation;
  wxSlider* m_sliderBrightness;
  wxSlider* m_sliderLightness;
  wxCheckBox* m_checkDisabled;
  wxSlider* m_sliderDisabled;
  wxCheckBox* m_checkGrey;
  wxSpinCtrlDouble* m_spinGreyWeightR;
  wxSpinCtrlDouble* m_spinGreyWeightG;
  wxSpinCtrlDouble* m_spinGreyWeightB;
  wxCheckBox* m_checkMono;
  wxSpinCtrl* m_spinMonoR;
  wxSpinCtrl* m_spinMonoG;
  wxSpinCtrl* m_spinMonoB;
  wxImage m_image;

protected:
  void OnNewImage( wxCommandEvent& WXUNUSED( event ) )
  {
    wxImage image;
    wxString filename =
      static_cast< MyFrame* >( GetParent() )->LoadUserImage( image );

    if( !filename.empty() ) {
      m_image = image;
      DoFilter();
    }
  }

  void OnReset( wxCommandEvent& WXUNUSED( event ) )
  {
    m_stcBitmap->SetBitmap( m_image );
    m_sliderHue->SetValue( 0 );
    m_sliderSaturation->SetValue( 0 );
    m_sliderBrightness->SetValue( 0 );
    m_sliderLightness->SetValue( 100 );
    m_checkDisabled->SetValue( false );
    m_sliderDisabled->SetValue( 255 );
    m_checkGrey->SetValue( false );
    m_spinGreyWeightR->SetValue( 0.299 );
    m_spinGreyWeightG->SetValue( 0.587 );
    m_spinGreyWeightB->SetValue( 0.114 );
    m_checkMono->SetValue( false );
    m_spinMonoR->SetValue( 0 );
    m_spinMonoG->SetValue( 0 );
    m_spinMonoB->SetValue( 0 );
  }

  void OnClose( wxCommandEvent& WXUNUSED( event ) ) { Close( true ); }

  virtual void OnFilter( wxEvent& WXUNUSED( event ) ) { DoFilter(); }

  void DoFilter()
  {
    wxImage image = m_image;
    image.RotateHue( m_sliderHue->GetValue() / 360. );
    image.ChangeSaturation( m_sliderSaturation->GetValue() / 100. );
    image.ChangeBrightness( m_sliderBrightness->GetValue() / 100. );
    image = image.ChangeLightness( m_sliderLightness->GetValue() );

    if( m_checkDisabled->IsChecked() )
      image = image.ConvertToDisabled( m_sliderDisabled->GetValue() );

    if( m_checkGrey->IsChecked() ) {
      image = image.ConvertToGreyscale( m_spinGreyWeightR->GetValue(),
                                        m_spinGreyWeightG->GetValue(),
                                        m_spinGreyWeightB->GetValue() );
    }

    if( m_checkMono->IsChecked() ) {
      image =
        image.ConvertToMono( m_spinMonoR->GetValue(), m_spinMonoG->GetValue(),
                             m_spinMonoB->GetValue() );
    }

    m_stcBitmap->SetBitmap( image );
    Layout();
  }
};