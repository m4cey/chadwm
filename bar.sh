#!/bin/bash

# ^c$var^ = fg color
# ^b$var^ = bg color

interval=0

# load colors
. ~/.cache/wal/colors.sh

pkg_updates() {
  #updates=$({ timeout 20 doas xbps-install -un 2>/dev/null || true; } | wc -l) # void
  updates=$({ timeout 20 checkupdates 2>/dev/null || true; } | wc -l) # arch
  # updates=$({ timeout 20 aptitude search '~U' 2>/dev/null || true; } | wc -l)  # apt (ubuntu, debian etc)

	if [ $updates -gt 10 ]; then printf "   ^c$color7^ $updates"" update(s)"; fi
}

volume() {
	printf "^b$foreground^"
	[[ $(pamixer --get-mute) = "true" ]] && printf " mute " && exit

	vol="$(pamixer --get-volume)"

	printf " vol $vol%%"
}

next() {
	if [[ $(playerctl -a status 2> /dev/null | grep -v Stopped | wc -l) -lt 2 ]]; then
		playerctld unshift
	else
		playerctld shift
	fi
	exit
}

getmetadata() {
	metadata=$(playerctl metadata --format '{{markup_escape(artist)}}' 2> /dev/null)
	if [[ -z "$metadata" ]]; then
		metadata=$(playerctl metadata --format '{{markup_escape(title)}}' 2> /dev/null)
	else
		metadata+=$(playerctl metadata --format ' - {{markup_escape(title)}}' 2> /dev/null)
	fi
	printf "$metadata" # | tr "^" - | sed -e "s/&apos;/\'/g"
}
mpris() {
	length=60
	player=$(playerctl -l 2> /dev/null | head -1)
	if [ -n "$player" ]; then
		status=$(playerctl status 2>/dev/null)
		case $status in
			Playing);;
			Paused);;
			Stopped) next; exit ;;
			*) exit ;;
		esac
		metadata=$(getmetadata | sed -E\
			"s/&apos;/\'/g;\
			s/&quote;/\"/g;\
			s/“|”/\"/g;\
			s/&amp;/\&/g"\
		)
		offset=${metadata:$length}
		printf "^c$foreground^%.*s%s" $length "$metadata" ${offset:+"..."} # if offset is not null, print delimiters
	fi
}

battery() {
	printf "^b$foreground^ bat %d%%" $(cat /sys/class/power_supply/BAT0/capacity)
}

clock() {
	printf "^b$color1^ $(date '+%a, %d/%m %I:%M%p') "
}

while true; do

  [ $interval = 0 ] || [ $(($interval % 3600)) = 0 ] && updates=$(pkg_updates)
  interval=$((interval + 1))

  xsetroot -name "   $updates $(mpris)^c$background^ $(volume) $(battery) $(clock)" 2> /dev/null && sleep 1
done
