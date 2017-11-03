package main

import (
	"fmt"
	"os"
	"strconv"

	"github.com/google/gopacket"
	"github.com/google/gopacket/pcap"
)

func main() {
	if len(os.Args) < 3 {
		os.Exit(1)
	}

	handle, err := pcap.OpenOffline(os.Args[1])
	if err != nil {
		panic(err)
	}

	primarydata, err := strconv.Atoi(os.Args[2])
	if err != nil {
		panic(err)
	}

	var lastPacket, firstPacket gopacket.Packet
	packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
	for packet := range packetSource.Packets() {
		if firstPacket == nil {
			firstPacket = packet
		}

		lastPacket = packet
	}

	t := lastPacket.Metadata().Timestamp
	d := t.Sub(firstPacket.Metadata().Timestamp)

	var res float64 = float64(primarydata) / d.Seconds()
	fmt.Println(res)
}
