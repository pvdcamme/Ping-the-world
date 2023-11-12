import os
import sys
import subprocess


def run_once(start_ip,count):
  collected = subprocess.run(["./pinger", "ens3", start_ip, str(count)], capture_output=True)
  print(collected)


if __name__ == "__main__":
  batch_size = 255
  run_once("0.0.0.0", batch_size)

  


