#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Ubuntu
requires=(
	ccache # Use ccache to speed up build
)

# https://git.launchpad.net/ubuntu/+source/libmateweather/tree/debian/control
requires+=(
	autopoint
	gcc
	git
	gtk-doc-tools
	libglib2.0-dev
	libgtk-3-dev
	libsoup-3.0-dev
	libxml2-dev
	libxml2-utils
	make
	mate-common
	tzdata
)

infobegin "Update system"
apt-get update -y
infoend

infobegin "Install dependency packages"
env DEBIAN_FRONTEND=noninteractive \
	apt-get install --assume-yes \
	${requires[@]}
infoend
