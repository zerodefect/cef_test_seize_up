# cef_test_seize_up

This repository is a test to validate the Chromium Embedded Framework (CEF) when run using manual-frame scheduling via SendExternalBeginFrame() as opposed to relying on a screen's vertical sync.

It has been observed that the client often locks up on initalisation - https://magpcss.org/ceforum/viewtopic.php?f=6&t=17967


## Dependencies

