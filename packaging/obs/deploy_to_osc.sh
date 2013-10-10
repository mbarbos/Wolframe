#!/bin/sh

VERSION=0.0.1
OSC_HOME=$HOME/home:andreas_baumann/Wolframe

# the original package
rm -f wolframe-$VERSION.tar.gz
make \
	WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 WITH_LIBXSLT=1 \
	dist-gz
cp wolframe-$VERSION.tar.gz $OSC_HOME/wolframe_$VERSION.tar.gz

# and a nice debian version
cp wolframe-$VERSION.tar.gz $OSC_HOME/wolframe_$VERSION.orig.tar.gz

# the Redhat build script
cp packaging/redhat/wolframe.spec $OSC_HOME/wolframe.spec

# patches
cp packaging/obs/boost1.48/boost_1_48_0-gcc-compile.patch $OSC_HOME/.

# compute sizes of packages
SIZE=`stat -c '%s' $OSC_HOME/wolframe_$VERSION.orig.tar.gz`
CHKSUM=`md5sum $OSC_HOME/wolframe_$VERSION.orig.tar.gz | cut -f 1 -d' '`

# copy all Debian versions of the description files.
cp packaging/obs/wolframe*.dsc $OSC_HOME
for i in `ls $OSC_HOME/wolframe-*.dsc`; do
	echo " $CHKSUM $SIZE wolframe_$VERSION.orig.tar.gz" >> $i
	OS_ORIG=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//'`
	OS=`echo $i | cut -f 2 -d '-' | sed 's/\.dsc$//' | tr -d '_'`
	rm -rf $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz
	rm -rf /tmp/debian
	cp -a packaging/debian /tmp/.
	test -f packaging/obs/control-$OS_ORIG && cp -a packaging/obs/control-$OS_ORIG /tmp/debian/control
	test -f packaging/obs/rules-$OS_ORIG && cp -a packaging/obs/rules-$OS_ORIG /tmp/debian/rules
	OLDDIR=$PWD
	cd /tmp
	tar zcf /tmp/wolframe_$VERSION-$OS.debian.tar.gz debian
	cd $OLDDIR
	mv -f /tmp/wolframe_$VERSION-$OS.debian.tar.gz $OSC_HOME/.
	DEBIAN_SIZE=`stat -c '%s' $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz`
	DEBIAN_CHKSUM=`md5sum  $OSC_HOME/wolframe_$VERSION-$OS.debian.tar.gz | cut -f 1 -d' '`
	echo " $DEBIAN_CHKSUM $DEBIAN_SIZE wolframe_$VERSION-$OS.debian.tar.gz" >> $i
done

# Archlinux specific files
cat packaging/obs/PKGBUILD > $OSC_HOME/PKGBUILD
cp packaging/archlinux/wolframe.conf $OSC_HOME/wolframe.conf
cp packaging/archlinux/wolframed.service $OSC_HOME/wolframed.service
cp packaging/archlinux/wolframe.install $OSC_HOME/wolframe.install

CHKSUM2=`md5sum $OSC_HOME/wolframe.conf | cut -f 1 -d' '`
CHKSUM3=`md5sum $OSC_HOME/wolframed.service | cut -f 1 -d' '`

echo "md5sums=('$CHKSUM' '$CHKSUM2' '$CHKSUM3')" >> $OSC_HOME/PKGBUILD

# the revision of the git branch we are currently building (master hash at the moment)
git rev-parse HEAD > $OSC_HOME/GIT_VERSION