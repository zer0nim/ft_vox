#!/bin/bash

# dependencies:
# 	glfw, glm, freetype, boost

# Linux
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	echo "install linux dependencies"
	sudo apt-get -y install libglfw3-dev libglfw3 \
	libglm-dev \
	libfreetype6-dev libfontconfig1-dev \
	libboost-all-dev;
# Mac OSX
elif [[ "$OSTYPE" == "darwin"* ]]; then
	echo "install osx dependencies"
	brew install boost  # c++ lib
	brew install glfw3  # window management
	brew install glm  # matrix/quaternions
	brew install freetype  # text on opengl
fi
