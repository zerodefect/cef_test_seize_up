# cef_test_seize_up

This repository is a test to validate the Chromium Embedded Framework (CEF) when run using manual-frame scheduling via SendExternalBeginFrame() as opposed to relying on a screen's vertical sync.

It has been observed that the client often locks up on initalisation - https://magpcss.org/ceforum/viewtopic.php?f=6&t=17967


## Dependencies

This project has been created using:
* CMake + gcc v9.2
* Boost::filesystem
* libevent (v2)
* X11
* pthread

Please be aware that the CEF is pulled down using CMake's built-in ExternalProject_Add() functionality.

The code has been compiled successfully on Ubuntu v20.04 with gcc v9.2
