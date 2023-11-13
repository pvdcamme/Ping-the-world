"""
  The IPv4 address-space is large. This tool helps pinging by splitting up
  the work in smaller batches.

  The script will loop continuously, each batch starting from the last pinged
  address.
"""
import os
import sys
import subprocess

def single_run(start_ip,count):
  collected = subprocess.run(["./pinger", "ens3", start_ip, str(count)], capture_output=True)
  return collected.stdout.decode("utf8")

def parse_last_address(line):
  key = "Last address: "
  if key in line:
    return line[len(key):].strip()
  else:
    return None

def search_last_address(output_lines):
  last_seen = None
  for line in output_lines:
    last_seen = parse_last_address(line) or last_seen
  return last_seen

def save_output(output, target):
  with open(target, "a") as f:
    f.write(output)

def find_start(file_name, default_start = "0.0.0.0"):
  try:
    with open(file_name, "r") as f:
      return search_last_address(f)
  except FileNotFoundError:
    return default_start

if __name__ == "__main__":
  batch_size = 256

  next_address = "0.0.0.0" 
  save_file = "result.txt"
  if len(sys.argv) > 1:
    save_file = sys.argv[1]
    next_address = find_start(sys.argv[1], next_address)

  while True:
    print(f"Starting at {next_address}")
    current_result = single_run(next_address, batch_size)
    next_address = search_last_address(current_result.split("\n"))
    save_output(current_result, save_file)


