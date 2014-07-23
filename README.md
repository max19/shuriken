Shuriken Beat Slicer
====================

Shuriken is an open source beat slicer which harnesses the power of aubio's onset detection algorithms and Rubber Band's time stretching capabilities. A simple Qt4 interface makes it easy to slice up drum loops, assign individual drum hits to MIDI keys, and change the tempo of loops in real-time. JUCE takes care of handling audio and MIDI behind the scenes.

Shuriken can either be built using Qt Creator or the supplied build script. To use the build script, simply open a terminal in Shuriken's root directory and enter:

    ./build (or optionally ./build debug)

To clean the project:

    ./build clean

You must have qmake-qt4, and the aubio (>=0.4.0) and rubberband (>=1.3) dev files installed on your system.
___

As noted above, Shuriken requires the latest incarnation of the aubio library (>= 0.4.0) which I've packaged for Ubuntu Precise and AVLinux 6:

https://launchpad.net/~rock-hopper/+archive/ubuntu/audiotools

https://dl.dropboxusercontent.com/u/23511236/libaubio4_0.4.1-1avl6_i386.deb
https://dl.dropboxusercontent.com/u/23511236/libaubio4-dev_0.4.1-1avl6_i386.deb
https://dl.dropboxusercontent.com/u/23511236/libaubio4-doc_0.4.1-1avl6_all.deb
https://dl.dropboxusercontent.com/u/23511236/libaubio4_0.4.1-1avl6.source.tar.gz

I'm aware that some software depends on the old aubio library so I've made sure there are no dependency issues with libaubio4 and the old libaubio2 package: the two sit side-by-side and applications which require the old aubio library are unaffected by the presence of the new.

I've also packaged the aubio4 dev files, which are needed to build Shuriken.  It isn't possible to have both the old libaubio-dev package and the new libaubio4-dev package installed at the same time, but it's easy enough to uninstall one and (re)install the other.
