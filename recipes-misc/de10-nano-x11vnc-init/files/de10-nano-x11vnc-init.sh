#!/bin/sh

X11VNC__SH_ENV="/home/root/.x11vnc_env"
X11VNC_HOME="/home/root"

# create this environment setup file for the shell in the x11vnc session
[ -f "${X11VNC__SH_ENV}" ] || {
cat <<EOF > "${X11VNC__SH_ENV}"

# this file is created by the script ‘/usr/bin/de10-nano-x11vnc-init.sh’.
#

# this file is assigned to the ENV environment variable which is evaluated
# when /bin/sh is executed.  We use this to setup environment variables for
# that shell.

export PS1='\u@\h:\w$ '
EOF
}

# then execute this command to start the x11vnc session
FD_PROG=/usr/bin/xfce4-session \
/usr/bin/x11vnc \
-loop \
-repeat \
-env ENV="${X11VNC__SH_ENV}" \
-env HOME="${X11VNC_HOME}" \
-display WAIT:cmd=FINDCREATEDISPLAY-Xvfb

