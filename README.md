mice2mouse
==========

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