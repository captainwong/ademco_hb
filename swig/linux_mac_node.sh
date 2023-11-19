#!/bin/bash

set -e

swig -c++ -javascript -node libademco.swig
node-gyp configure build
