
#include "Frame.hpp"
#include "ImageFrame.hpp"

wxIMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame );
wxBEGIN_EVENT_TABLE( MyFrame, wxFrame ) EVT_MENU( ID_ABOUT, MyFrame::OnAbout )
  EVT_MENU( ID_QUIT, MyFrame::OnQuit ) EVT_MENU( ID_NEW, MyFrame::OnNewFrame )
    EVT_MENU( ID_NEW_HIDPI, MyFrame::OnNewFrameHiDPI )
      EVT_MENU( ID_INFO, MyFrame::OnImageInfo )
        EVT_MENU( ID_SHOWTHUMBNAIL, MyFrame::OnThumbnail )
          EVT_MENU( ID_FILTERS, MyFrame::OnFilters )
            EVT_UPDATE_UI( ID_NEW_HIDPI, MyFrame::OnUpdateNewFrameHiDPI )
              wxEND_EVENT_TABLE()

                MyFrame::MyFrame()
  : wxFrame( (wxFrame*)NULL, wxID_ANY, "wxImage sample", wxPoint( 20, 20 ),
             wxSize( 950, 700 ) )
{

  wxMenuBar* menu_bar = new wxMenuBar();

  wxMenu* menuImage = new wxMenu;
  menuImage->Append( ID_NEW, "&Show any image...\tCtrl-O" );
  menuImage->Append( ID_NEW_HIDPI, "Show any image as &HiDPI...\tCtrl-H" );
  menuImage->Append( ID_INFO, "Show image &information...\tCtrl-I" );
  menuImage->AppendSeparator();
  menuImage->Append( ID_SHOWTHUMBNAIL, "Test &thumbnail...\tCtrl-T",
                     "Test scaling the image during load (try with JPEG)" );
  menuImage->AppendSeparator();
  menuImage->Append( ID_FILTERS, "Test image &filters...\tCtrl-F",
                     "Test applying different image filters" );
  menuImage->AppendSeparator();
  menuImage->Append( ID_ABOUT, "&About\tF1" );
  menuImage->AppendSeparator();
  menuImage->Append( ID_QUIT, "E&xit\tCtrl-Q" );
  menu_bar->Append( menuImage, "&Image" );

  SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
  CreateStatusBar( 2 );
  int widths[] = { -1, 100 };
  SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED( event ) ) { Close( true ); }

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
  wxArrayString array;

  array.Add( "wxImage demo" );
  array.Add( "(c) Robert Roebling 1998-2005" );
  array.Add( "(c) Vadim Zeitlin 2005-2009" );

  array.Add( wxEmptyString );
  array.Add( "Version of the libraries used:" );

#if wxUSE_LIBPNG
  array.Add( wxPNGHandler::GetLibraryVersionInfo().ToString() );
#endif
#if wxUSE_LIBJPEG
  array.Add( wxJPEGHandler::GetLibraryVersionInfo().ToString() );
#endif
#if wxUSE_LIBTIFF
  array.Add( wxTIFFHandler::GetLibraryVersionInfo().ToString() );
#endif
  (void)wxMessageBox( wxJoin( array, '\n' ), "About wxImage Demo",
                      wxICON_INFORMATION | wxOK );
}

wxImage wx_from_mat( cv::Mat& img )
{
  using namespace cv;
  Mat im2;
  if( img.channels() == 1 ) {
    cvtColor( img, im2, cv::COLOR_GRAY2RGB );
  } else if( img.channels() == 4 ) {
    cvtColor( img, im2, COLOR_BGRA2RGB );
  } else {
    cvtColor( img, im2, COLOR_BGR2RGB );
  }
  long imsize = im2.rows * im2.cols * im2.channels();
  wxImage wx( im2.cols, im2.rows, (unsigned char*)malloc( imsize ), false );
  unsigned char* s = im2.data;
  unsigned char* d = wx.GetData();
  for( long i = 0; i < imsize; i++ ) {
    d[ i ] = s[ i ];
  }
  return wx;
}

cv::Mat mat_from_wx( wxImage& wx )
{
  using namespace cv;
  Mat im2( Size( wx.GetWidth(), wx.GetHeight() ), CV_8UC3, wx.GetData() );
  cvtColor( im2, im2, COLOR_RGB2BGR );
  return im2;
}

wxString MyFrame::LoadUserImage( wxImage& image )
{
  wxString filename;

#if wxUSE_FILEDLG
  filename = wxLoadFileSelector( "image", wxEmptyString );
  if( !filename.empty() ) {

    if( !image.LoadFile( filename ) ) {
      wxLogError( "Couldn't load image from '%s'.", filename );

      return wxEmptyString;
    }
  }
#endif // wxUSE_FILEDLG

  return filename;
}

void MyFrame::OnNewFrame( wxCommandEvent& WXUNUSED( event ) )
{
  wxImage image;
  wxString filename = LoadUserImage( image );

  if( !filename.empty() )
    new MyImageFrame( this, filename, image );
}

void MyFrame::OnNewFrameHiDPI( wxCommandEvent& )
{
  wxImage image;
  wxString filename = LoadUserImage( image );
  if( !filename.empty() )
    new MyImageFrame( this, filename, image, GetContentScaleFactor() );
}

void MyFrame::OnUpdateNewFrameHiDPI( wxUpdateUIEvent& event )
{
  event.Enable( GetContentScaleFactor() > 1 );
}

void MyFrame::OnImageInfo( wxCommandEvent& WXUNUSED( event ) )
{
  wxImage image;
  if( !LoadUserImage( image ).empty() ) {
    // TODO: show more information about the file
    wxString info = wxString::Format( "Image size: %dx%d", image.GetWidth(),
                                      image.GetHeight() );

    int xres = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONX ),
        yres = image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONY );
    if( xres || yres ) {
      info += wxString::Format( "\nResolution: %dx%d", xres, yres );
      switch( image.GetOptionInt( wxIMAGE_OPTION_RESOLUTIONUNIT ) ) {
        default:
          wxFAIL_MSG( "unknown image resolution units" );
          wxFALLTHROUGH;

        case wxIMAGE_RESOLUTION_NONE:
          info += " in default units";
          break;

        case wxIMAGE_RESOLUTION_INCHES:
          info += " in";
          break;

        case wxIMAGE_RESOLUTION_CM:
          info += " cm";
          break;
      }
    }

    wxLogMessage( "%s", info );
  }
}

void MyFrame::OnThumbnail( wxCommandEvent& WXUNUSED( event ) )
{
#if wxUSE_FILEDLG
  wxString filename =
    wxLoadFileSelector( "image", wxEmptyString, wxEmptyString, this );
  if( filename.empty() )
    return;

  static const int THUMBNAIL_WIDTH  = 320;
  static const int THUMBNAIL_HEIGHT = 240;

  wxImage image;
  image.SetOption( wxIMAGE_OPTION_MAX_WIDTH, THUMBNAIL_WIDTH );
  image.SetOption( wxIMAGE_OPTION_MAX_HEIGHT, THUMBNAIL_HEIGHT );

  wxStopWatch sw;
  if( !image.LoadFile( filename ) ) {
    wxLogError( "Couldn't load image from '%s'.", filename );
    return;
  }

  int origWidth  = image.GetOptionInt( wxIMAGE_OPTION_ORIGINAL_WIDTH );
  int origHeight = image.GetOptionInt( wxIMAGE_OPTION_ORIGINAL_HEIGHT );

  const long loadTime = sw.Time();

  MyImageFrame* const frame = new MyImageFrame( this, filename, image );
  wxLogStatus( frame, "Loaded \"%s\" in %ldms; original size was (%d, %d)",
               filename, loadTime, origWidth, origHeight );
#else
  wxLogError( "Couldn't create file selector dialog" );
  return;
#endif // wxUSE_FILEDLG
}

void MyFrame::OnFilters( wxCommandEvent& WXUNUSED( event ) )
{
  ( new MyFiltersFrame( this ) )->Show();
}
