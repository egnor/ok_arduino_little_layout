def test_basic_layout(sim_output_dir, diff_image):
    diff_image(
        sim_output_dir / "screenshot.png",
        sim_output_dir.parent / "expected.screenshot.png"
    )
