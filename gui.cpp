#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>

class ScrollBox : public Fl_Box {
    Fl_Scroll *scroll;
public:
    int handle(int e) {
        if ( e == FL_PUSH ) {
            fprintf(stderr, "event_x,event_y: %d,%d, Hit on box: %d,%d\n",
                Fl::event_x(),
                Fl::event_y(),
                Fl::event_x() - scroll->x() + scroll->hscrollbar.value(),
                Fl::event_y() - scroll->y() + scroll->scrollbar.value());
        }
        return(Fl_Box::handle(e));
    }
    void SetScroll(Fl_Scroll *val) {
        scroll = val;
    }
    ScrollBox(int x,int y,int w,int h,const char*l=0) : Fl_Box(x,y,w,h,l) {
        color(FL_BLUE);
        box(FL_FLAT_BOX);
    }
};

int main (int argc, char **argv) {
  fl_register_images();
  Fl_PNG_Image picture("picture.png");
  Fl_Double_Window window(600,400,"Dynamic Systems");
  Fl_Scroll scroll(0,0,600,400);
  ScrollBox box(0,0,picture.w(),picture.h());
	box.SetScroll(&scroll);
  box.image(picture);
	scroll.end();
  window.resizable(window);
  window.show();
  return Fl::run();
}
