# How to Use OctoKnight

OctoKnight is a UCI-compliant chess engine, which means you can use it with any UCI-compatible chess GUI. Here's a general guide on how to set it up.

## 1. Get a Chess GUI

If you don't have one already, you'll need a chess GUI that supports the UCI protocol. Here are a few popular free options:

*   **Arena:** (http://www.playwitharena.de/) - A free and powerful GUI for Windows.
*   **Cute Chess:** (https://github.com/cutechess/cutechess) - A free, open-source GUI for Windows, Linux, and macOS.
*   **Tarrasch Chess GUI:** (https://www.triplehappy.com/) - A simple and clean GUI for Windows.

## 2. Install the Engine

Once you have a GUI, you'll need to "install" OctoKnight. This usually involves telling the GUI where to find the `OctoKnight` executable.

The steps will vary depending on the GUI, but you'll typically find an "Engines" or "Engine Management" menu. From there, you can add a new engine and point it to the `OctoKnight` executable in your `build` directory.

## 3. Configure the Engine

Most GUIs will allow you to configure the engine's settings. OctoKnight doesn't have many configurable options at the moment, but you can usually set things like the hash table size.

## 4. Play!

Once the engine is installed and configured, you can start playing against it. You can also use it to analyze positions or play against other engines.
