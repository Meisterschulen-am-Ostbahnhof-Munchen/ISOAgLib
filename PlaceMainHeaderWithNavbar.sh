#!/bin/sh
# -*-mode: shell-script-*-
exec sed "s|^INCLUDE_\(.*\)_INCLUDE$|\"$0\" \"\1\"|e" "$1"

# Dies Skript hier ist als Filter fuer Doxygen geschrieben.

# Urspruenglich hat es nur das hier gemacht (daher sein Name):

# /bin/sed 's/^INCLUDE_mainHeaderWithNavbar.txt_INCLUDE$/cat mainHeaderWithNavbar.txt/e' $1

# Es war also darauf beschraenkt, den Inhalt der Datei
# mainHeaderWithNavbar.txt einzufuegen.

# Jetzt kann es auch den Inhalt anderer Dateien einfuegen, und das
# sogar rekursiv (es laesst sich ggf. von sed wieder selbst aufrufen,
# siehe $0 oben).

# Es koennte leicht implementiert werden, dass der Pfad der
# eingefuegten Datei relativ zu dem der einfuegenden Datei angegeben
# werden kann (wie bei #include in C), aber dann ginge die
# Rueckwaertskompatibilitaet verloren.

#  Thomas Hafner, 2007-02-21.
