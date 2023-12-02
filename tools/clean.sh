#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $CURRENT_DIR

rm -rf linux_java
rm -rf mac_java
rm -rf win_csharp
rm -rf win_java
rm -rf server_demo
rm -rf win_node
cd ../swig
rm -rf *.exp *.lib *.dll *.exe *.obj *.class com/ build/ *_wrap.c*
