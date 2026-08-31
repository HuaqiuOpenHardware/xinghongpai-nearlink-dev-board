# Hardware | 硬件设计

This directory contains the KiCad hardware source files and BOM for the Xinghongpai NearLink development board.

本目录保存星鸿派 WS63V100 / Hi3863 星闪开发板的 KiCad 原理图、PCB 与 BOM。制造或修改前请同时核对原理图、PCB、BOM 和实际板卡版本。

## Files

- `kicad/*.kicad_pro`: KiCad project.
- `kicad/*.kicad_sch`: Schematic source.
- `kicad/*.kicad_pcb`: PCB layout source.
- `bom/BOM_Board1_Schematic1_2026-01-09.xlsx`: Bill of materials.

## Notes

The p.eda.cn page describes a 96 mm x 70 mm two-layer PCB. Gerber files were mentioned on the page but were not present as separate files in the downloaded PCB zip used for this import.

Before fabrication:

1. Open the complete project rather than a single schematic or PCB file.
2. Re-run ERC/DRC with the KiCad version used for your modification.
3. Verify footprints, alternative parts, connector orientation, power rails and RF-related layout against the intended board revision.
4. Generate fresh Gerber, drill, position and BOM outputs from the reviewed source. Do not treat generated files from an unknown revision as authoritative.
5. Record material substitutions and board changes in the Pull Request.

The presence of source files is not evidence that a modified board has been fabricated or electrically validated. Clearly label unverified derivatives.
