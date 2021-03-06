#!/bin/sh

VERSION=0.0.3
PKGBUILD=$HOME/solarisbuild
ORIG_ARCH=`uname -m`

check_for_errors( )
{
	RET=$?
	if test $RET -gt 0; then
		echo "Build failed."
		exit 1
	fi
}

rm -rf $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION

mkdir -p $PKGBUILD/BUILD/wolframe-$VERSION $PKGBUILD/PKG/wolframe-$VERSION $PKGBUILD/PKGS/$ARCH

rm -f wolframe-$VERSION.tar.Z
rm -f $PKGBUILD/BUILD/wolframe_$VERSION.tar.Z

GIT_COMMIT_COUNT=`git describe --long --tags | cut -f 2 -d -`
gmake BOOST_DIR=/opt/csw/boost-1.55.0 distclean
rm -rf /tmp/wolframe-$VERSION
mkdir /tmp/wolframe-$VERSION
cp -R * /tmp/wolframe-$VERSION
OLDPWD=$PWD
cd /tmp
sed "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD $GIT_COMMIT_COUNT/g" wolframe-$VERSION/include/wolframe.hpp > _tmp
mv -f _tmp wolframe-$VERSION/include/wolframe.hpp
tar cf - wolframe-$VERSION | compress -c > wolframe-$VERSION.tar.Z
cd $OLDPWD
mv /tmp/wolframe-$VERSION.tar.Z .
rm -rf /tmp/wolframe-$VERSION

cp -f wolframe-$VERSION.tar.Z $PKGBUILD/BUILD/.
cd $PKGBUILD/BUILD
uncompress -c wolframe-$VERSION.tar.Z | tar xvf -
cd wolframe-$VERSION

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man \
	help

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man \
	config
	
LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man \
	config

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man
check_for_errors

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man \
	testreport
check_for_errors

LD_RUN_PATH=/opt/csw/lib:/opt/csw/postgresql/lib \
	LDFLAGS='-Wl,-R/opt/csw/lib -Wl,-R/opt/csw/lib/wolframe' \
	OPENSSL_DIR=/opt/csw PGSQL_DIR=/opt/csw/postgresql LIBLT_DIR=/usr \
	BOOST_DIR=/opt/csw/boost-1.55.0 \
	ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_EXPECT=1 WITH_SSL=1 WITH_LOCAL_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 \
	WITH_LIBXML2=1 WITH_LIBXSLT=1 WITH_PAM=1 WITH_SASL=1 \
	WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 ICU_DIR=/opt/csw/icu4c-49.1.2 \
	WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 \
	PYTHON_DIR=/opt/csw/python-3.3.2 WITH_CJSON=1 WITH_TEXTWOLF=1 \
	gmake CC='ccache gcc' CXX='ccache g++' CFLAGS='-mcpu=v9' CXXFLAGS='-mcpu=v9' \
	prefix=/opt/csw \
	sysconfdir=/etc/opt/csw libdir=/opt/csw/lib \
	libdir=/opt/csw/lib DEFAULT_MODULE_LOAD_DIR=/opt/csw/lib/wolframe/modules \
	DEFAULT_MAIN_CONFIGURATION_FILE=/etc/opt/csw/wolframe/wolframe.conf \
	mandir=/opt/csw/share/man \
	DESTDIR=$PKGBUILD/PKG/wolframe-$VERSION \
	install
check_for_errors

# doxygen package currently broken
#cd docs; gmake DESTDIR=$PKGBUILD/PKG doc-doxygen; cd ..

cp packaging/solaris/pkginfo $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/copyright $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/prototype $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/depend $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/preinstall $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/postinstall $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/preremove $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/postremove $PKGBUILD/PKG/wolframe-$VERSION/.
cp packaging/solaris/wolframed.init $PKGBUILD/PKG/wolframe-$VERSION/.

ARCH=`uname -m`
OSARCH=`uname -p`
OSVER=`uname -r`
cat <<EOF >> $PKGBUILD/PKG/wolframe-$VERSION/pkginfo
ARCH="$OSARCH"
EOF

mkdir -p $PKGBUILD/PKG/wolframe-$VERSION/etc/opt/csw/wolframe

BOOST_VERSION=1.55.0
for i in \
	libboost_chrono.so.$BOOST_VERSION \
	libboost_date_time.so.$BOOST_VERSION \
	libboost_filesystem.so.$BOOST_VERSION \
	libboost_locale.so.$BOOST_VERSION \
	libboost_program_options.so.$BOOST_VERSION \
	libboost_regex.so.$BOOST_VERSION \
	libboost_system.so.$BOOST_VERSION \
	libboost_thread.so.$BOOST_VERSION; do
	cp /opt/csw/boost-$BOOST_VERSION/lib/$i $PKGBUILD/PKG/wolframe-$VERSION/opt/csw/lib/wolframe/.
done

cp /opt/csw/python-3.3.2/lib/libpython3.3m.so.1.0 $PKGBUILD/PKG/wolframe-$VERSION/opt/csw/lib/wolframe/.

ICU_VERSION=49.1.2
for i in \
	libicudata.so.$ICU_VERSION \
	libicui18n.so.$ICU_VERSION \
	libicuio.so.$ICU_VERSION \
	libicule.so.$ICU_VERSION \
	libiculx.so.$ICU_VERSION \
	libicutu.so.$ICU_VERSION \
	libicuuc.so.$ICU_VERSION; do
	cp /opt/csw/icu4c-49.1.2/lib/$i $PKGBUILD/PKG/wolframe-$VERSION/opt/csw/lib/wolframe/.
	j=`echo $i | cut -f 1 -d .`
	ln -fs $i $PKGBUILD/PKG/wolframe-$VERSION/opt/csw/lib/wolframe/$j.so.49
done

cd $PKGBUILD/PKG/wolframe-$VERSION

pkgmk -o -r .
check_for_errors
pkgtrans -s /var/spool/pkg $PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$OSARCH-$OSVER.pkg WFRGwolfrm
check_for_errors

compress -f $PKGBUILD/PKGS/$ARCH/wolframe-$VERSION-$OSARCH-$OSVER.pkg

# rm -rf $PKGBUILD/BUILD/wolframe-$VERSION
# rm -rf $PKGBUILD/PKG/wolframe-$VERSION

echo "Build succeeded."
exit 0
