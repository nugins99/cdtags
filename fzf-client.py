#!/usr/bin/env python3
"""
Simple JSON-RPC client that launches the fuzzy-search binary with --stdin --jsonrpc
and sends/receives messages over pipes.

Usage:
  python3 scripts/fuzzy_search_json.py --binary ./build/release/bin/fuzzy-search --text "example"
  or interactively:
  python3 scripts/fuzzy_search_json.py
"""
import argparse
import json
import subprocess
import threading
import time
import sys

class FzfJsonClient:
    def __init__(self, binary="./build/debug/bin/fuzzy-search", search="", extra_args=None):
        args = [binary, "--search", search, "--files", "--jsonrpc"]
        if extra_args:
            args += extra_args
        # Start process with line-buffered text pipes
        self.proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1)
        self._reader_thread = threading.Thread(target=self._reader, daemon=True)
        self._reader_thread.start()
        self._final_result = None
        self._final_event = threading.Event()

    def _reader(self):
        # Read lines from stdout and process JSON notifications
        for raw in self.proc.stdout:
            line = raw.strip()
            if not line:
                continue
            try:
                obj = json.loads(line)
            except Exception:
                # Not JSON -- print raw
                print("<<", line)
                continue

            method = obj.get("method")
            if method == "results":
                # results payload in params
                params = obj.get("params", {})
                print("RESULTS:", json.dumps(params, indent=2))
            elif method == "progress":
                print("PROGRESS:", obj.get("params"))
            elif method == "finalResult":
                params = obj.get("params", {})
                self._final_result = params.get("result")
                print("FINAL:", self._final_result)
                self._final_event.set()
            else:
                # Unknown/other notifications
                print("NOTIF:", json.dumps(obj))
        # stdout closed
        self._final_event.set()

    def send_input(self, typ, character=None):
        msg = {"jsonrpc": "2.0", "method": "input", "params": {"type": typ}}
        if character is not None:
            # JSONRPCInterface accepts "character" or "char"
            msg["params"]["character"] = character
        line = json.dumps(msg) + "\n"
        try:
            self.proc.stdin.write(line)
            self.proc.stdin.flush()
        except BrokenPipeError:
            print("Pipe closed (process exited).")

    def wait_final(self, timeout=None):
        self._final_event.wait(timeout)
        return self._final_result

    def stop(self):
        try:
            if self.proc.stdin:
                self.proc.stdin.close()
        except Exception:
            pass
        try:
            self.proc.terminate()
        except Exception:
            pass
        self.proc.wait(timeout=1)

def demo_noninteractive(client, text):
    # Send each character as PrintableChar
    for ch in text:
        client.send_input("PrintableChar", ch)
        time.sleep(0.05)
    # send newline to accept / finish
    client.send_input("Newline")
    # wait for final result (if any)
    final = client.wait_final(timeout=5.0)
    print("Demo final result:", final)

def interactive_loop(client):
    print("Interactive mode. Type characters, ENTER to finish, BACKSPACE to delete, ':up' or ':down' for arrows, ':quit' to exit.")
    while True:
        try:
            s = input("> ")
        except EOFError:
            break
        if s == ":quit":
            break
        if s == ":up":
            client.send_input("UpArrow")
            continue
        if s == ":down":
            client.send_input("DownArrow")
            continue
        if s == ":back":
            client.send_input("Backspace")
            continue
        # Send each char
        for ch in s:
            client.send_input("PrintableChar", ch)
        # send newline to submit this line to fuzzy-search
        client.send_input("Newline")
        # wait briefly for a response
        time.sleep(0.1)
    # Optionally wait for final result
    final = client.wait_final(timeout=1.0)
    if final:
        print("Final:", final)

def main():
    p = argparse.ArgumentParser(description="Launch fuzzy-search (JSON-RPC) and talk via pipes.")
    p.add_argument("--binary", default="./build/debug/bin/fuzzy-search", help="Path to fuzzy-search binary")
    p.add_argument("--search", default="", help="Initial --search value")
    p.add_argument("--text", help="If provided, send this text then newline and exit")
    args = p.parse_args()

    client = FzfJsonClient(binary=args.binary, search=args.search)
    try:
        if args.text:
            demo_noninteractive(client, args.text)
        else:
            interactive_loop(client)
    finally:
        client.stop()

if __name__ == "__main__":
    main()
