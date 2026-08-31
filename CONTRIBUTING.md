# Contributing | 参与贡献

Thanks for improving this open hardware project.

Please include the following when opening issues or pull requests:

- Board revision and KiCad version for hardware changes.
- SDK / OpenHarmony version for firmware changes.
- Photos, logs or oscilloscope captures when reporting hardware behavior.
- BOM impact when replacing components.

Keep generated fabrication outputs separate from source changes, and document any license impact when adding third-party material.

## Recommended workflow

1. Search existing Issues before opening a duplicate.
2. Keep one Pull Request focused on one hardware, firmware or documentation change.
3. Explain what changed, why it changed and how it was verified.
4. Update the relevant README or factsheet when behavior, files or compatibility changes.
5. Preserve upstream copyright and license notices.

## Verification levels

Use precise language in Issues and Pull Requests:

- **Inspected**: source or design files were reviewed but not built or fabricated.
- **Built**: firmware compiled successfully; include SDK version and build command.
- **Bench tested**: tested on named board revision; include setup, logs and measured result.
- **Fabricated**: PCB was manufactured and assembled from the submitted design; include revision and known deviations.

Do not describe inferred performance, radio range, power consumption, compatibility or hardware behavior as measured unless evidence is included.

## Pull Request scope

- Hardware changes should include the KiCad source, affected BOM entries, ERC/DRC status and board-revision impact.
- Firmware changes should include the target example, SDK/OpenHarmony version, build result and runtime evidence when available.
- Documentation changes should link to a primary source for new specifications or clearly mark information as unverified.
