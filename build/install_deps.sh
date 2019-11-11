#!/bin/bash
set -e

if [ -d "$HOME/BYOND/byond/bin" ];
then
  echo "Using cached BYOND directory."
else
  echo "Setting up BYOND."
  mkdir -p "$HOME/BYOND"
  cd "$HOME/BYOND"
  curl "http://www.byond.com/download/build/512/512.1488_byond_linux.zip" -o byond.zip
  unzip byond.zip
  cd byond
  make here
  source $HOME/BYOND/byond/bin/byondsetup
  cd $HOME/
fi

if [ -f "$HOME/MariaDB/libmariadb.so.2" ] && [ -f "$HOME/MariaDB/libmariadb.so" ];
then
  echo "Using cached MariaDB library."
else
  echo "Setting up MariaDB."
  rm -rf "$HOME/MariaDB"
  mkdir -p "$HOME/MariaDB"
  cp /usr/lib/i386-linux-gnu/libmariadb.so.2 $HOME/MariaDB/
  ln -s $HOME/MariaDB/libmariadb.so.2 $HOME/MariaDB/libmariadb.so
fi

if [ -d "$HOME/vcpkg/triplets/x86-linux.cmake" ];
then
  echo "x86 library located"
else
  echo "Building Cmake x86 libraries"
  cp $HOME/BSQL/build/x86-linux.cmake $HOME/vcpkg/triplets
fi
