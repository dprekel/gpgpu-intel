#!/bin/bash

make clean
make DEBUG=0 INFO=1 WARNINGS=0
make install
