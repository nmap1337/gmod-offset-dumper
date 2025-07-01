# skibidiware.cc official dumper for Garry's Mod

## Overview

This project is a Garry's Mod (Source Engine) netvar dumper. It is designed to be injected as a DLL into Garry's Mod, where it will automatically dump all networked variables (netvars) to text files. The ImGui-based menu code is present but commented out, so the DLL acts as a pure dumper by default.

- **DirectX 9**: Used for rendering and hooking EndScene.
- **ImGui**: Used for the (currently disabled) in-game menu.
- **Kiero**: Used for DirectX9 function hooking.
- **MinHook**: Used internally by Kiero for function detouring.

## Features

- Dumps all netvars from Garry's Mod client to `output/netvars.txt` and per-table files in the same folder.
- Console output for status and errors.
- ImGui menu code is included but commented out due to no purpose for it but you can still uncomment it out if you want, it's located in main.cpp file.

## Usage

1. **Build the DLL**
   - Open the Visual Studio solution/project file.
   - Build the project in `Debug` mode for your target architecture (x64 for Garry's Mod).
   - The output DLL will be in the `x64/Debug` or `x64/Release` folder (depending on your configuration).

2. **Inject the DLL**
   - Use your preferred DLL injector (eg. process hacker, extreme injetor, etc.) to inject the built DLL into a running Garry's Mod process (`gmod.exe`).
   - The dumper will automatically create an `output` folder next to `gmod.exe` and write `netvars.txt` and per-table dumps there.
   - The console window will show log output.

3. **(Optional) Enable the Menu**
   - To re-enable the ImGui menu, uncomment the relevant code in `main.cpp` inside the `hkEndScene` function.

## Output

- `output/netvars.txt`: All netvars dumped in a single file.
- `output/<table>.txt`: Each netvar table in its own file.

## Dependencies

- [ImGui](https://github.com/ocornut/imgui) (included but not used)
- [Kiero](https://github.com/Rebzzel/kiero) (included)
- [MinHook](https://github.com/TsudaKageyu/minhook) (included via Kiero)

## Credits

- [ocornut/imgui](https://github.com/ocornut/imgui) for the ImGui library
- [Rebzzel/kiero](https://github.com/Rebzzel/kiero) for the DirectX9 hook
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook) for MinHook

## Disclaimer

This project is for educational and research purposes only. Use responsibly and only in environments where you have permission.

## Thanks for reading!

Star this project, thanks.

---

## Contact us

- [Discord Server](https://discord.gg/zhQWEWBEqG)
- [Telegram](https://t.me/skibidiware_official)

## Support this project

- [CloudTips](https://pay.cloudtips.ru/p/9efc03ba)
