"""
  The IPv4 address-space is large. This tool helps pinging by splitting up
  the work in smaller batches.
"""
import os
import sys
import subprocess


def run_once(start_ip,count):
  collected = subprocess.run(["./pinger", "ens3", start_ip, str(count)], capture_output=True)
  result = collected.stdout.decode("utf8")

  key = "Last address: "
  for line in result.split("\n"):
    if key in line:
      next_start = line[len(key):].strip()
      return next_start, result


if __name__ == "__main__":
  batch_size = 255

  initial_address = "0.0.0.0" 
  next_address = initial_address
  while True:
    print(f"Starting at {next_address}")
    next_address, result = run_once(next_address, batch_size)
    with open('result.txt', 'a') as f:
      f.write(result)


  


