#include "Frame.hpp"

class MyApp : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

wxIMPLEMENT_APP( MyApp );

bool MyApp::OnInit()
{
  if( !wxApp::OnInit() )
    return false;

  wxInitAllImageHandlers();

  wxFrame* frame = new MyFrame();
  frame->Show( true );

  return true;
}
