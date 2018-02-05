#include <iostream>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_PNG_Image.H>

#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Input.H>

#include "compute.hpp"

class ScrollBox : public Fl_Box {
    Fl_Scroll *scroll;
    int window_x_1;
    int window_y_1;
    int param_x_1;
    int param_y_1;
    int window_x_2;
    int window_y_2;
    int param_x_2;
    int param_y_2;
public:
    int handle(int e) {
        if ( e == FL_PUSH ) {
          // register mouse push as first corner of selection-rectangle
          window_x_1 = Fl::event_x();
          window_y_1 = Fl::event_y();
          param_x_1 = Fl::event_x() - scroll->x() + scroll->hscrollbar.value();
          param_y_1 = Fl::event_y() - scroll->y() + scroll->scrollbar.value();
            fprintf(stderr, "event_x,event_y: %d,%d, Hit on box: %d,%d\n",
              window_x_1, window_y_1, param_x_1, param_y_1);
        } else if (e == FL_RELEASE) {
          // register mouse release as second corner of selection-rectangle
          window_x_2 = Fl::event_x();
          window_y_2 = Fl::event_y();
          param_x_2 = Fl::event_x() - scroll->x() + scroll->hscrollbar.value();
          param_y_2 = Fl::event_y() - scroll->y() + scroll->scrollbar.value();
          
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

struct Params {

  Fl_Value_Input *in_num_iterations;
  Fl_Value_Input *in_threshold;
  Fl_Value_Input *in_alphamin;
  Fl_Value_Input *in_alphamax;
  Fl_Value_Input *in_alpha_num_intervals;
  Fl_Value_Input *in_betamin;
  Fl_Value_Input *in_betamax;
  Fl_Value_Input *in_beta_num_intervals;
  Fl_Value_Input *in_num_seedpoints;
  Fl_Check_Button *in_output_csv;

  ScrollBox *box;
  Fl_PNG_Image *picture;

  int num_iterations;
  float threshold;
  float alphamin;
  float alphamax;
  int alpha_num_intervals;
  float betamin;
  float betamax;
  int beta_num_intervals;
  int num_seedpoints;
  std::vector<float> seedpoints = {};
  bool output_csv;
};

void buttonPushed(Fl_Widget*, void* data) {
  std::cout << "Button pushed. Do some work now!" << std::endl;

  // save parameters from input boxes into variables
  Params *params = reinterpret_cast<Params*>(data);
  params->num_iterations = params->in_num_iterations->value();
  params->threshold = params->in_threshold->value();

  params->alphamin = params->in_alphamin->value();
  params->alphamax = params->in_alphamax->value();
  params->alpha_num_intervals = params->in_alpha_num_intervals->value();

  params->betamin = params->in_betamin->value();
  params->betamax = params->in_betamax->value();
  params->beta_num_intervals = params->in_beta_num_intervals->value();

  params->num_seedpoints = params->in_num_seedpoints->value();
//  params->seedpoints = new std::vector<float>(0);
  params->output_csv = params->in_output_csv->value();

  // call compute function with parameters
  std::cout << "amin:" << params->alphamin << std::endl;
  std::cout << "amax:" << params->alphamax << std::endl;
  std::cout << "csv?:" << params->output_csv << std::endl;

  compute_all(
   params->num_iterations,
   params->threshold,
   params->alphamin,
   params->alphamax,
   params->alpha_num_intervals,
   params->betamin,
   params->betamax,
   params->beta_num_intervals,
   params->num_seedpoints,
   params->output_csv,
   params->seedpoints
  );
  // redraw image after computation
//  params->picture->draw();
  params->box->image(params->picture);
}

int main (int argc, char **argv) {
  Params input;

  fl_register_images();
  Fl_PNG_Image picture("picture.png");
  Fl_Double_Window window(800,600,"Dynamic Systems");
  Fl_Scroll scroll(0,0,800,400);
  ScrollBox box(0,0,picture.w(),picture.h());
	box.SetScroll(&scroll);
  box.image(picture);
	scroll.end();
  // Parameter input
  int boxwidth = 80;  // width input boxes
  int boxheight = 20;  // height input boxes
  int firstrow = 450;
  int secondrow = 500;
  int padding = 100;

  input.in_alphamin = new Fl_Value_Input(0*padding,firstrow,boxwidth,boxheight,"alpha_min");
  input.in_alphamin->align(FL_ALIGN_TOP);
  input.in_alphamin->value(0.0); 
  input.in_alphamax = new Fl_Value_Input(1*padding,firstrow,boxwidth,boxheight,"alpha_max");
  input.in_alphamax->align(FL_ALIGN_TOP);
  input.in_alphamax->value(1.0); 
  input.in_alpha_num_intervals = new Fl_Value_Input(2*padding,firstrow,boxwidth,boxheight,"width");
  input.in_alpha_num_intervals->align(FL_ALIGN_TOP);
  input.in_alpha_num_intervals->value(400); 

  input.in_betamin = new Fl_Value_Input(0*padding,secondrow,boxwidth,boxheight,"beta_min");
  input.in_betamin->align(FL_ALIGN_TOP);
  input.in_betamin->value(0.0); 
  input.in_betamax = new Fl_Value_Input(1*padding,secondrow,boxwidth,boxheight,"beta_max");
  input.in_betamax->align(FL_ALIGN_TOP);
  input.in_betamax->value(1.0); 
  input.in_beta_num_intervals = new Fl_Value_Input(2*padding,secondrow,boxwidth,boxheight,"height");
  input.in_beta_num_intervals->align(FL_ALIGN_TOP);
  input.in_beta_num_intervals->value(400); 

  input.in_num_iterations = new Fl_Value_Input(3*padding,firstrow,boxwidth,boxheight,"iterations");
  input.in_num_iterations->align(FL_ALIGN_TOP);
  input.in_num_iterations->value(100);
  input.in_threshold = new Fl_Value_Input(4*padding,firstrow,boxwidth,boxheight,"threshold");
  input.in_threshold->align(FL_ALIGN_TOP);
  input.in_threshold->value(1);

  input.in_num_seedpoints = new Fl_Value_Input(3*padding,secondrow,boxwidth,boxheight,"#seedpoints");
  input.in_num_seedpoints->align(FL_ALIGN_TOP);
  input.in_num_seedpoints->value(8);

  // Button
  Fl_Button *button = new Fl_Button(5*padding,firstrow,boxwidth,boxheight,"compute");
  button->type(FL_NORMAL_BUTTON);
  button->color(FL_RED);
  button->callback(buttonPushed,&input);

  input.in_output_csv = new Fl_Check_Button(5*padding,secondrow,boxwidth,boxheight,"csv output");

  window.resizable(window);
  window.show();
  return Fl::run();
}
