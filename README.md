mice2mouse
==========

News
--------------------
16.01.2012: Added screenshot functionality - output is a vector image :)


Aims of this project
--------------------

An attempt to merge outputs from 2 pointing devices (i.e. mouse,
trackpoint, touchpad ...) into single 3D cursor (like space ball)
and to try usability of such configuration.

### Highlights
* each mouse determines coordinates on 1 plane (xy, xz or yz). In the
  end, each mouse controls coordinate of 1 axis on its own and the last
  (probably z) axis is beeing shared by both mice (resulting z coordinate
  is then averaged)
* using some special pad - orthogonal to the table (say in plane yz),
  the second mouse will be carried over yz even in reality. Just for
  start, advanced users probably wont carry heavy mouse in the air
  all the time
* possible usage:
    * manipulate (drag & drop, rotate, morph) objects in 3D scene easily
    * 3D drawing
    * crane controll
    * or something totally different - the goal is to experiment with
      possible forms of interaction
* demo included: tic-tac-toe (without logic)
* under Linux, you need to specify 2 device files, from where input
  events are read (something like /dev/input/eventX).
  Windows is not supported yet.

### Uinput
There is a program m2m\_device.c (uses Linux uinput kernel module), which does
the translation and the output (3D coordinates and 6 buttons) is then
accessible as a m2m joystick in /dev/input/ (use `sudo evtest` to list
available input devices).
This device reports EV\_ABS values, but values are relative, just because
we didnt find another way, how to feed GLUT or SDL with 3D events. To test
this, use the CApp application (in SDL). Before that, create m2m device
by running the uinput daemon:

    sudo ./m2m_device mouseA mouseB


### Files
    CApp.*, CEvent.*  - SDL application which uses m2m joystick created
                        with uinput
    mice2mouse.*      - C module, which can translate events from 2 mice
                        into 1 3D device. Events are fed into client application
                        using clients callbacks. See mice2mouse.h for more info
                        (deprecated, in demos, we are now using m2m joystick
                        created with m2m_device)
    main.cpp          - application using mice2mouse.o module services
    hunt_log.*        - class used in main.cpp to harvest statistical data
                        about clicks timing
    m2m_device.c      - create virtual 3D m2m joystick using uinput and then
                        feed it with merged events from /dev/input/eventAB
    joytest.c         - uses SDL to get info from system about available
                        joysticks and theirs options

    delta_correct.py  - input: raw log file from testing application
                      - output: interclick delay between 2 correct hits
    filter.py         - linear filter (median, mean, ..) you can use to smooth
                        output from delta_correct.py

### Typical usage
* start in background m2m\_device daemon (it does the translation), in command line parameters specify 2 input devices, which should be merged (Example: ./m2m\_device /dev/input/event10 /dev/input/event11).
* run some demo application, f.e. DrawingDemo

Note, that itu\_demo uses own translator and so you have to give to it same parameters as to m2m\_device daemon.
Also note, that default mapping of 4 input axes to 3 output axes can be fully customized, by the mapping string (now simply hardcoded into source - TODO).
