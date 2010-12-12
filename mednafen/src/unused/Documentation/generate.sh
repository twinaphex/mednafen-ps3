#!/bin/bash

../src/mednafen -dump_settings_def settings.def

# TODO: Dump to settings.def.tmp, and move to settings.def if different for more useful "last modified" time.

php gba.php > gba.html
php gb.php > gb.html
php lynx.php > lynx.html
php md.php > md.html
php nes.php > nes.html
php ngp.php > ngp.html
php pce.php > pce.html
php pcfx.php > pcfx.html
php vb.php > vb.html
php wswan.php > wswan.html
