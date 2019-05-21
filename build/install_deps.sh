#!/bin/bash
set -e

if [ -d "$HOME/BYOND-${BYOND_MAJOR}.${BYOND_MINOR}/byond/bin" ];
then
  echo "Using cached BYOND directory."
else
  echo "Setting up BYOND."
  mkdir -p "$HOME/BYOND-${BYOND_MAJOR}.${BYOND_MINOR}"
  cd "$HOME/BYOND-${BYOND_MAJOR}.${BYOND_MINOR}"
  curl "http://www.byond.com/download/build/${BYOND_MAJOR}/${BYOND_MAJOR}.${BYOND_MINOR}_byond_linux.zip" -o byond.zip
  unzip byond.zip
  cd byond
  make here
  cd ~/
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
