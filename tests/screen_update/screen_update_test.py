def test_screen_update(wokwi_output_dir, diff_image):
    for n in (1, 2, 3, 4, 5):
        diff_image(
            wokwi_output_dir / f"screenshot-{n}.png",
            wokwi_output_dir.parent / f"expected.screenshot-{n}.png"
        )
