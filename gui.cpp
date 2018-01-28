#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>

int main (int argc, char **argv) {
  fl_register_images();
  Fl_Double_Window window(600,400,"Dynamic Systems");
  Fl_Scroll scroll(0,0,600,400);
  Fl_PNG_Image picture("picture.png");
  Fl_Box box(0,0,picture.w(),picture.h());
  box.image(picture);
  window.resizable(window);
  window.show();
  return Fl::run();
}
