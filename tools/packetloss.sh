#!/bin/sh
set -e

setloss() {
	ip6tables -A INPUT -m statistic --mode random \
		--probability "${1}" -j DROP -i tap0
	ip6tables -A OUTPUT -m statistic --mode random \
		--probability "${1}" -j DROP -o tap0
}

mkdir -p results

loss=0.00
while [ "${loss}" != "1.02" ]; do
	printf "\n##\n# %s\n##\n\n" "Running with ${loss} packet loss"
	pcapfile="results/${loss}.pcap"

	setloss "${loss}"
	./run.sh "${pcapfile}"

	ip6tables -F
	loss=$(python -c "print(${loss} + 0.03)")
done
