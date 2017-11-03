#!/bin/sh

[ $# -eq 1 ] || exit 1

pcapfile="${1}"

(tcpdump -n -i tap0 -w "${pcapfile}" 'ip6 && (udp || tcp)' 1>/dev/null 2>&1) &
dumppid=$!
trap "kill ${dumppid}" INT

env -i RIOTBASE=/root/RIOT \
	FEUCHT_PROTO=9P \
	FEUCHT_HOST="fe80::e04b:1bff:fe8d:6b14" \
	/usr/sbin/chroot /mnt/debian-chroot \
	/bin/bash -c 'cd /root/feucht && ./tools/runner.py ; echo $? > /root/exit'

exitstatus=
read -r exitstatus < /mnt/debian-chroot/root/exit

sleep 5

kill ${dumppid}
trap - INT

if [ "${exitstatus}" -ne 0 ]; then
	mv "${pcapfile}" "${pcapfile}.failed"
fi
