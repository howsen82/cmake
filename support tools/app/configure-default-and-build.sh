#!/bin/bash

./configure-default.sh
cmake --build build --config RelWithDebInfo --target install
