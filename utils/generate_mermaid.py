#!/usr/bin/python3

import sys
import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import glob

def print_usage():
    print(f'Usage: ./generate_mermaid.py "docs/diagrams/*.mmd"')

def generate_mermaid_diagrams(f):
    try:
        print(f'Generating Mermaid diagrams from {f}')
        f_svg = Path(f).with_suffix('.svg').name
        f_png = Path(f).with_suffix('.png').name

        target_dir = Path(f).parent.absolute().joinpath('mermaid')
        target_svg = target_dir.joinpath(f_svg)
        target_png = target_dir.joinpath(f_png)

        # Create target directory if it does not exist
        target_dir.mkdir(parents=True, exist_ok=True)

        # Prepare commands to generate SVG and PNG files
        cmd_svg = ['mmdc', '-i', f, '-o', str(target_svg), '-t', 'dark', '-b', 'transparent']
        cmd_png = ['mmdc', '-i', f, '-o', str(target_png), '-t', 'dark', '-b', 'transparent', '-w', '10000', '-H', '10000']

		# On Windows, the mmdc.cmd script is used to run the mmdc command
        if sys.platform == 'win32':
            cmd_svg[0] = 'mmdc.cmd'
            cmd_png[0] = 'mmdc.cmd'

        # Execute commands
        subprocess.run(cmd_svg, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        subprocess.run(cmd_png, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    except subprocess.CalledProcessError as e:
        print(f'ERROR: Generating Mermaid diagrams from {f} failed with error: {e.stderr.decode()}')
        return False

    return True

# Process wildcard patterns
def expand_wildcards(pattern):
    return glob.glob(pattern)

if len(sys.argv) < 2:
    print_usage()
    sys.exit(1)

patterns = sys.argv[1:]

# Collect all files matching the patterns
files = []
for pattern in patterns:
    files.extend(expand_wildcards(pattern))

if not files:
    print_usage()
    sys.exit(1)

ok = 0

with ThreadPoolExecutor(max_workers=16) as executor:
    result = list(executor.map(lambda f: generate_mermaid_diagrams(f), files))
    ok = all(result)

sys.exit(0 if ok else 1)
