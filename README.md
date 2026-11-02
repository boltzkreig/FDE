# Faux Desktop Emulation

## Contents

- [What this is](#what-this-is)
  - [Groups](#groups)
- [Dependencies](extended.md#dependencies)
- [Install](#install)
  - [What the scripts touch](#what-the-scripts-touch)
- [Session anatomy](#session-anatomy)
- [Components](extended.md#components)
- [Keybindings](#keybindings)
  - [Placement](#placement)
  - [Group keys](#group-keys)
  - [Group theory — two habits to unlearn](#group-theory--two-habits-to-unlearn)
  - [Session & applications](#session--applications)
  - [Media keys](#media-keys)
  - [Mouse](#mouse)
  - [Layouts](#layouts)
- [Landmines — read before filing a bug](extended.md#landmines--read-before-filing-a-bug)
- [Fine print](extended.md#fine-print)
- [FAQ](#faq)
- [Credits](extended.md#credits)
- [Footnotes](#footnotes)

## What this is

**Not** a Desktop Environment. A window manager, a hotkey daemon and scripting glue, plus a few supervised add-ons and hand-written organs for quality of life.

| Stance | What it means |
|--------|---------------|
| **One screen** | One Screen to rule them all. External outputs mirror automatically. Presenting is the only multi-monitor problem worth having; mixed-DPI cannot occur. |
| **Dark mode only** | Come to the dark side: GTK, cursor, terminal, one pre-wired theme. Qt needs one export (see [Install](#install)). No toggle. |
| **Three trademarks** | It takes ~~two~~ three to tango: cwm · sxhkd · xdotool. Everything else is a replaceable add-on. |
| **You're a wizard, Harry** | You are the mechanic: `nmcli` for networking, `bluetoothctl` for bluetooth, a mount script for USB. [Helpful guide](https://www.youtube.com/watch?v=_VrFV5r8cs0). |
| **No tray, no wizards** | No file manager either (browsers ship a GUI one anyway). Free, in exchange for your time to tune it. |
| **Paved, not silent** | Reading configs is the price. Supervision, a config gate and the [landmines](extended.md#landmines--read-before-filing-a-bug) keep breakage loud. |
| **X11/XLibre, no apologies** | Hard to break, runs almost everywhere, plus a secondary clipboard. |
| **Groups beat workspaces** | cwm = ❤️. See below. |

### Groups

Windows are resizable papers on a desk: they move out of sight instantly and return instantly. Workspaces are **rooms** (you travel to your windows). cwm groups are **summons** (windows come to you).

Every window lives in exactly one group, 1–9. What you see is the **union** of the groups currently shown.

| Keybind | Action |
|---------|--------|
| `W-#` | Show *only* group #: classic workspace behaviour |
| `WC-#` | *Toggle* group # in or out of view: **the point** |
| `WS-#` | Send focused window to group # |
| `W-a` | Show everything |

Overlay chat on code, glance, toggle it away: no mainstream desktop can express this. Use only `W-#` and you have ordinary workspaces; the bigger model costs you nothing. Two habits to unlearn: see [Group theory](#group-theory--two-habits-to-unlearn).

## Dependencies

Packages needed for the session, the C organs and the optional extras are listed in [Dependencies](extended.md#dependencies).

## Install

```sh
git clone https://github.com/boltzkreig/FDE && cd FDE
./init.sh     # audits dependencies and names what's missing; then, after a
              # typed "yes" for each step, builds the C organs, creates a
              # Python venv and downloads media assets — all inside the repo
./FDE.sh      # plug and play* — starts X with the repo's own xinitrc
```

> [!NOTE]
> *Plug and play applies once setup is done. `init.sh` pauses for a typed `yes` at each stage, the config files are not placed for you (greenclip and rofi need theirs in your XDG config directory, usually `~/.config`), and your terminal and browser must be set by hand (see [Landmines](extended.md#landmines--read-before-filing-a-bug)).

Anything running X11 carries it. If an app misbehaves, these two exports are the usual fixes. They are not set by default; add them to your own startup file if you need them:

```sh
export _JAVA_AWT_WM_NONREPARENTING=1   # Java GUIs (IntelliJ & friends) draw correctly
export QT_QPA_PLATFORMTHEME=qt5ct      # Qt apps follow the qt5ct theme
```

### What the scripts touch

What the current `init.sh` does:

| Step | Asks first | Writes to | Network |
|------|------------|-----------|---------|
| Dependency audit | Enter | nothing | no |
| Build C organs | typed `yes` | `custom/builds`, then `./builds` (an existing `./builds` is deleted) | no |
| Python venv + OpenCV | typed `yes` | `./fde-venv` | PyPI (`opencv-python`) |
| Media assets | typed `yes` | `./Assets`; `dim-screen.sh` is moved into `custom/` and made executable | third-party hosts (see below) |

- **No root.** `init.sh` never calls `sudo` and writes nothing outside the repo directory.
- **No backups, no config wiring.** It installs no system packages and does not copy configs into place. You place them yourself (for example, greenclip and rofi need theirs in your XDG config directory, usually `~/.config`). `FDE.sh` starts X with the repo's `xinitrc` and does not replace `~/.xinitrc`.
- **Downloads.** Sounds, wallpapers and images are fetched from several third-party sites with no checksum verification, and are subject to their own terms (see [Credits](extended.md#credits)). `dim-screen.sh` is a shell script fetched from a commit-pinned URL.

## Session anatomy

tty login → `FDE.sh` → `startx` → `xinitrc` → *DONE*. `FDE.sh` only sets a few paths and starts X with the repo's `xinitrc`. `xinitrc` assembles the session, keeps it standing and restarts what dies.

| Component | Job | Notes |
|-----------|-----|-------|
| cwm | Window policy | Bare bones; script around it freely |
| sxhkd | Every hotkey | Write any shortcut you wish |
| picom | Vsync, shadows | Animate it and polish it |
| dunst | Notifications | Basic, configured to be nice |
| polybar | Clock, vitals | Heads-up display for system stats |
| greenclip | Clipboard history | Simple plain-text management |
| live-wallpaper | Root-window shader | Tuned for least GPU load |
| pipewire | Audio | Hardware has quirks; fixing them is on you |

## Components

The tools, add-ons and C organs that make up the session are listed in [Components](extended.md#components).

## Keybindings

**Verbs**

| Verb | Meaning |
|------|---------|
| **Align** | Put the window in a region: position *and* size |
| **Move** | Position only |
| **Resize** | Size only |
| **Snap** | Slide to an edge; size untouched |
| **Rogue keys** | `hjkl` + diagonals `y u b n`. Numpad grid for desks that have one; rogue keys for laptops that don't |

The *Source* column names the file that owns each behaviour. Edit it, then reload: `W-Escape` (sxhkd) or `CAS-r` (cwm).

| Super (Window) | Shift | Control | Meta (Alt) |
|----------------|-------|---------|------------|
| W | S | C | A |

### Placement

| Binding | Action | Source |
|---------|--------|--------|
| `W-NumberPad` | Align window in region [[1a]](#fn-1a) | sxhkd |
| `WC-Rogue_keys` | Align window in region [[1b]](#fn-1b) | sxhkd |
| `W-Rogue_keys` | Move window (vi-style) [[1b]](#fn-1b); `m` excluded, see below | sxhkd |
| `WS-Rogue_keys` | Resize window (vi-style) [[1b]](#fn-1b); `m` excluded, see below | sxhkd |
| `W-Arrow_keys` | Snap window to edge [[1c]](#fn-1c) | cwmrc |
| `WS-Arrow_keys` | Snap window, 45° CW [[1c]](#fn-1c) | cwmrc |

### Group keys

| Binding | Action | Source |
|---------|--------|--------|
| `W-Group_ID` | Show only that group | cwmrc |
| `WC-Group_ID` | Toggle that group in/out of view | cwmrc |
| `WS-Group_ID` | Send window to that group | cwmrc |
| `W-a` | Show all groups | cwmrc |
| ``W-` `` | Rotate within the group | cwmrc |
| ``WS-` `` | Rotate within the group, reverse | cwmrc |

### Group theory — two habits to unlearn

| Habit | Reality |
|-------|---------|
| **Focus raises** | **Focus does not raise.** A focused window keeps its stack place until you lower what covers it (`A-Right_Click`) or raise it (`A-Left_Click`). "Always on top" is the resting state of whatever is on top; stacking is acknowledged, not fought. |
| **Taskbar = control** | **Hidden means summoned-away, not lost.** No taskbar to scan; use the desktop menus: Left_Click lists windows, Middle_Click lists groups (see [Mouse](#mouse)). Pick a window to summon it from any group. |

### Session & applications

| Binding | Action | Source |
|---------|--------|--------|
| `W-r` | Applications (rofi, drun) | sxhkd |
| `WS-r` | Rofi combi-mode | sxhkd |
| `W-v` | Clipboard history (greenclip) | sxhkd |
| `A-c` | Swap primary/secondary clipboard | sxhkd |
| `W-t` | Terminal [[2a]](#fn-2a) | sxhkd |
| `WC-r` | One-command terminal [[2b]](#fn-2b) | sxhkd |
| `W-b` | Browser [[2c]](#fn-2c) | sxhkd |
| `A-q` | Kill focused window; unsaved work in it can be lost | sxhkd |
| `AS-c` | Read QR with camera [[2d]](#fn-2d) | sxhkd |
| `Print` | Screenshot [[2e]](#fn-2e) | sxhkd |
| `CA-q` | Lock screen [[2f]](#fn-2f) | sxhkd |
| `WS-q` | Hibernate [[2g]](#fn-2g) | sxhkd |
| `WS-e` | Kill X11 — instant; closes every open app | sxhkd |
| `W-Escape` | Reload sxhkd | sxhkd |
| `CAS-r` | Reload cwm config | cwmrc |

> [!WARNING]
> `WS-e` ends the whole X session immediately and `A-q` kills the focused window. Neither is guaranteed to give an application time to save.

### Media keys

| Binding | Action | Source |
|---------|--------|--------|
| `XF86AudioRaiseVolume` | Increase volume | sxhkd |
| `XF86AudioLowerVolume` | Decrease volume | sxhkd |
| `XF86AudioMute` | Mute | sxhkd |
| `XF86AudioPlay` | Play/pause media | sxhkd |
| `XF86MonBrightnessUp` | Increase brightness | sxhkd |
| `XF86MonBrightnessDown` | Decrease brightness | sxhkd |

### Mouse

| Binding | Action | Source |
|---------|--------|--------|
| Left_Click (desktop) | Window list | cwmrc |
| Middle_Click (desktop) | Group list | cwmrc |
| Right_Click (desktop) | Favourite applications | cwmrc |
| A-Left_Click | Raise window | cwmrc |
| A-Left_Drag | Move window | cwmrc |
| A-Middle_Drag | Resize window | cwmrc |
| A-Right_Click | Lower window (see Group theory) | cwmrc |
| W-Left_Click | Toggle window's group membership | cwmrc |

### Layouts

Three engines, one philosophy: floating-first, tiling as a **moment**, not a requirement.

| Key | Engine | Behaviour |
|-----|--------|-----------|
| `W-m` | cwm master–slave | Tiles visible windows of the focused group only (coherent under unions). Ignores gap and bar tolerances: it will tile under polybar. |
| `W-f` | Smart Hover | Hover the main window; others move aside |
| `W-s` | Smart Toggle | Browser ↔ terminal, MPV to the side |
| `W-x` + letter | `custom/layout` verb chord | See below |

**`W-x` chord**

| Key | Action | Description |
|-----|--------|-------------|
| a | Auto-tile | All visible windows into a grid (max 9) |
| b | Browser | Browser with a narrow terminal on top |
| f | Focus | Stage-Manager-like layout with window selection |
| h | Half | Terminal left; browser right |
| l | Large | Maximize |
| p | Password | KeePassXC to the side |
| s | Stack | Cascade visible windows into one pile (max 9) |
| x | Help | Cheatsheet, generated from these very configs |

## Landmines — read before filing a bug

Known problems and their fixes are in [Landmines](extended.md#landmines--read-before-filing-a-bug).

## Fine print

Details on greenclip, headers, GPU, lock blur, theme and fonts are in [Fine print](extended.md#fine-print).

## FAQ

| Question | Answer |
|----------|--------|
| **Why "Faux Desktop Emulation"?** | By a DE's checklist it is faux: no settings app, tray or wizards. It emulates the experience (bar, notifications, launcher, polish), staged by one script (`FDE.sh`) from text files you can read in an afternoon. |
| **Isn't X11 dead?** | [Reports of its death are greatly exaggerated](https://quoteinvestigator.com/2024/06/07/report-death/). No — **finished**. Nothing this session needs is missing, and it runs on twenty years of hardware. If the window manager crashes, apps survive and it respawns around them. If Xorg stops, Xlibre exists; if toolkits leave, the group model boards river. |
| **Isn't X11 insecure?** | Every client can observe every other client. That same exposure powers this repo's two-line tricks. Treat your apps as mutually trusted; quarantine the rest in a VM or Xephyr. |
| **Multi-monitor?** | One screen, by design. Hotplug mirrors automatically for presenting. |
| **Light theme?** | No. Let there be dark. |
| **HDR, VRR, 10-bit, fractional scaling?** | Out of scope. Use a Wayland compositor. |
| **Why not dwm / awesome?** | Same lineage: combinable views are **tags**, and dwm shipped them in 2006. This is the gentlest on-ramp: floating-first, tiling as a moment, a short config instead of a recompile. |
| **Where is the settings app?** | You're in it. Welcome to the matrix. Every behaviour names its owning file (Source column); the cheatsheet (`W-x x`) is generated from live configs. |

## Credits

Thanks to the sources of the downloaded media assets: see [Credits](extended.md#credits).

## Footnotes

### <a id="fn-1a"></a>1a — Numpad grid

|                                  |                                   |                                   |
|:--------------------------------:|:---------------------------------:|:---------------------------------:|
| **7**<br>Top-Left                | **8**<br>Top (full width)         | **9**<br>Top-Right                |
| **4**<br>Left (full height)      | **5**<br>Full height & width      | **6**<br>Right (full height)      |
| **1**<br>Bottom-Left             | **2**<br>Bottom (full width)      | **3**<br>Bottom-Right             |

### <a id="fn-1b"></a>1b — Rogue keys, same grid, home-row edition

|                                  |                                   |                                   |
|:--------------------------------:|:---------------------------------:|:---------------------------------:|
| **y**<br>Top-Left                | **k**<br>Top (full width)         | **u**<br>Top-Right                |
| **h**<br>Left (full height)      | **m**<br>Full height & width<br>*(Align only)* | **l**<br>Right (full height)      |
| **b**<br>Bottom-Left             | **j**<br>Bottom (full width)      | **n**<br>Bottom-Right             |

Under **Align** (`WC-`) the grid reads as regions. **Move** (`W-`) shifts the top-left corner; **Resize** (`WS-`) shifts the bottom-right corner. `m` is Align-only: `W-m` is the master–slave layout (see [Layouts](#layouts)), and `WS-m` is unused.

### <a id="fn-1c"></a>1c — Snap directions

|                                  |                                   |                                   |
|:--------------------------------:|:---------------------------------:|:---------------------------------:|
| **Shift + ←**<br>Top-Left        | **↑**<br>Top                      | **Shift + ↑**<br>Top-Right        |
| **←**<br>Left                    |                                   | **→**<br>Right                    |
| **Shift + ↓**<br>Bottom-Left     | **↓**<br>Bottom                   | **Shift + →**<br>Bottom-Right     |

### 2a–2g — Application notes

| ID | Note |
|----|------|
| <a id="fn-2a"></a>2a | Configure the terminal in `cwmrc`, `sxhkdrc` & `custom/layout`. Find the lines with `grep -rn "#Terminal & Browser"`. |
| <a id="fn-2b"></a>2b | Extracts aliases from your `.zshrc` (`.bashrc` works too) and offers them for one-shot execution in `xterm`. Shell functions are ignored. |
| <a id="fn-2c"></a>2c | Configure the browser in `cwmrc`, `sxhkdrc` & `custom/layout`. Find the lines with `grep -rn "#Terminal & Browser"`. |
| <a id="fn-2d"></a>2d | Python script (`cv2`): reads a QR code through the camera and copies the payload to the clipboard via `xsel`. |
| <a id="fn-2e"></a>2e | Built into `sxhkdrc`; `ffmpeg` captures one frame of the screen, always fullscreen. |
| <a id="fn-2f"></a>2f | Lockscreen script: captures the screen and blurs it into the lock backdrop, and more; read `custom/lock`. |
| <a id="fn-2g"></a>2g | Built into `sxhkdrc`; asks for a password. |
