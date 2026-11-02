#!/usr/bin/sh

export FDE="PLACEHOLDER"
export BUILDS="$FDE/builds"
export XINITRC="$FDE/xinitrc"
export ASSETS="$FDE/Assets"

startx "$XINITRC"
