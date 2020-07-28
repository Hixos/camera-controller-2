#!/bin/bash

python3 generate_events.py events.txt topics.txt generated

mv -f generated/CCEvents.cpp ../../src/cameracontroller/CCEvents.cpp
mv -f generated/CCEvents.h ../../src/cameracontroller/CCEvents.h