#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Archlinux
requires=(
	ccache # Use ccache to speed up build
	clang  # Build with clang on Archlinux
)

# https://gitlab.archlinux.org/archlinux/packaging/packages/libmateweather
requires+=(
	dconf
	gdk-pixbuf2
	glib2
	glibc
	gtk3
	hicolor-icon-theme
	libsoup3
	libxml2
	git
	glib2-devel
	gobject-introspection
	gtk-doc
	make
	mate-common
	tar
	which
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend
