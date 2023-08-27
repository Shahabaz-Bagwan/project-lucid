#include <L2DFileDialog.h>
#include <nlohmann/json.hpp>
#include <project-lucid/lib.h>
#include <string>

static void glfw_error_callback( int error, const char* description )
{
  fprintf( stderr, "GLFW Error %d: %s\n", error, description );
}

// Main code
int main( int, char** )
{
  glfwSetErrorCallback( glfw_error_callback );
  if( !glfwInit() )
    return 1;

  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow( 1280, 720, "JsonCreationTool", nullptr, nullptr );
  if( window == nullptr )
    return 1;
  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 ); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL( window, true );
  ImGui_ImplOpenGL3_Init( glsl_version );

  // Our state
  bool show_demo_window    = false;
  bool show_another_window = false;
  ImVec4 clear_color       = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );

  // Application variables
  char inputText[ 256 ] = "";
  char fieldName[ 256 ] = "";
  char fieldData[ 256 ] = "";

  std::string displayedText;
  bool addFieldClicked = false;
  // List of field options for the dropdown
  std::vector< std::pair< std::string, std::string > > fieldOptions = { { "start", "" } };

  int selectedFieldIndex = 0; // Index of the selected field in fieldOptions

  // ImGui::SetNextWindowSize( ImVec2( 740.0f, 600.0f ) );

  // Main loop
  while( !glfwWindowShouldClose( window ) ) {

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Add text to list
    ImGui::InputText( "Add field", fieldName, sizeof( fieldName ) );
    ImGui::InputText( "Add field data", fieldData, sizeof( fieldData ) );
    if( ImGui::Button( "Add" ) ) {
      fieldOptions.push_back( std::make_pair( fieldName, fieldData ) );
      inputText[ 0 ]  = '\0'; // Clear input field
      addFieldClicked = true;
    }
    // Create a dropdown to select a field
    const char* selectedField = fieldOptions[ selectedFieldIndex ].first.c_str();
    if( ImGui::BeginCombo( "Select Field", selectedField ) ) {
      for( int i = 0; i < fieldOptions.size(); ++i ) {
        bool isSelected = ( selectedFieldIndex == i );
        if( ImGui::Selectable( fieldOptions[ i ].first.c_str(), isSelected ) ) {
          selectedFieldIndex = i;
          displayedText      = ""; // Reset displayed text when field is changed
        }
        if( isSelected ) {
          ImGui::SetItemDefaultFocus();
        }
      }

      // Add the new field to the fieldOptions if it doesn't already exist
      auto it = std::find_if( fieldOptions.begin(), fieldOptions.end(), [ & ]( auto& fname ) {
        return fname.first == selectedField;
      } );
      if( it == fieldOptions.end() ) {
        fieldOptions.push_back( std::make_pair( selectedField, "" ) );
      }
      ImGui::EndCombo();
    }
    if( ImGui::Button( "Remove" ) ) {

      fieldOptions[ selectedFieldIndex ];
    }
    // Display the entered text in a separate textbox
    if( ImGui::Button( "Display" ) ) {
      // Create a JSON object
      nlohmann::json jsonObject;

      // if( addFieldClicked ) {
      // Get the selected field from the dropdown
      const std::string selectedField = fieldOptions[ selectedFieldIndex ].first;
      jsonObject[ selectedField ]     = fieldOptions[ selectedFieldIndex ].second;

      // addFieldClicked = false;
      // }
      // Serialize the JSON object to a formatted string
      displayedText = jsonObject.dump( 4 ); // Use 4 spaces for indentation
    }

    ImGui::Text( "Formatted JSON:\n%s", displayedText.c_str() );

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize( window, &display_w, &display_h );
    glViewport( 0, 0, display_w, display_h );
    glClearColor( clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                  clear_color.z * clear_color.w, clear_color.w );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    glfwSwapBuffers( window );
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}
