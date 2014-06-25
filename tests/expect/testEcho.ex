#!/usr/bin/expect
#
# Start a wolframe daemon and communicate with it
#

package require Expect

set config wolframe.conf

set timeout 3
set serverpid [spawn ../../src/wolframed -c $config -f]

puts "wolframed started with configuration file: $config (pid: $serverpid)"
# allow the server to start
sleep 1

set timeout 3
if { [catch {
	spawn telnet localhost 7661

	expect 	{
		timeout	{ return "--> oops. timeout occured waiting for server banner\n" }
		eof	{ return "--> EOF waiting for server banner: that's bad\n" }
		"OK\r"	{ send_user " --> Connected.\n" }
	}

	set message "1 short line"
	send "$message\r"
	expect "$message\r"

	set message "2nd short line with üèöéäà°§"
	send "$message\r"
	expect "$message\r"

	sleep 0.5

	send "quit\r"
	expect "BYE\r"
	expect 	{
		timeout	{ send_user " --> oops. timeout occured\n"; return 1 }
		eof	{ send_user " --> Connection closed. OK\n" }
	}
} reason] } {
	send_user "Error: $reason\n"
	exit 1
}
