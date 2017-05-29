# Open Urban Television - OUT

## Description

Open Urban Television is an Open Framework project implementing computer vision algorithms to the live streams of the public space of the city. 

The implemented algorithm processes the stream to extract the levels of movement and noise:
- Frame differencing is used to extract the levels of movement from the stream.
- For both noise and sound the instantaneous and rolling average levels are calculated. 
- the goal of the code is to introduce minimum latency between the post processed stream and the source

The extracted information is presented to the user in a visually appealing way together with the pre and post processed streams. 

### Software
Project developed using OpenFrameworks and the addon ofxCV (https://github.com/kylemcdonald/ofxCv). 

### Language
C++

### License
BY-SA 4.0
