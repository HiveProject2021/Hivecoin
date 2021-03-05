
Debian
====================
This directory contains files used to package hived/hive-qt
for Debian-based Linux systems. If you compile hived/hive-qt yourself, there are some useful files here.

## hive: URI support ##


hive-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install hive-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your hive-qt binary to `/usr/bin`
and the `../../share/pixmaps/hive128.png` to `/usr/share/pixmaps`

hive-qt.protocol (KDE)

