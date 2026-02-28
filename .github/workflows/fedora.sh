#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Fedora
requires=(
	ccache # Use ccache to speed up build
)

# https://src.fedoraproject.org/cgit/rpms/libmateweather.git
requires+=(
	autoconf-archive
	gcc
	git
	gtk3-devel
	libsoup3-devel
	libxml2-devel
	make
	mate-common
	redhat-rpm-config
	tzdata
)

infobegin "Update system"
dnf update -y
infoend

infobegin "Install dependency packages"
dnf install -y ${requires[@]}
infoend
