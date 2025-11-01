#!/usr/bin/env python3

import argparse
import logging
import ok_logging_setup
import ok_subprocess_defaults
import shutil
import subprocess
from pathlib import Path
from PIL import Image, ImageChops

ok_logging_setup.install()
ok_logging_setup.skip_traceback_for(subprocess.CalledProcessError)

parser = argparse.ArgumentParser()
parser.add_argument("test_dir", nargs="*", type=Path)
args = parser.parse_args()

tests_dir = Path(__file__).parent
test_dirs = list(args.test_dir or tests_dir.glob("*_test"))
for test_dir in test_dirs:
    logging.info(f"\n🧪 TEST: {test_dir}/")
    sub = ok_subprocess_defaults.SubprocessDefaults(cwd=test_dir)
    sub.run("rm", "-rf", "output.tmp")
    sub.run("arduino-cli", "compile", f"--output-dir=output.tmp")
    sub.run("wokwi-cli", test_dir, "--scenario=scenario.yaml")

    for expected_path in test_dir.glob("expected.*"):
        logging.info(f"🔎 Comparing: {expected_path}")
        actual_path = test_dir / "output.tmp" / expected_path.name[9:]
        if not actual_path.is_file():
            ok_logging_setup.exit(f"Missing expected file: {actual_path}")
        elif actual_path.suffix == ".png":
            expected_image = Image.open(expected_path).convert("RGB")
            image = Image.open(actual_path).convert("RGB")
            diff = ImageChops.difference(expected_image, image)
            mask = diff.convert("L").point(lambda x: 127 if x else 0)
            diff_path = actual_path.parent / f"diff.{actual_path.name}"
            red_image = Image.new("RGB", expected_image.size, (255, 0, 0))
            image.paste(red_image, (0, 0), mask)
            while max(image.size) < 600:
                image = image.resize([d * 2 for d in image.size], 0)
            image.save(diff_path)
            if mask.getbbox():
                ok_logging_setup.exit(f"Images differ: {diff_path}")
        else:
            ok_logging_setup.exit(f"Bad comparison file: {expected_path}")

logging.info(f"\n🎉 All {len(test_dirs)} tests passed")
