package main

import (
	"fmt"
	"net"
	"os"
	"time"
)

func main() {
	var host = os.Args[1]
	for {
		ips, err := net.LookupIP(host)
		if err != nil {
            fmt.Println(err)
			os.Exit(1)
		}
		for _, ip := range ips {
			fmt.Printf("%s. IN A %s\n", os.Args[1], ip.String())
		}
		fmt.Printf("\n")
		time.Sleep(200 * time.Millisecond)
	}
}
