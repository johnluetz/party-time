import socket
import struct
from time import sleep

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 4210
# regarding socket.IP_MULTICAST_TTL
# ---------------------------------
# for all packets sent, after two hops on the network the packet will not 
# be re-sent/broadcast (see https://www.tldp.org/HOWTO/Multicast-HOWTO-6.html)
MULTICAST_TTL = 2

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
sock.settimeout(0.05)
sock.setblocking(False)
# For Python 3, change next line to 'sock.sendto(b"robot", ...' to avoid the
# "bytes-like object is required" msg (https://stackoverflow.com/a/42612820)

while True:
    # target = int(input("target:"))
    target = 0
    type = int(input("type:"))
    animation = int(input("animation:"))
    on_t = int(input("on:"))
    off_t = int(input("off:"))
    intensity = int(input("intensity:"))
    color1 = bytes(input("color1:")[0], 'utf-8')
    color2 = bytes(input("color2:")[0], 'utf-8') 
    color3 = bytes(input("color3:")[0], 'utf-8') 
    
    # packet = struct.pack('!s', color)
    print(f"""
    SENDING:
    Target: {target}
    Type: {type}
    Animation: {animation}
    On Time: {on_t}
    Off Time: {off_t}
    Intensity: {intensity}
    Color 1: {color1}
    Color 2: {color2}
    Color 3: {color3}""")

    packet = struct.pack('iiiiiiccc', target, type, animation, on_t, off_t, intensity, color1, color2, color3)

    sock.sendto(packet, (MCAST_GRP, MCAST_PORT))
    # sock.sendto(str.encode("A"), (MCAST_GRP, MCAST_PORT))
    # sleep(0.1)
    # sock.sendto(str.encode("B"), (MCAST_GRP, MCAST_PORT))
    # sleep(0.1)

    