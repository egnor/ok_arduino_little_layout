import logging
import ok_logging_setup
import ok_subprocess_defaults
import pytest
import shutil
import subprocess
from pathlib import Path
from PIL import Image, ImageChops

ok_logging_setup.install({"OK_LOGGING_OUTPUT": "stdout"})
ok_logging_setup.skip_traceback_for(subprocess.CalledProcessError)

@pytest.fixture(scope="module")
def wokwi_output_dir(request):
    sketch_dir = Path(request.path).parent
    output_dir = sketch_dir / "output.tmp"
    if output_dir.is_dir(): shutil.rmtree(output_dir)

    sub = ok_subprocess_defaults.SubprocessDefaults(cwd=sketch_dir)
    sub.run("arduino-cli", "compile", "--output-dir=output.tmp")
    sub.run(
        "wokwi-cli",
        "--scenario=scenario.yaml",
        "--serial-log-file=output.tmp/serial_log.txt"
    )
    return output_dir


@pytest.fixture(scope="module")
def diff_image(request, tmp_path_factory):
    def _run_diff(a_path, b_path):
        logging.info(f"🔎 Comparing:\n- {a_path}\n- {b_path}")
        a_image = Image.open(a_path).convert("RGB")
        b_image = Image.open(b_path).convert("RGB")

        diff = ImageChops.difference(a_image, b_image)
        diff_mask = diff.convert("L").point(lambda x: 127 if x else 0)
        red_image = Image.new("RGB", b_image.size, (255, 0, 0))
        a_image.paste(red_image, (0, 0), diff_mask)
        while max(a_image.size) < 600:
            a_image = a_image.resize([d * 2 for d in a_image.size], 0)

        test_module_name = request.module.__name__
        diff_path = tmp_path_factory.mktemp(test_module_name) / a_path.name
        a_image.save(diff_path)
        if diff_mask.getbbox():
            raise ValueError(f"Images differ, see:\n  {diff_path}")

        logging.info(f"👾 Clean diff: {diff_path}")

    return _run_diff
