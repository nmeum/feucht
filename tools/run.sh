#!/bin/sh

[ $# -eq 1 ] || exit 1

pcapfile=${1}
datasize=5

(tcpdump -n -i tap0 -w "${pcapfile}" 'ip6 && (udp || tcp)' 1>/dev/null 2>&1) &
dumppid=$!
trap "kill ${dumppid}" INT

mkdir -p /tmp/ufs-root
printf "" > /tmp/ufs-root/humidity

(ufs -ntype tcp6 -root /tmp/ufs-root 1>/dev/null 2>&1) &
serverpid=$!
trap "kill ${serverpid}" INT

env -i RIOTBASE=/root/RIOT \
	FEUCHT_PROTO=9P \
	FEUCHT_HOST="fe80::e04b:1bff:fe8d:6b14" \
	/usr/sbin/chroot /mnt/debian-chroot \
	/bin/bash -c 'cd /root/feucht && ./tools/runner.py ; echo $? > /root/exit'

exitstatus=
read -r exitstatus < /mnt/debian-chroot/root/exit

sleep 5

kill ${serverpid}
kill ${dumppid}
trap - INT

sleep 1

if [ "${exitstatus}" -eq 0 ]; then
	transactions=$(wc -l < /tmp/ufs-root/humidity)
	echo "${transactions}" > "${pcapfile}.transactions"

	./pcap "${pcapfile}" $((datasize * transactions)) \
		> "${pcapfile}.primarydata"
fi
