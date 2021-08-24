VERSION=4.3.2.5
rm -rf ./release-linux
mkdir release-linux

cp ./src/hived ./release-linux/
cp ./src/hive-cli ./release-linux/
cp ./src/qt/hive-qt ./release-linux/
cp ./Hivecoin_small.png ./release-linux/

cd ./release-linux/
strip hived
strip hive-cli
strip hive-qt

#==========================================================
# prepare for packaging deb file.

mkdir hivecoin-$VERSION
cd hivecoin-$VERSION
mkdir -p DEBIAN
echo 'Package: hivecoin
Version: '$VERSION'
Section: base 
Priority: optional 
Architecture: all 
Depends:
Maintainer: HiveProject2021
Description: Hive coin wallet and service.
' > ./DEBIAN/control
mkdir -p ./usr/local/bin/
cp ../hived ./usr/local/bin/
cp ../hive-cli ./usr/local/bin/
cp ../hive-qt ./usr/local/bin/

# prepare for desktop shortcut
mkdir -p ./usr/share/icons/
cp ../Hivecoin_small.png ./usr/share/icons/
mkdir -p ./usr/share/applications/
echo '
#!/usr/bin/env xdg-open

[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/usr/local/bin/hive-qt
Name=hivecoin
Comment= hive coin wallet
Icon=/usr/share/icons/Hivecoin_small.png
' > ./usr/share/applications/hivecoin.desktop

cd ../
# build deb file.
dpkg-deb --build hivecoin-$VERSION

#==========================================================
# build rpm package
rm -rf ~/rpmbuild/
mkdir -p ~/rpmbuild/{RPMS,SRPMS,BUILD,SOURCES,SPECS,tmp}

cat <<EOF >~/.rpmmacros
%_topdir   %(echo $HOME)/rpmbuild
%_tmppath  %{_topdir}/tmp
EOF

#prepare for build rpm package.
rm -rf hivecoin-$VERSION
mkdir hivecoin-$VERSION
cd hivecoin-$VERSION

mkdir -p ./usr/bin/
cp ../hived ./usr/bin/
cp ../hive-cli ./usr/bin/
cp ../hive-qt ./usr/bin/

# prepare for desktop shortcut
mkdir -p ./usr/share/icons/
cp ../Hivecoin_small.png ./usr/share/icons/
mkdir -p ./usr/share/applications/
echo '
[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/usr/bin/hive-qt
Name=hivecoin
Comment= hive coin wallet
Icon=/usr/share/icons/Hivecoin_small.png
' > ./usr/share/applications/hivecoin.desktop
cd ../

# make tar ball to source folder.
tar -zcvf hivecoin-$VERSION.tar.gz ./hivecoin-$VERSION
cp hivecoin-$VERSION.tar.gz ~/rpmbuild/SOURCES/

# build rpm package.
cd ~/rpmbuild

cat <<EOF > SPECS/hivecoin.spec
# Don't try fancy stuff like debuginfo, which is useless on binary-only
# packages. Don't strip binary too
# Be sure buildpolicy set to do nothing

Summary: Hivecoin wallet rpm package
Name: hivecoin
Version: $VERSION
Release: 1
License: MIT
SOURCE0 : %{name}-%{version}.tar.gz
URL: https://www.hivecoin.org/

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
%{summary}

%prep
%setup -q

%build
# Empty section.

%install
rm -rf %{buildroot}
mkdir -p  %{buildroot}

# in builddir
cp -a * %{buildroot}


%clean
rm -rf %{buildroot}


%files
/usr/share/applications/hivecoin.desktop
/usr/share/icons/Hivecoin_small.png
%defattr(-,root,root,-)
%{_bindir}/*

%changelog
* Tue Aug 24 2021  Hive Project Team.
- First Build

EOF

rpmbuild -ba SPECS/hivecoin.spec



