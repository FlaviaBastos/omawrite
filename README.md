# Omawrite

A dead-simple Markdown writing app built with Qt Quick and C++ that automatically follows system dark/light mode.

This is a personal fork of [omacom/omawrite](https://github.com/omacom/omawrite) that adds a live Markdown preview window (`Ctrl+E`). The upstream writing surface is unchanged.

<img width="2948" height="3227" alt="screenshot-2026-06-23_15-24-08" src="https://github.com/user-attachments/assets/4e930c0d-edda-4046-b444-a59eff523329" />
<img width="2948" height="3227" alt="screenshot-2026-06-23_15-23-23" src="https://github.com/user-attachments/assets/8ced7c26-961b-4ded-b263-84403001a951" />


## Install

The packaged Omarchy `omawrite` stays in `/usr/bin`. This fork never replaces that file, so `omarchy update` cannot overwrite the preview build.

To live on the fork:

```sh
./bin/install-user
```

That builds the app and installs a user overlay:

- `~/.local/bin/omawrite`
- `~/.local/share/applications/omawrite.desktop` (shadows the packaged launcher)

On Omarchy, Super+Shift+W must launch that overlay binary. `uwsm-app -- omawrite` can still pick `/usr/bin/omawrite` because the session PATH lists `/usr/bin` first. `~/.config/hypr/bindings.lua` on this machine points Super+Shift+W at `~/.local/bin/omawrite`.

Check which binary a launch will hit:

```sh
./bin/install-user --status
```

To go back to the packaged app (including if upstream later ships preview):

```sh
./bin/uninstall-user
```

That only deletes the overlay. The Omarchy package is left in place.

## Shortcuts

## Shortcuts

- `Ctrl+S` saves. Unsaved documents use the XDG desktop portal file picker.
- `Ctrl+Shift+S` saves as.
- `Ctrl+O` opens a Markdown file through the portal picker.
- `Ctrl+P` opens the system print dialog.
- `Ctrl+E` opens or closes a live Markdown preview in a second window. Hyprland tiles it next to the editor. The footer eye does the same.
- `Ctrl+N` opens a new Omawrite window.
- `Ctrl+Z`, `Ctrl+Shift+Z`, and `Ctrl+Y` handle undo and redo.
- `Super+F` toggles fullscreen. Qt maps this key as `Meta+F`.
- `Ctrl+F` searches the document. Use `Enter` or `Ctrl+G` for the next match and `Shift+Enter` for the previous match.
- `Ctrl+H` opens find and replace.
- `Ctrl+B`, `Ctrl+I`, and `Ctrl+K` insert bold, italic, and link Markdown.
- `Ctrl+?` shows the keyboard shortcut reference.

Unsaved drafts are recovered after an abnormal exit. Omawrite also watches open files
and warns before an external change can replace local work.

Text follows the desktop text size — `omarchy display text size`, or GNOME's
`text-scaling-factor` — and re-flows without a restart. The default of 12px leaves
Omawrite at the size it is designed around; larger and smaller sizes scale from there.

## Requirements

- Qt 6: `qt6-base`, `qt6-declarative`, `qt6-quickcontrols2`
- `xdg-desktop-portal` and a portal backend

The iA Writer Mono font is bundled under the SIL Open Font License 1.1; see
`fonts/OFL.txt`. The font is copyright Information Architects Inc. and based on
IBM Plex, copyright IBM Corp.
