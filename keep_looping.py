"""
  The IPv4 address-space is large. This tool helps pinging by splitting up
  the work in smaller batches.

  The script will loop continuously, each batch starting from the last pinged
  address.
"""
import os
import sys
import subprocess

def parse_last_address(line):
  key = "Last address: "
  if key in line:
    return line[len(key):].strip()
  else:
    return None


def run_once(start_ip,count):
  collected = subprocess.run(["./pinger", "ens3", start_ip, str(count)], capture_output=True)
  result = collected.stdout.decode("utf8")

  key = "Last address: "
  for line in result.split("\n"):
    if key in line:
      next_start = line[len(key):].strip()
      return next_start, result

def last_block(result_file):
  with open(result_file, "r") as f:
    last_seen = None
    for line in f:
      last_seen = parse_last_address(line) or last_seen
    return last_seen

if __name__ == "__main__":
  batch_size = 256

  initial_address = "0.0.0.0" 
  if len(sys.argv) > 1:
    initial_address = last_block(sys.argv[1])

  next_address = initial_address
  while True:
    print(f"Starting at {next_address}")
    next_address, result = run_once(next_address, batch_size)
    with open('result.txt', 'a') as f:
      f.write(result)


