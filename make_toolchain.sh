#!/usr/bin/env bash

COLOR_ERROR='\e[1;31m'
COLOR_STEP='\e[1;32m'
COLOR_NORMAL='\e[0m'

error() {
    echo -e "${COLOR_ERROR}[E]:${COLOR_NORMAL}" $@
}

step() {
    echo -e "${COLOR_STEP}[v]:${COLOR_NORMAL}" $@
}

# main

set -e

cd `dirname $0`

if [ -e toolchain ]; then
    error "cannot make 'toolchain' directory: already exists"
    exit 1
fi

mkdir -p toolchain/build
DESTDIR="`realpath toolchain`"

pushd toolchain/build

# make LCC
step "cloning lcc..."
git clone https://github.com/0xNULLderef/lcc1802
mkdir lcc1802_build
step "building lcc..."
make -C lcc1802/lcc42 BUILDDIR="`realpath lcc1802_build`" all -j $(nproc)
step "installing lcc..."
make -C lcc1802/lcc42 BUILDDIR="`realpath lcc1802_build`" DESTDIR="${DESTDIR}" install
step "copying includes..."
cp -r lcc1802/include/* ${DESTDIR}/share/lcc1802/include

step "downloading asl..."
wget http://john.ccac.rwth-aachen.de:8000/ftp/as/source/c_version/asl-current.tar.gz
step "extracting asl..."
tar -xzf asl-current.tar.gz

step "patching makefile..."
sed -i 's/all: binaries docs/all: binaries/' asl-current/Makefile

step "dropping Makefile.def"
cat << EOF > asl-current/Makefile.def
# -------------------------------------------------------------------------
# choose your compiler (must be ANSI-compliant!) and linker command, plus
# any additionally needed flags

OBJDIR =
CC = gcc
CFLAGS = -g -O3 -march=athlon64 -fomit-frame-pointer -Wall
HOST_OBJEXTENSION = .o
LD = \$(CC)
LDFLAGS =
HOST_EXEXTENSION =

# no cross build

TARG_OBJDIR = \$(OBJDIR)
TARG_CC = \$(CC)
TARG_CFLAGS = \$(CFLAGS)
TARG_OBJEXTENSION = \$(HOST_OBJEXTENSION)
TARG_LD = \$(LD)
TARG_LDFLAGS = \$(LDFLAGS)
TARG_EXEXTENSION = \$(HOST_EXEXTENSION)

# -------------------------------------------------------------------------
# directories where binaries, includes, and manpages should go during
# installation

BINDIR = bin
INCDIR = share/asl/include
MANDIR = share/man
LIBDIR =
DOCDIR = share/doc
EOF

step "building asl..."
make -C asl-current -j $(nproc)

step "installing asl..."
make -C asl-current INSTROOT="${DESTDIR}" install

popd
rm -rf toolchain/build
