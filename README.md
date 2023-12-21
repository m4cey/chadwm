# chadwm
Fork of chadwm, dwm only with personal touches

# changes
- Applied dwm-xresources-6 patch with font and colorscheme support
- Disabled gap controls because I don't need them
- Fullscreen hints don't control window geometry, and the
  fullscreengeometry function doesn't change window hints.
  This allows proper fake fullscreen functionality while still being
  able to set windows to fullscreen.
- Run autostart scripts in ```sh\~/.local/share/dwm/{autostart.sh, autostart_blocking.sh}```
- Added a `textsystray` switch; System tray icons are displayed with the 2 first characters of the name by default
- Some program shortcuts
