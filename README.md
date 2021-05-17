# rundlg

A standalone GTK+3 run dialogue program, written in C.

FLTK version:

![Screenshot of the run dialogue window (FLTK version)](/screenshot-fltk.png?raw=true)

GTK+3 Version:

![Screenshot of the run dialogue window (GTK+3 version)](/screenshot-gtk3.png?raw=true)

I made this pretty much just for my own use alongside FVWM3, since in
the transition from FVWM2 to 3 it had its FvwmForm support removed (at least
temporarily). Thus I had to implement a new one, as my previously used run
dialog was written as FvwmForm configuration syntax.

### Known problems

It uses a `system()` call, which I could possibly replace with some variant of
the `exec()` call for better security. 

Also, I use a couple global variables because I'm still not sure how to get a
pointer to a GtkApplication from within a callback function. I'm still very
new to GTK. If you know what to do instead, please let me know.

I have not tested it outside of FVWM (on other window managers), so it may not
stay on top or may otherwise misbehave on them and I would not know.

If you use this program, please report any bugs you have.
