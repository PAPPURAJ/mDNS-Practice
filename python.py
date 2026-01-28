import time
import socket
import requests
from zeroconf import Zeroconf, ServiceBrowser, ServiceStateChange


SERVICE = "_myapp._tcp.local."


def _safe_decode(b):
    if b is None:
        return ""
    if isinstance(b, bytes):
        try:
            return b.decode("utf-8", errors="ignore")
        except Exception:
            return str(b)
    return str(b)


class Listener:
    def __init__(self):
        self.found = {}  # key -> info dict

    def remove_service(self, zeroconf, type_, name):
        # Device went offline (optional)
        self.found.pop(name, None)

    def add_service(self, zeroconf, type_, name):
        info = zeroconf.get_service_info(type_, name, timeout=2000)
        if not info:
            return

        addrs = []
        for addr in info.addresses:
            try:
                addrs.append(socket.inet_ntoa(addr))
            except OSError:
                pass

        props = { _safe_decode(k): _safe_decode(v) for k, v in (info.properties or {}).items() }

        self.found[name] = {
            "service_name": name,
            "addresses": addrs,
            "port": info.port,
            "server": info.server,
            "properties": props
        }


def discover(timeout_sec=5):
    zc = Zeroconf()
    listener = Listener()
    browser = ServiceBrowser(zc, SERVICE, listener)

    time.sleep(timeout_sec)

    zc.close()
    return list(listener.found.values())


def main():
    print(f"Searching for service: {SERVICE}")
    devices = discover(timeout_sec=5)

    if not devices:
        print("No devices found. (Same Wi-Fi? multicast allowed?)")
        return

    print(f"Found {len(devices)} device(s):\n")
    for d in devices:
        addr = d["addresses"][0] if d["addresses"] else "?"
        print("-" * 50)
        print("Service:", d["service_name"])
        print("IP:     ", addr)
        print("Port:   ", d["port"])
        print("Server: ", d["server"])
        print("TXT:    ", d["properties"])

        # Optional: call /info endpoint
        if addr != "?":
            url = f"http://{addr}:{d['port']}/info"
            try:
                r = requests.get(url, timeout=1.5)
                print("GET /info:", r.status_code, r.text)
            except Exception as e:
                print("GET /info failed:", e)

    print("\nDone.")


if __name__ == "__main__":
    main()
