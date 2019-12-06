#!/bin/bash
echo "Updating"
sudo apt update
echo "Installing Software"
sudo apt-get install curl unzip tar
sudo apt-get install build-essential
sudo apt-get install gcc-multilib
sudo apt-get install g++-multilib
sudo apt-get install gcc-4.8-multilib
sudo apt-get install g++-4.8-multilib
sudo apt-get install cmake
if [ -d "$HOME/BSQLdependencies" ];
then
  echo "Dependencies Already Exist"
else
  echo "Installing Dependencies"
  mkdir $HOME/BSQLdependencies
  cd $HOME/BSQLdependencies
  wget -O libmariadb.deb http://launchpadlibrarian.net/322701203/libmariadb2_2.3.3-1_i386.deb
  wget -O libmariadbclient.deb http://launchpadlibrarian.net/437033517/libmariadbclient-dev_10.1.41-0ubuntu0.18.04.1_i386.deb
  sudo dpkg -i libmariadb.deb
  sudo apt --fix-broken install
  sudo dpkg -i libmariadbclient.deb
  sudo apt --fix-broken install
fi

if [ -d "$HOME/vcpkg" ];
then
  echo "vcpkg already exists"
else
  echo "Collecting vcpkg"
  cd $HOME/
  git clone https://github.com/Microsoft/vcpkg
fi

if [ -f "$HOME/vcpkg/vcpkg" ];
then
  echo "vcpkg is already built"
else
  echo "Building vcpkg"
  cd $HOME/vcpkg
  ./bootstrap-vcpkg.sh
  ./vcpkg integrate install
fi

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

cd $HOME/vcpkg/
./vcpkg install libmariadb:x86-linux
cd $HOME/BSQL
cmake -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake $HOME/BSQL
make
