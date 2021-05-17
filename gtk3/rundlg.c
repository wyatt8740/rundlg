/**
 * A stupid run dialogue for FVWM3,
 * since FvwmForm is currently missing and my run dialogue in FVWM2 used it.
 * Copyright © 2020 Wyatt Ward.
 * GPL3 license.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include <gdk/gdkkeysyms.h>

/* Functions that run when specific events occur. */
static gboolean check_escape(GtkWidget *widget, GdkEventKey *event, gpointer data);
static void quit_button(GtkWidget *widget, gpointer data);
static void run_button(GtkWidget *widget, gpointer data);

/* 
 * Set a special name so fvwm can apply its rules to it easily.
 * Example FVWM2/FVWM3 rule:
 * Style Wyatt_RunDlg StaysOnTop, Sticky
 */
char *app_name="Wyatt_RunDlg";

/*
 * Dumb global variable hack, because I don't know the right way to get a
 * GtkApplication pointer from a callback function otherwise.
 * If you can help me figure this out, please, please do so. I'm still
 * kind of new to GTK in C.
*/
GtkApplication *mainapp;
/* Similar for fetching text input from a gtkEntry in a callback function: */
GtkEntry *textEntryGlobal;

static void activate (GtkApplication* rundlg, gpointer user_data)
{
  GtkWidget *window;
  window = gtk_application_window_new (rundlg);

  /* Set title */
  gtk_window_set_title (GTK_WINDOW (window), "Run");

  /*
   * Suggest to keep it on top; not all WM's respect this, including FVWM.
   * (I use FVWM, so this is not verified to work on other systems yet).
   */
  gtk_window_set_keep_above(GTK_WINDOW (window), TRUE);

  /*
   * This might be leftover from an earlier program I made.
   * Not sure it's necessary here.
   */
  GdkScreen *screen=gdk_screen_get_default();
  if(screen == NULL)
  {
    fprintf(stderr,"Error: gdk_screen_get_default() returned NULL!\nExiting.");
    exit(1);
  }

  /* make a grid */
  GtkGrid *grid=(GtkGrid*)gtk_grid_new ();
  gtk_grid_set_row_spacing(grid, 2);
  gtk_grid_set_column_spacing(grid, 2);

  /* Add grid to window */
  gtk_container_add (GTK_CONTAINER (window), (GtkWidget*)grid);

  /* create widgets */
  GtkEntry *textEntry=(GtkEntry*)gtk_entry_new();
  textEntryGlobal=textEntry;
  gtk_entry_set_width_chars (textEntry, 40);
  GtkButton *runBtn=(GtkButton*)gtk_button_new_with_label((const gchar*)"Run");
  gtk_widget_set_hexpand ((GtkWidget *)runBtn, TRUE);
  GtkButton *escBtn=(GtkButton*)gtk_button_new_with_label((const gchar*)"Cancel");
  gtk_widget_set_hexpand ((GtkWidget *)escBtn, TRUE);
  if(!textEntry || !runBtn || !escBtn)
  {
    fprintf(stderr, "Error: failed to create a widget!\n");
  }
  g_object_set (textEntry, "margin", 6, NULL);
  g_object_set (runBtn, "margin-bottom", 6, NULL);
  g_object_set (runBtn, "margin-right", 6, NULL);
  g_object_set (escBtn, "margin-bottom", 6, NULL);
  g_object_set (escBtn, "margin-left", 6, NULL);

  GtkLabel *padding=(GtkLabel *)gtk_label_new("");
  gtk_widget_set_hexpand ((GtkWidget *)padding, TRUE);;
  
  /* pack into grid */
  gtk_grid_attach(grid,(GtkWidget*)textEntry,0,0,3,1);
  gtk_grid_attach(grid,(GtkWidget*)escBtn,0,1,1,1);
  gtk_grid_attach(grid,(GtkWidget*)padding,1,1,1,1);
  gtk_grid_attach(grid,(GtkWidget*)runBtn,2,1,1,1);

  /* all grid slots are equally wide */
  gtk_grid_set_column_homogeneous((GtkGrid *)grid, TRUE);
  
  /* make text input default focus */
  gtk_entry_grab_focus_without_selecting(textEntry);
  gtk_entry_set_activates_default(textEntry, TRUE);
  gtk_widget_set_can_default ((GtkWidget*)runBtn, TRUE);
  gtk_widget_grab_default ((GtkWidget*)runBtn);

  /*
   * Make buttons do things. Also make escape key work to quit.
   * (escape key is actually an improvement over my FvwmForm dialogue)
   */
  g_signal_connect(window, "key_press_event", G_CALLBACK(check_escape), NULL);
  g_signal_connect(escBtn, "clicked", G_CALLBACK(quit_button), NULL);

  g_signal_connect(runBtn, "clicked", G_CALLBACK(run_button), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_widget_show_all (window);
}

static gboolean check_escape(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (event->keyval == GDK_KEY_Escape) {
    g_application_quit(G_APPLICATION(mainapp));
    return TRUE;
  }
  return FALSE;
}

static void run_button(GtkWidget *widget, gpointer data)
{
  if(gtk_entry_get_text_length (textEntryGlobal) > 0)
  {
    const gchar *runcmd = gtk_entry_get_text(textEntryGlobal);
    /* +1 for '&', +1 for null terminator */
    char *newcmd=(char*)malloc(sizeof(char)*strlen(runcmd)+2);
    /* check that malloc succeeded; if so, add ampersand to command */
    if(newcmd)
    {
      /*
       * We're guaranteed that runcmd is already null terminated as returned
       * from GTK.
       */
      strcpy(newcmd, runcmd);
      size_t len=strlen(newcmd);
      newcmd[len]='&';
      newcmd[len+1]='\0';
      if(len+1 != strlen(newcmd))
      {
        fprintf(stderr, "ERROR: Something's horribly wrong with my concatenation.\n");
        fprintf(stderr, "length is %zu, should be %zu\n",strlen(newcmd),len+1);
      }
      system(newcmd); /* Maybe a security issue? Probably. Almost definitely. */
      free(newcmd); /* never forget to free. */

      /* We're done, so quit. */
      g_application_quit(G_APPLICATION(mainapp));
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

static void quit_button(GtkWidget *widget, gpointer data)
{
  g_application_quit(G_APPLICATION(mainapp));
}

int main (int argc, char **argv)
{   
  /* GTK applet initialization stuff */
  GtkApplication *rundlg;
  int status;  
  
  rundlg = gtk_application_new ("org.wyatt8740.rundlg", G_APPLICATION_FLAGS_NONE);
  mainapp=rundlg; /* global */
  g_signal_connect (rundlg, "activate", G_CALLBACK (activate), NULL);

  g_set_prgname(app_name); /* instead of wmclass which is deprecated */
  status = g_application_run (G_APPLICATION (rundlg), argc, argv);
  g_object_unref (rundlg);
  /* wait(NULL); */
  return status;
}
