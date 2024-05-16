#!/bin/bash

make clean
make DEBUG=1 WARNINGS=1
make install
