# RLSDKGenerator

A high-performance C++ SDK generator for Rocket League (Unreal Engine 3). It scans the game's memory to reconstruct the internal object database into usable C++ headers.

## Credits & Special Thanks
This project is built by:
- **ItsBranK** (brank) - Primary Author & Lead Developer
- **TheFeckless** - Core SDK Logic & Research
- **crunchy** - Optimization, Maintenance & Contributions
- **OpenCode + GPT 5.3 Codex** - Code Generation & Optimization

## Features
- **Full SDK Generation**: Maps `Core`, `Engine`, `TAGame`, and `ProjectX`.
- **Memory Scanning**: Robust pattern matching for `GObjects` and `GNames`.
- **Member Alignment**: Automatic padding calculation for 1:1 binary compatibility.
- **Customizable Output**: Easily configurable paths and logging.

## Usage
1. Build the solution (`RLSDKGenerator.sln`) in Release x64 mode.
2. Launch Rocket League without EAC enabled. (use -noeac launch option)
3. Inject the resulting `RLSDKGenerator.dll` into Rocket League (Using any injector, EX: https://github.com/CrunchyRL/CMInjector).
4. Check the `RLSDKGenerator` folder in the game directory for the generated headers.

---
*Disclaimer: This tool is for educational and research purposes only. Psyonix has stated that they allow this, for offline modding. Use at your own risk, and be aware of legal consequences.*

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact
For any questions or inquiries, please contact 0ekn on Discord.