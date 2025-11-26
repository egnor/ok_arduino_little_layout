def test_screen_update(wokwi_output_dir, diff_image):
    for r in ("r0", "r1", "r2", "r3", "mirror", "mirror-vertical"):
        diff_image(
            wokwi_output_dir / f"screenshot-{r}.png",
            wokwi_output_dir.parent / f"expected.screenshot-{r}.png"
        )
