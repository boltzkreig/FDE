#!/bin/sh
# set -e

echo "This script only check which scripts are installed. It will show the binary or version is properly installed."
echo "Then it will install python dependencies [CHAPTER 4], build the C code [CHAPTER 2] & download assets [CHAPTER 6]"
read -p "Press 'Enter' to continue..." reply

audit_the_following(){
    for i in "$@"
    do
        printf "%-15s : " "$i"
        command -v "$i" || printf '[31mMISSING[0m\n'
        # which $i
    done
    read -p "Press 'Enter' to continue..." reply
}

download_asset(){
    # printf "%-10s" $2
    # curl -fL# $1 -o $2 -w '%{filename_effective}  http:%{response_code}  %{size_download}B  %{time_total}s\n'
    curl -fLs --retry 3 -w "http:%{response_code}  %{time_total}s  %{size_download}B  %{filename_effective}\n" \
        -o winxp.mp3          "https://winhistory.de/more/winstart/mp3/winxp.mp3" \
        -o winxpshutdown.mp3  "https://winhistory.de/more/winstart/mp3/winxpshutdown.mp3" \
        -o lock.mp3           "https://quicksounds.com/uploads/tracks/605323671_352749582_826341636.mp3" \
        -o dailup.mp3         "https://www.freesoundslibrary.com/wp-content/uploads/2021/03/dial-up-internet-sound.mp3" \
        -o xfce-shapes.svg    "https://gitlab.xfce.org/artwork/public/-/raw/master/wallpapers/xfce-4.18-final.svg" \
        -o terminal.mp3       "https://cdn.pixabay.com/download/audio/2026/03/26/audio_30de868d1f.mp3" \
        -o tux.png            "https://upload.wikimedia.org/wikipedia/commons/archive/a/af/20050808195403%21Tux.png" \
        -o 0253.jpg           "https://gitlab.com/dwt1/wallpapers/-/raw/master/0253.jpg" \
        -o dim-screen.sh      "https://bitbucket.org/raymonad/xss-lock/raw/1e158fb20108058dbd62bd51d8e8c003c0a48717/doc/dim-screen.sh"
    # 2> /dev/null | column -t
}

# [assumed base, unaudited]
echo "[34m\nCHAPTER 0: Coreutils check[0m"
audit_the_following "sh" "bash" "grep" "sed" "awk" "xinit" "curl"

# [Core base]
echo "[34m\nCHAPTER 1: Basic Software Kit[0m"
audit_the_following "picom" "pipewire" "dunst" "cwm" "sxhkd" "polybar" "greenclip"
audit_the_following "wpctl" "playerctl" "brightnessctl" "xss-lock"
echo "\nFollowing are fallback / optional"
audit_the_following "feh" "keepassxc" "upower"


# [C Components]
echo "[34m\nCHAPTER 2: System Specific Programmes ( written in C )[0m"
echo "\nProg build system"
audit_the_following "gcc" "make" "pkg-config" "python3"
# [libraries — headers to build, .so to run]
echo "\n C prog library dependencies"
for i in "glfw3" "libnotify" "portaudio-2.0" "espeak-ng" "x11" "gl" "glew"
do
    printf "%-15s : " "$i"
    pkg-config --modversion  --short-errors $i || printf '[31mMISSING[0m\n'

done
read -p "The C programmes will be built. Type '[31myes[0m' exactly to continue..." reply
if [ "$reply" = "yes" ]; then

    if ( cd custom && mkdir -p builds && make && cd ..); then
        rm -rf builds
        mv custom/builds .
    else
        printf '[31mFAILED[0m\n'
    fi
fi

# [runtime binaries]
echo "[34m\nCHAPTER 3: Runtime Binaries[0m"
audit_the_following "xdotool" "i3lock" "upower" "loginctl" "ffmpeg" "magick" "mpv" "rofi" "xterm" "zsh" "xsel" "curl" "ip" "killall" "pgrep" "notify-send" "wall"

# [python]
echo "[34m\nCHAPTER 4: Python Script Dependencies[0m"
read -p "The Python dependencies will be installed. Type '[31myes[0m' exactly to continue..." reply
if [ "$reply" = "yes" ]; then
    python3 -m venv fde-venv && \
        ./fde-venv/bin/pip3 install opencv-python
fi

# [Assets]
echo "[34m\nCHAPTER 5: Media Assets Downloading[0m"
read -p "Audio & Image Assets will be downloaded. Type '[31myes[0m' exactly to continue..." reply
if [ "$reply" = "yes" ]; then
    mkdir -p Assets && (
        cd Assets || exit 1
        download_asset

        resolution=$( (xdotool getdisplaygeometry 2> /dev/null || echo "1920 1080" ) | tr " " "x")
        if [ $(convert -version 2>&1 | grep -c convert) -ne 0 ]; then
            magick_convert="magick"
        else
            magick_convert="convert"
        fi
        $magick_convert 0253.jpg -resize "$resolution>" 0253.png
        $magick_convert xfce-shapes.svg -resize "$resolution>" xfce-shapes.png
        ffmpeg -y -v 24 -i dailup.mp3 -ss 7.5 -to 11.85 browser.mp3
        chmod +x dim-screen.sh && mv dim-screen.sh ../custom/
    )
fi

cat << 'EOF'

[34mCHAPTER 6: Extra warnings and special notes[0m
These are needed but won't break the system if unavailable.
Please try to install if possible.
# [fonts & assets]
terminus (X bitmap font) -> Distro specific help pages would help
icon font (FontAwesome)  -> check with `fc-list | grep -i 'NerdFont'`
adwaita-icon-theme       -> Generally ships with any GTK app

Add the following details too:
- [35m'location'[0m variable in 'custom/wttr' for weather in polybar
- The 'layout' script is tuned for [35m1080x1920[0m display. There is auto-resolution use if needed.
- The 'check_battery.c' assumes [35m'BAT0'[0m, it may be these for your system: BAT1 / ADP1 / ACAD
- The 'run_non_interactive' assumes [35m'zsh'[0m, replace with bash if you prefer bash
- Get a [35mterminal & browser[0m which needs to be updated in 'cwmrc', 'sxhkdrc' & 'custom/layout'. 
  Run `grep -rn "#Terminal & Browser"` to identify the lines.
- The 'greenclip' & 'rofi' need the config to be placed in [35m'$XDG_CONFIG'[0m to be effective

EOF
