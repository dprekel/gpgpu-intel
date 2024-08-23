#!/bin/bash

make clean
make DEBUG=1 INFO=1 WARNINGS=0
make install
