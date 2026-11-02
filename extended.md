# Extended notes

Reference material for the [README](README.md).

## Contents

- [Dependencies](#dependencies)
- [Components](#components)
- [Landmines — read before filing a bug](#landmines--read-before-filing-a-bug)
- [Fine print](#fine-print)
- [Credits](#credits)

## Dependencies

Upstream names; your distro may spell them differently. `init.sh` audits most of this list and names whatever is missing. It does not check the X server, `dbus`, `wireplumber`, `xrandr`, `qt5ct` or `wmname`. Installing is system-specific.

| Group | Packages |
|-------|----------|
| **X11** | X server · `xinit` · `dbus` |
| **Trademarks** | `cwm` · `sxhkd` · `xdotool` |
| **Base tools** | `sh` · `bash` · `zsh` (or edit `run_non_interactive` for bash, landmine 9) · `grep` · `sed` · `awk` · `curl` · `ip` · `killall` · `pgrep` · `wall` |
| **Add-ons: shell** | rofi · dunst · polybar · picom |
| **Add-ons: audio & media** | pipewire (+ wireplumber) · `wpctl` · playerctl |
| **Add-ons: system** | brightnessctl · xterm · xsel · xrandr · libnotify (`notify-send`) · qt5ct · `upower` · `loginctl` (systemd or elogind) |
| **Add-ons: screen & lock** | ffmpeg · ImageMagick (`magick` or `convert`) · `i3lock` · `xss-lock` · feh (fallback wallpaper) |
| **Add-ons: clipboard** | greenclip (manual install) |
| **Organ-building** | `gcc` · `make` · `pkg-config` · development headers for X11, OpenGL, `glfw3`, `glew`, `portaudio-2.0`, `espeak-ng` and `libnotify` |
| **QR eye** | python3 + OpenCV (`cv2`); `init.sh` installs `opencv-python` with pip into `./fde-venv` |
| **Optional** | keepassxc (`W-x p`) · mpv (`W-s`) · wmname ([landmine 2](#landmines--read-before-filing-a-bug)). Skipped = its keybind has nothing to summon. |

- **xterm** is required: the one-command terminal (`WC-r`) runs in it regardless of your daily terminal.
- **greenclip** is rarely packaged: see [Fine print](#fine-print).

## Components

| Tier | Item | Role |
|------|------|------|
| **Trademark** | cwm | Window policy: groups, stacking, title search |
| | sxhkd | Every keybinding cwm doesn't own |
| | xdotool | The hands; scripts reach windows through it |
| **Add-on** | rofi | Launcher + clipboard front-end |
| | dunst | Notifications |
| | polybar | Bar |
| | picom | Compositor |
| | pipewire (`wpctl`, `playerctl`) | Audio |
| | brightnessctl | Brightness |
| | greenclip | Clipboard history |
| **Organ** | `custom/layout` | Every window-management verb behind `W-x`, `W-f`, `W-s` ([Layouts](README.md#layouts)) |
| | `check_battery.c` | C daemon: warns on low **and** high battery, firmly enough to keep it healthy |
| | `blink.c` | Blurs the screen every 5 minutes (tunable) as an eye break; easily disabled |
| | `chime.c` | Imitation of macOS's time readout |
| | `live-wallpaper.c` | C + GLX shader on the root window; replace with `feh` if needed |

## Landmines — read before filing a bug

| # | Area | Problem | Fix / status |
|---|------|---------|--------------|
| 1 | **Tray-dwellers** | Discord, Steam and Telegram default to close-to-tray. No tray exists, so a withdrawn window is a running process with no reachable UI. | Set close-to-quit in each |
| 2 | **Java GUIs** | Some Java GUIs may draw blank or misplaced windows. | Set `_JAVA_AWT_WM_NONREPARENTING=1` (see [Install](README.md#install)); stray blank window: `wmname LG3D` |
| 3 | **Clipboard** | greenclip history is text-only. Images copy-paste app-to-app while the source window lives, but never persist in history. | Known, accepted |
| 4 | **EWMH** | Partial. The odd `wmctrl` verb no-ops; some games fullscreen poorly. | Unverified; no known-good verb list yet |
| 5 | **pipewire** | With `pipewire` + `wireplumber` installed, audio should just work. | Otherwise hardware quirks are yours to resolve |
| 6 | **Battery daemon** | `check_battery.c` assumes the battery is `BAT0`. | Change the name in the source to match your system (`BAT1`, `ADP1` or `ACAD`) and rebuild |
| 7 | **Layout script** | `custom/layout` is tuned for a 1080x1920 display. | An auto-resolution mode exists; see the script |
| 8 | **Weather in polybar** | No location is set by default. | Set the `location` variable in `custom/wttr` |
| 9 | **Shell** | `run_non_interactive` assumes `zsh`. | Replace `zsh` with `bash` in it if you prefer bash |
| 10 | **Terminal & browser** | Both must be set by hand. | Edit `cwmrc`, `sxhkdrc` and `custom/layout`; find the lines with `grep -rn "#Terminal & Browser"` |

## Fine print

| Topic | Detail |
|-------|--------|
| **greenclip** | Static Haskell binary, rarely packaged. Install manually: drop the release from `erebe/greenclip` (GitHub) into `~/.local/bin`. It runs as you and records what you copy, passwords included. Use the official release page, verify any checksum or signature offered, and see `greenclip.toml` for what is excluded. |
| **Organ headers** | C organs compile against X11 and OpenGL. Header package names vary by distro; a failed compile from `init.sh` names the missing piece. |
| **GPU** | The stock modesetting driver is usually enough. If tearing or performance disappoints, install your distro's driver for your card. |
| **Lock blur** | `ffmpeg` captures the screen; ImageMagick blurs it. |
| **Theme & fonts** | Set in the shipped configs (GTK theme, icon theme, cursor, fonts); exact names not yet listed. |

## Credits

Thanks to the creators and hosts of the media assets that `init.sh` downloads. Apart from the Windows sounds, the assets are shared under Creative Commons licences; see each source for its terms.

The Windows XP start-up and shutdown sounds belong to Microsoft and are not Creative Commons. This is a non-commercial project; please use them for personal, non-commercial purposes only.

| Asset | Source |
|-------|--------|
| Windows XP start-up and shutdown sounds (Microsoft) | [winhistory.de](https://winhistory.de/) |
| Lock sound | [QuickSounds](https://quicksounds.com/) |
| Dial-up sound | [Free Sounds Library](https://www.freesoundslibrary.com/) |
| Terminal sound | [Pixabay](https://pixabay.com/) |
| Xfce wallpaper | [Xfce artwork](https://gitlab.xfce.org/artwork/public) |
| Wallpaper | [dwt1/wallpapers](https://gitlab.com/dwt1/wallpapers) |
| Tux | [Wikimedia Commons](https://commons.wikimedia.org/) |
| `dim-screen.sh` | [xss-lock](https://bitbucket.org/raymonad/xss-lock) |
