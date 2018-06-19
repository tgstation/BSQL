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

wget http://mirrors.kernel.org/ubuntu/pool/universe/m/mariadb-connector-c/libmariadb-dev_2.3.3-1_i386.deb
sudo dpkg -i libmariadb-dev_2.3.3-1_i386.deb
