#!/usr/bin/sh

export FDE="$HOME/Projects/FDE"
export BUILDS="$FDE/builds"
export XINITRC="$FDE/xinitrc"
export ASSETS="$FDE/Assets"

startx "$XINITRC"
