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

def single_run(start_ip,count):
  collected = subprocess.run(["./pinger", "ens3", start_ip, str(count)], capture_output=True)
  return collected.stdout.decode("utf8")

def last_block(result_file):
  last_seen = None
  for line in result_file:
    last_seen = parse_last_address(line) or last_seen
  return last_seen

if __name__ == "__main__":
  batch_size = 256

  initial_address = "0.0.0.0" 
  if len(sys.argv) > 1:
    with open(sys.argv[1], "r") as f:
      initial_address = last_block(f)

  next_address = initial_address
  while True:
    print(f"Starting at {next_address}")
    current_result = single_run(next_address, batch_size)
    next_address = last_block(current_result.split("\n"))
    with open('result.txt', 'a') as f:
      f.write(current_result)


