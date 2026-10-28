#!/bin/sh

#==========================================================
#
#
#   ███████╗████████╗ █████╗ ██████╗ ████████╗██╗  ██╗
#   ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝╚██╗██╔╝
#   ███████╗   ██║   ███████║██████╔╝   ██║    ╚███╔╝ 
#   ╚════██║   ██║   ██╔══██║██╔══██╗   ██║    ██╔██╗ 
#   ███████║   ██║   ██║  ██║██║  ██║   ██║   ██╔╝ ██╗
#   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝
#
#
#==========================================================

setxkbmap
[ -f "$FDE"/.Xmodmap ] && xmodmap "$FDE"/.Xmodmap
[ -f "$FDE"/.Xresources ] && xrdb -merge "$FDE"/.Xresources

pipewire &
dunst --config "$FDE"/dunstrc &
greenclip daemon &
sxhkd -c "$FDE"/sxhkdrc &

polybar --config="$FDE"/config.ini &

$BUILDS/check_battery &
$BUILDS/blink &
$BUILDS/chime &

mpv --no-config --really-quiet "$ASSETS"/winxp.mp3 &
picom --backend glx --config "$FDE"/picom.conf -b
# xset dpms 30 30 30
xset s 900 30
# xset s noblank
xss-lock -n "$FDE"/custom/dim-screen.sh --transfer-sleep-lock -- "$FDE"/custom/lock &

# feh --no-fehbg --bg-fill "$ASSETS"/0253.jpg
$BUILDS/live-wallpaper &

exec cwm -c "$FDE"/cwmrc
