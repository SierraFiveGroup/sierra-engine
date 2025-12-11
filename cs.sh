#!/bin/bash

#This file is used to compile all the vulkan shaders

#declare -a files=$(find test_shaders/vk -type f)

find test_shaders/vk -type f -iregex ".*\.\(vert\|frag\)" |
while read f
do
    glslc $f -o $f.spv
done