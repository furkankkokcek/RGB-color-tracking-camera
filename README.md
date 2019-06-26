# RGB-color-tracking-camera
The scope of the project is to develop RGB color tracking camera. This device
would track one of the RGB color by understanding the direction of the color by
looking RGB intensity matrix of image and servo motors would allow the device
to track the color.
Initially our server would periodically analyze frames from video feed of camera
in order to differentiate RGB color direction from its color intensity matrix.
Arduino Uno selected for capturing images. Secondly, server would send
UDP packets to our launchpad that ESP8266 wifi module would capture. This
UDP packet would consist of information that only red values of frames.This approach
provided to compress the packet size and hiding information . Finally,
launchpad tracks color by moving servo motors to its corresponding degree
such as 30.
