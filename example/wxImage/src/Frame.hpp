#pragma once
#include <project-lucid/lib.hpp>
enum
{
  ID_QUIT  = wxID_EXIT,
  ID_ABOUT = wxID_ABOUT,
  ID_NEW   = 100,
  ID_NEW_HIDPI,
  ID_INFO,
  ID_SHOWTHUMBNAIL,
  ID_FILTERS
};

class MyFrame : public wxFrame
{
public:
  MyFrame();

  void OnAbout( wxCommandEvent& event );
  void OnNewFrame( wxCommandEvent& event );
  void OnNewFrameHiDPI( wxCommandEvent& );
  void OnImageInfo( wxCommandEvent& event );
  void OnThumbnail( wxCommandEvent& event );
  void OnFilters( wxCommandEvent& event );
  void OnUpdateNewFrameHiDPI( wxUpdateUIEvent& );

  void OnQuit( wxCommandEvent& event );

private:
  // ask user for the file name and try to load an image from it
  //
  // return the file path on success, empty string if we failed to load the
  // image or were cancelled by user
  static wxString LoadUserImage( wxImage& image );

private:
  friend class MyFiltersFrame;

private:
  wxDECLARE_DYNAMIC_CLASS( MyFrame );
  wxDECLARE_EVENT_TABLE();
};