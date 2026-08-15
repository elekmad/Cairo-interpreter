import json
import subprocess
import os

import argparse

#print("Proto Start\n")

parser = argparse.ArgumentParser()

parser.add_argument(
    "--include",
    action="append",
    default=[],
    help="Répertoire d'inclusion"
)

parser.add_argument(
    "--source",
    action="append",
    default=[],
    help="Répertoire d'inclusion"
)

parser.add_argument(
    "--compile",
    action="append",
    default=[],
    help="Répertoire d'inclusion"
)

args = parser.parse_args()
#print(args.include)

#with open("build/compile_commands.json") as f:
with open(args.compile[0]+"/build/compile_commands.json") as f:
    db = json.load(f)
    
#print("source : "+os.path.basename(args.source[0]))

for entry in db:
    src = entry["file"]
    
    #print("liste : "+os.path.basename(src))
    
    if os.path.basename(src) != os.path.basename(args.source[0]):
        continue

    if not src.endswith(".c"):
        continue

    cmd = entry["command"].split()
    compiler_flags = []

    i = 0
    while i < len(cmd):
        arg = cmd[i]

        if arg.startswith(("-I", "-D")):#, "-U")):
            compiler_flags.append(arg)

        elif arg in ("-isystem", "-include", "-imacros"):
            compiler_flags.extend(["-I", cmd[i + 1]])
            i += 1

        i += 1

#    print(compiler_flags)

    proto = os.path.splitext(src)[0] + ".proto.h"

    subprocess.run(
        ["cproto", *compiler_flags, src],
        stdout=open(proto, "w"), stderr=open("/dev/null", "w")
    )
#    print(["cproto", *compiler_flags, src])

#print("Proto End\n")
