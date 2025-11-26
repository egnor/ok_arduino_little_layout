def test_basic_layout(wokwi_output_dir, diff_image):
    diff_image(
        wokwi_output_dir / "screenshot.png",
        wokwi_output_dir.parent / "expected.screenshot.png"
    )
