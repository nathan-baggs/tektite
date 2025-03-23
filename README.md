# Tektite

[![Video Link](https://img.youtube.com/vi/uYpAY1BPW64/0.jpg)](https://www.youtube.com/watch?v=uYpAY1BPW64)

A small self contained game that fits inside a small image.

## Features
- 3D graphics (OpenGL)
- Per-pixel lighting
- Procedural texture generation
- Procedural model generation
- Normal mapping
- Sound

## Design
Several liberties had to be taken in order to make the game small, including removing all default libraries. As such the code is pretty terrible.

A list of things that make me sad about the code:
- No exceptions
- No templates
- Minimal error handling
- No clean-up
- No unit tests

I built this using [msvc-wine](https://github.com/mstorsjo/msvc-wine) and WSL, YMMV.

Good luck!
