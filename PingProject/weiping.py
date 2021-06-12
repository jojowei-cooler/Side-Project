import os
import sys
import argparse
import socket
import struct # used to handle binary
import select
import time
import signal
from datetime import datetime

ICMP_ECHO_REQUEST = 8 # platform specific
DEFAULT_TIMEOUT = 2
DEFAULT_COUNT = 4
DEFAULT_INTERVAL = 0
INFINITE = False
ICMPSIZE = 32
TTL = 115
GETHOSTNAME = False
SHOWTIME = False

class Pinging(object):
    def __init__(self, target_host, count = DEFAULT_COUNT, timeout = DEFAULT_TIMEOUT, interval = DEFAULT_INTERVAL, infinite = INFINITE, icmp_size = ICMPSIZE, ttl = TTL, showtime = SHOWTIME):
        self.target_host = target_host
        self.count = count
        self.timeout = timeout
        self.interval = interval
        self.infinite = infinite
        self.icmp_size = icmp_size
        self.ttl = ttl
        self._ttl = 0
        self.showtime = showtime

        self.total = 0
        self.success = 0
        self.failure = 0
        self.max = 0
        self.min = sys.float_info.max
        self.average = 0
    
    def checksum(self, src_string):
        sum = 0
        max_count = (len(src_string)/2)*2
        count = 0
        while count < max_count:
            val = src_string[count + 1]*256 + src_string[count]
            sum = sum + val
            sum = sum & 0xffffffff
            count += 2
        
        if max_count < len(src_string):
            sum = sum + ord(src_string[len(src_string) - 1])
            sum = sum & 0xffffffff
        
        sum = (sum >> 16) + (sum & 0xffff)
        sum = sum + (sum >> 16)
        result = ~sum
        result = result & 0xffff
        result = result >> 8 | (result << 8 & 0xff00)
        return result

    def receive_ping(self, soc, ID, timeout):
        time_remaining = timeout
        while True:
            start = time.time()
            readable = select.select([soc], [], [], time_remaining)
            time_spent = (time.time() - start)
            if readable[0] == []: # timeout
                return
            
            time_received = time.time()
            recv_packet, addr = soc.recvfrom(1024)
            icmp_header = recv_packet[20:28]
            type, code, checksum, packet_id, sequence = struct.unpack(
                "bbHHh", icmp_header
            )

            if packet_id == ID:
                bytes_in_double = struct.calcsize("d")
                self._ttl = recv_packet[8]
                time_sent = struct.unpack("d", recv_packet[28:28+bytes_in_double])[0]
                return time_received - time_sent
            
            time_remaining = time_remaining - time_spent
            if time_remaining <= 0:
                return
    
    def send_ping(self, soc, ID):
        target_addr = socket.gethostbyname(self.target_host)
        the_checksum = 0
        if not self.icmp_size or self.icmp_size < 11:
            self.icmp_size = ICMPSIZE

        # create a dummy header for 0 checksum
        header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, the_checksum, ID, 1)
        bytes_in_double = struct.calcsize("d")
        data = (self.icmp_size - bytes_in_double) * "Q"
        data = struct.pack("d", time.time()) + bytes(data.encode('utf-8'))

        # check the checksum
        the_checksum = self.checksum(header + data)
        header = struct.pack(
            "bbHHh", ICMP_ECHO_REQUEST, 0, socket.htons(the_checksum), ID, 1
        )

        packet = header + data
        soc.sendto(packet, (target_addr, 1))

    def ping_interval(self):
        icmp = socket.getprotobyname("icmp")
        try:
            soc = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)
            soc.setsockopt(socket.IPPROTO_IP, socket.IP_TTL, self.ttl)
        except socket.error as e:
            if e.errno == 1:
                # not superuser, sp operation not permitted
                e.msg += "ICMP message can only be sent from root user process"
                raise socket.error(e.msg)
        except Exception as e:
            print("Exception: %s" %(e))

        pid = os.getpid() & 0xFFFF
        self.send_ping(soc, pid)
        delay = self.receive_ping(soc, pid, self.timeout)
        soc.close()
        return delay

    def ping_statistics(self):
        self.infinite = False
        print("\n"+self.target_host+"'s Ping Statistics:")
        print("\tpacket: total=%d, success=%d, failure=%d" %(self.total, self.success, self.failure))
        if self.average > 0:
            print("Round Trip Time:")
            print("\tmin=%0.1fms, max=%0.1fms, average=%0.1fms\n" %(self.min, self.max, self.average/self.success))

    def ping(self):
        for i in range(self.count):
            if self.showtime:
                print(datetime.now().time(), end=' ')
            print("ping to %s:" %self.target_host, end=' ')

            try:
                delay = self.ping_interval()
            except socket.gaierror as e:
                print("ping failed. (socket error: '%s)" %e[1])
                break
            
            if delay == None:
                print("ping failed. (timeout within %s sec)" %self.timeout)
                
                self.total += 1
                self.failure += 1
            else:
                delay = delay*1000
                print("bytes=%d time=%0.1fms TTL=%d" %(self.icmp_size,delay,self._ttl))

                self.total += 1
                self.success += 1
                self.average += delay
                if(delay > self.max):
                    self.max = delay
                if(delay < self.min):
                    self.min = delay
            
            if(self.interval > 0):
                time.sleep(self.interval)

            if i == self.count - 1 and infinite == False:
                self.ping_statistics()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='B10702113 Lin Xiu Wei PING PROJECT')
    
    parser.add_argument('host', action="store", help=u'host name')
    parser.add_argument("-n", help="ping count (default = 4)", dest="count", default=DEFAULT_COUNT)
    parser.add_argument("-w", help="timeout (default = 2)", dest="timeout", default=DEFAULT_TIMEOUT)
    parser.add_argument("-d", help="interval between each ping (default = 0)", dest="interval", default=DEFAULT_INTERVAL)
    parser.add_argument("-t", help="ping continuously until the user press \"Ctrl + C\" (maximum = 10000)", action='store_true', dest="infinite") # TLL
    parser.add_argument("-s", help="the size of ICMP packet (default = 32)", dest="icmp_size", default=ICMPSIZE)
    parser.add_argument("-i", help="time to live (default = 115)", dest="ttl", default=TTL)
    parser.add_argument("-a", help="resolve IP address to host name", action='store_true', dest="gethostname")
    parser.add_argument("--show-time", help="show system time", action='store_true', dest="showtime")

    given_args = parser.parse_args()
    target_host = given_args.host
    count = int(given_args.count)
    timeout = float(given_args.timeout)
    interval = float(given_args.interval)
    infinite = given_args.infinite
    icmp_size = int(given_args.icmp_size)
    ttl = int(given_args.ttl)
    gethostname = given_args.gethostname
    showtime = given_args.showtime

    pinging = Pinging(target_host=target_host, count=count, timeout=timeout,
                    interval=interval, infinite=infinite, icmp_size=icmp_size,
                    ttl=ttl, showtime=showtime)
    
    if gethostname:
        print("\nPing "+socket.getfqdn(target_host)+" ["+target_host+"] ", end='')
    else:
        print("\nPing "+target_host, end=' ')
    print("use "+str(icmp_size)+" bytes data:")

    if pinging.infinite:
        while pinging.infinite:
            try:
                pinging.ping()
            except KeyboardInterrupt:
                pinging.ping_statistics()
    else:
        pinging.ping()