/* Wyatt Ward, 2021

A variant of my originally-GTK3 run dialogue program, this time in FLTK.
This is my first FLTK GUI program I've ever written.

I decided to rewrite this based on a few things:

1) A recommendation to try FLTK from a friend of mine on IRC
2) Annoyance with how long it took the GTK3 run dialogue to load on my laptop
     (which still uses an HDD, by the way)
3) A short encounter with an FLTK app in the wild (Dillo) which seemed to be
     pretty snappy.
4) My desktop theme doesn't look that different from the default FLTK theme, so
     the appearance doesn't bother me much at all. :)

Startup time is much improved, especially with the statically linked version.

I think I might use FLTK for more stuff from now on; it seemed generally
easier to write for me than the GTK program was as well.
*/

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
/* boxes */
#include <FL/Fl_Box.H>
/* buttons */
#include <FL/Fl_Button.H>
/* for adding new fonts */
#include <FL/fl_draw.H>
/* text input field */
#include <FL/Fl_Input.H>

#include <stdio.h>

/*
  These two definitions can be changed or undefined and the program should
  still work. Definitely change it if you don't have MS Sans Serf installed.
  I use it because it looks good without antialiasing and doesn't suffer as
  badly as Tahoma does hinting-wise under Harfbuzz in modern GTK3. That said,
  Tahoma would actually render just fine here because FLTK isn't GTK :D
*/
#define CUSTOM_TYPEFACE "Microsoft Sans Serif"
#define CUSTOM_TYPEFACE_SIZE 11

/* needs to be global so I can access it from run button callback */
Fl_Input *text_input;

void font_init();
void box_button_setup(Fl_Button *button);
void box_input_setup(Fl_Input *input);
void widget_setup(Fl_Widget *widget);
void cancel_cb(Fl_Widget *ob);
void run_cb(Fl_Widget *ob);

/*
   note: on high-DPI displays, you may need to set the FLTK_SCALING_FACTOR
   environment variable when running FLTK programs if fltk can't automatically
   determine your screen DPI.
   I'd do that in ~/.xsessionrc, ~/.bashrc, ~/.profile, etc. - whatever gets
   set earliest/at the highest level upon login in your configuration.
*/

int main(int argc, char **argv) {
  /* Fl::get_system_colors(); */
  /* we are going to force the 'none' style to look more like my Motif/CDE
     desktop */
  Fl::background(174,178,195);
  Fl::background2(255,247,233);

/* To center window on screen, need to get info on window & screen first. */
  int screen_x, screen_y, screen_w, screen_h;
/* Uncomment this line and comment the one below if you do care about
   working area: */
/*Fl::screen_work_area(&screen_x, &screen_y, &screen_w, &screen_h);*/
  Fl::screen_xywh(screen_x, screen_y, screen_w, screen_h);
  
  Fl_Window *window = new Fl_Window(262,64,"Run");
  
  /* 
   * Set a special wm class so fvwm can apply its rules to it easily.
   * Example FVWM2/FVWM3 rule:
   * Style Wyatt_RunDlg StaysOnTop, Sticky
   */
  window->xclass("Wyatt_RunDlg");

  int dec_width  = window->decorated_w();
  int dec_height = window->decorated_h();

  int win_init_x = ((screen_w - dec_width) / 2);
  int win_init_y = ((screen_h - dec_height) / 2);
  
  window->position(win_init_x,win_init_y);
  
  /* handle custom font creation (see preprocessor defines up top) */
  font_init();

  /* Fl_Button *cancel_box = new Fl_Button(20, 20, 80, 25, "&Beep");*/

  Fl_Button *cancel_box = new Fl_Button(6, 37, 80, 21, "Cancel");
  box_button_setup(cancel_box);
  cancel_box->callback(cancel_cb);
  Fl_Button *run_box = new Fl_Button(176, 37, 80, 21, "Run");
  box_button_setup(run_box);
  run_box->callback(run_cb);

  /* text_input already defined globally, just initialized now */
  text_input = new Fl_Input(6, 6, 250, 23);
  box_input_setup(text_input);
  text_input->take_focus();
  text_input->callback(run_cb);
  text_input->when(FL_WHEN_ENTER_KEY_ALWAYS);

  /* cancel_box->labeltype(FL_SHADOW_LABEL); */
  window->end();
  window->show(argc, argv);
  return Fl::run();
}

/* fortunately(?) fltk already listens for the escape key to exit, so I only
   have to worry about making the cancel button work and listening for the
   return (enter) key. */
void cancel_cb(Fl_Widget *ob)
{
  exit(0);
}

void run_cb(Fl_Widget *ob)
{
  const char *runcmd = ((Fl_Input*)text_input)->value();
  if(strlen(runcmd) > 0)
  {
    /* +1 for '&', +1 for null terminator */
    char *newcmd=(char*)malloc(sizeof(char)*strlen(runcmd)+2);
    /* check that malloc succeeded; if so, add ampersand to command */
    if(newcmd)
    {
      /*
       * We're (hopefully) guaranteed that runcmd is already null terminated as
       * returned from FLTK.
       */
      strcpy(newcmd, runcmd);
      size_t len=strlen(newcmd);
      newcmd[len]='&'; /* overwrite old terminator */
      newcmd[len+1]='\0'; /* add new terminator */
      if(len+1 != strlen(newcmd))
      {
        fprintf(stderr, "ERROR: Something's horribly wrong with my concatenation.\n");
        fprintf(stderr, "length is %zu, should be %zu\n",strlen(newcmd),len+1);
      }
      system(newcmd); /* Maybe a security issue? Probably. Almost definitely! */
      free(newcmd); /* never forget to free. */
      /* We're done, so quit. */
      exit(0);
    }
    else
    {
      fprintf(stderr,"Error: Could not malloc() space for command string! (Out of memory?)\n");
    }
    /*
     * don't quit app if failed to malloc, so I know something's wrong when launching
     * with a shortcut key from FVWM.
     */
  }
  else
  {
    fprintf(stderr,"E: No text was entered. Doing nothing.\n");
  }
}

void font_init()
{
  /* YUCK. todo: Fix this ugly preprocessor garbage for font selection
   at compile-time. */
#if defined(CUSTOM_TYPEFACE)
  Fl::set_font(FL_FREE_FONT, CUSTOM_TYPEFACE);
#if defined(CUSTOM_TYPEFACE_SIZE)
  fl_font(FL_FREE_FONT, CUSTOM_TYPEFACE_SIZE);
#else
  fl_font(FL_FREE_FONT, 11); /* default to 11px */
#endif /* defined(CUSTOM_TYPEFACE_SIZE) */
#else /* default to FL_HELVETICA if no custom font is defined */
#if defined(CUSTOM_TYPEFACE_SIZE)
  fl_font(FL_HELVETICA, CUSTOM_TYPEFACE_SIZE);
#else
  fl_font(FL_HELVETICA, 11); /* default to 11px */
#endif /* defined(CUSTOM_TYPEFACE_SIZE) */
#endif /* defined(CUSTOM_TYPEFACE) */
}

/* handles basic configuration of button box properties */
void box_button_setup(Fl_Button *button)
{
  /* button->button(FL_UP_BOX); */
  widget_setup((Fl_Widget *) button);
}

void box_input_setup(Fl_Input *input)
{
  widget_setup((Fl_Widget *)input);
  input->textfont(fl_font());
#if defined(CUSTOM_TYPEFACE_SIZE)
  input->textsize(CUSTOM_TYPEFACE_SIZE);
#else
  input->textsize(11);
#endif
}

void widget_setup(Fl_Widget *widget)
{
  widget->labelfont(fl_font());
  /* widget->labelfont(FL_BOLD+FL_ITALIC); */
#if defined(CUSTOM_TYPEFACE_SIZE)
  widget->labelsize(CUSTOM_TYPEFACE_SIZE);
#else
  widget->labelsize(11);
#endif
}