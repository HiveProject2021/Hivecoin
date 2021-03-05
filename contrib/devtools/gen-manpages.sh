#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

HIVED=${HIVED:-$SRCDIR/hived}
HIVECLI=${HIVECLI:-$SRCDIR/hive-cli}
HIVETX=${HIVETX:-$SRCDIR/hive-tx}
HIVEQT=${HIVEQT:-$SRCDIR/qt/hive-qt}

[ ! -x $HIVED ] && echo "$HIVED not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
HVNVER=($($HIVECLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for hived if --version-string is not set,
# but has different outcomes for hive-qt and hive-cli.
echo "[COPYRIGHT]" > footer.h2m
$HIVED --version | sed -n '1!p' >> footer.h2m

for cmd in $HIVED $HIVECLI $HIVETX $HIVEQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${HVNVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${HVNVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
