#include <iostream>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>

#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Input.H>

#include "compute.hpp"

class ScrollBox : public Fl_Box {
    Fl_Scroll *scroll;
public:
    void SetScroll(Fl_Scroll *val) {
        scroll = val;
    }
    ScrollBox(int x,int y,int w,int h,const char*l=0) : Fl_Box(x,y,w,h,l) {
        color(FL_BLUE);
        box(FL_FLAT_BOX);
    }
};

class SimpleWindow : public Fl_Window {

  public: 
    SimpleWindow(int w, int h, const char* title);
    ~SimpleWindow();

    Fl_Scroll* scroll;
    ScrollBox* scrollbox;

    Fl_PNG_Image* image;

    Fl_Button* button_compute;

    Fl_Value_Input* in_num_iterations;
    Fl_Value_Input* in_threshold;
    Fl_Value_Input* in_alphamin;
    Fl_Value_Input* in_alphamax;
    Fl_Value_Input* in_alpha_num_intervals;
    Fl_Value_Input* in_betamin;
    Fl_Value_Input* in_betamax;
    Fl_Value_Input* in_beta_num_intervals;
    Fl_Value_Input* in_num_seedpoints;
    Fl_Value_Input* in_special_seedpoint;
    Fl_Check_Button* in_output_csv;

  private:
    static void callback_compute(Fl_Widget*, void*);
    inline void callback_compute_il();

};

int main() {
  SimpleWindow win(800,600,"Dynamic Systems");
  return Fl::run();
}

// Constructor
SimpleWindow::SimpleWindow(int w, int h, const char* title):Fl_Window(w,h,title) {
  // between begin...end comes what to show in window
  this->begin(); // this-> is implicit
  
  // Top: ScrollBox containing the picture
  image = new Fl_PNG_Image("picture.png");

  scroll = new Fl_Scroll(0,0,800,400);
  scroll->begin();
  scrollbox = new ScrollBox(0,0,800,400);
  scrollbox->SetScroll(scroll);
  scrollbox->image(image);

  scroll->end();

  // Bottom: inputboxes and button

  int boxwidth = 80;  // width input boxes
  int boxheight = 20;  // height input boxes
  int firstrow = 450;
  int secondrow = 550;
  int padding = 100;

  in_alphamin = new Fl_Value_Input(0*padding,firstrow,boxwidth,boxheight,"alpha_min");
  in_alphamin->align(FL_ALIGN_TOP);
  in_alphamin->value(0.0); 

  in_alphamax = new Fl_Value_Input(1*padding,firstrow,boxwidth,boxheight,"alpha_max");
  in_alphamax->align(FL_ALIGN_TOP);
  in_alphamax->value(1.0); 

  in_alpha_num_intervals = new Fl_Value_Input(2*padding,firstrow,boxwidth,boxheight,"width");
  in_alpha_num_intervals->align(FL_ALIGN_TOP);
  in_alpha_num_intervals->value(400); 

  in_betamin = new Fl_Value_Input(0*padding,secondrow,boxwidth,boxheight,"beta_min");
  in_betamin->align(FL_ALIGN_TOP);
  in_betamin->value(0.0); 

  in_betamax = new Fl_Value_Input(1*padding,secondrow,boxwidth,boxheight,"beta_max");
  in_betamax->align(FL_ALIGN_TOP);
  in_betamax->value(1.0); 

  in_beta_num_intervals = new Fl_Value_Input(2*padding,secondrow,boxwidth,boxheight,"height");
  in_beta_num_intervals->align(FL_ALIGN_TOP);
  in_beta_num_intervals->value(400); 

  in_num_iterations = new Fl_Value_Input(3*padding,firstrow,boxwidth,boxheight,"iterations");
  in_num_iterations->align(FL_ALIGN_TOP);
  in_num_iterations->value(100);

  in_threshold = new Fl_Value_Input(4*padding,firstrow,boxwidth,boxheight,"threshold");
  in_threshold->align(FL_ALIGN_TOP);
  in_threshold->value(1);

  in_num_seedpoints = new Fl_Value_Input(3*padding,secondrow,boxwidth,boxheight,"#seedpoints");
  in_num_seedpoints->align(FL_ALIGN_TOP);
  in_num_seedpoints->value(8);

  button_compute = new Fl_Button(5*padding,firstrow,boxwidth,boxheight,"compute");
  button_compute->type(FL_NORMAL_BUTTON);
  button_compute->color(FL_RED);
  button_compute->callback(callback_compute,this);
  in_special_seedpoint = new Fl_Value_Input(4*padding,secondrow,boxwidth,boxheight,"special seed");
  in_special_seedpoint->align(FL_ALIGN_TOP);
  in_special_seedpoint->value(0);

  in_output_csv = new Fl_Check_Button(5*padding,secondrow,boxwidth,boxheight,"csv output");

  this->end();
  resizable(this);
  this->show();
}

// Destructor
SimpleWindow::~SimpleWindow() {}

// Button callback, just cast object and call real function
void SimpleWindow::callback_compute(Fl_Widget* o, void* v) {
  ( (SimpleWindow*)v )->callback_compute_il();
}

// no arguments needed, because has access to all class members
void SimpleWindow::callback_compute_il() {
  std::cout << "start computation..." << std::endl;

  std::vector<float> seedpoints;
  seedpoints.push_back(in_special_seedpoint->value());

  compute_all(
    in_num_iterations->value(),
    in_threshold->value(),
    in_alphamin->value(),
    in_alphamax->value(),
    in_alpha_num_intervals->value(),
    in_betamin->value(),
    in_betamax->value(),
    in_beta_num_intervals->value(),
    in_num_seedpoints->value(),
    in_output_csv->value(),
    seedpoints
  );

  image = new Fl_PNG_Image("picture.png");
  scrollbox->image(image);
  scrollbox->resize(0,0,image->w(),image->h());
  redraw();
  std::cout << "done." << std::endl;
}

