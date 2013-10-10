#!/bin/sh

# Note: Create a ~/.rpmmacros to set number of CPUs for
# parallel building like this:
# %_smp_mflags -j24

# Of course you need build toold and rpm-build, also a ~/rpmbuild directory
# set up

# the Centos version is set below as 'centos_version 630' (OSB service
# linguo for Centos 6.3)

VERSION=0.0.1
RPMBUILD=$HOME/rpmbuild
#OSB_PLATFORM=

rm -rf $RPMBUILD/BUILDROOT $RPMBUILD/BUILD $RPMBUILD/RPMS/*/* \
	$RPMBUILD/SRPMS/* $RPMBUILD/SPECS/log \
	/tmp/boost-1.48.0 /tmp/libxml2-2.7.8 /tmp/Python-3.3.2
touch $RPMBUILD/SPECS/log

rm -f wolframe-$VERSION.tar.gz
rm -f $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz

make \
	WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	dist-gz >/dev/null 2>&1

cp wolframe-$VERSION.tar.gz $RPMBUILD/SOURCES/wolframe_$VERSION.tar.gz
cp packaging/redhat/wolframe.spec $RPMBUILD/SPECS/wolframe.spec

if test ! -f $RPMBUILD/SOURCES/boost_1_48_0.tar.gz; then
	wget -O $RPMBUILD/SOURCES/boost_1_48_0.tar.gz \
		http://downloads.sourceforge.net/project/boost/boost/1.48.0/boost_1_48_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.48.0%2F&ts=1353483626&use_mirror=ignum
fi
 
cp packaging/obs/boost1.48/boost_1_48_0-gcc-compile.patch $RPMBUILD/SOURCES/.

cd $RPMBUILD/SPECS

echo "Building started, check with 'tail -f $RPMBUILD/SPECS/log'."

export CC='ccache gcc'
export CXX='ccache g++'
rpmbuild -ba --define "$OSB_PLATFORM" wolframe.spec > log 2>&1

echo "Build done."
