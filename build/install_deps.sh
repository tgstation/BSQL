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

if [ -f "$HOME/MariaDB/libmariadb.so" ];
then
  echo "Using cached MariaDB directory."
  exit 0
else
  echo "Setting up MariaDB."
  rm -rf "$HOME/MariaDB"
  mkdir -p "$HOME/MariaDB"
  curl "http://mirrors.kernel.org/ubuntu/pool/universe/m/mariadb-connector-c/libmariadb-dev_2.3.3-1_i386.deb" -o mariadb.deb
  dpkg -x mariadb.deb /tmp/extract
  rm mariadb.deb
  mv /tmp/extract/usr/lib/i386-linux-gnu/libmariadb.so $HOME/MariaDB/
  rm -rf /tmp/extract
  exit 0
fi

#some variable not set correctly, panic
exit 1
