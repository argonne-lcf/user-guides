#!/usr/bin/env python3
import socket
import sys

def find_available_port(start_port=8000, max_port=8100):
    """Find the next available port starting from start_port."""
    port = start_port
    while port <= max_port:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.bind(('127.0.0.1', port))
            sock.close()
            return port
        except OSError:
            print(f"Port {port} is in use, trying {port+1}...", file=sys.stderr)
            port += 1
    
    print(f"Error: No available ports in range {start_port}-{max_port}", file=sys.stderr)
    sys.exit(1)

if __name__ == "__main__":
    port = find_available_port()
    # Just print the port to stdout for the Makefile to capture
    print(port) 