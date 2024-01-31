# README.md für das ESP8266 Flashing Tool

## Übersicht
Dieses Tool ist dafür entwickelt, ESP8266-Mikrocontroller automatisch zu flashen. Es erkennt angeschlossene ESP8266-Geräte, lädt die neuesten Images von einem spezifizierten GitHub-Repository herunter und flasht sie auf die Geräte. Zudem wird eine Blacklist-Funktion unterstützt, um bestimmte Geräte vom Flashen auszuschließen.

## Features
- Automatische Erkennung von ESP8266-Geräten
- Herunterladen und Flashen der neuesten Firmware- und SPIFFS-Images
- Unterstützung einer Blacklist für MAC-Adressen
- Kontinuierliches Scannen von Ports für neue Geräte

## Voraussetzungen
Um dieses Tool verwenden zu können, benötigen Sie:
- Python 3
- Folgende Python-Bibliotheken: `serial`, `requests`, `tqdm`
- `esptool` für das Flashen der Geräte
- Internetverbindung für den Download der Images

## Installation
1. Installieren Sie Python 3 von [python.org](https://www.python.org/downloads/).
2. Installieren Sie die erforderlichen Bibliotheken mit dem Befehl:
   ```
   pip install pyserial requests tqdm esptool
   ```
3. Klonen Sie dieses Repository oder laden Sie die Dateien herunter.

## Verwendung
1. Starten Sie das Tool mit:
   ```
   python <Skriptname>.py
   ```
oder doppelklick
2. Das Tool scannt automatisch nach angeschlossenen ESP8266-Geräten.
3. Bei Erkennung eines Geräts wird es automatisch geflasht, es sei denn, seine MAC-Adresse befindet sich in der `blacklist.txt`.

## Blacklist-Funktion
- Fügen Sie MAC-Adressen, die nicht geflasht werden sollen, in die `blacklist.txt`-Datei ein.
- Jede MAC-Adresse muss in einer neuen Zeile stehen.

## Fehlersuche
- Überprüfen Sie, ob alle benötigten Bibliotheken installiert sind.
- Stellen Sie sicher, dass der ESP8266 korrekt an den Computer angeschlossen ist.
- Bei Problemen mit der automatischen Port-Erkennung überprüfen Sie den Gerätemanager Ihres Betriebssystems.

## Lizenz
TBDQ

## Mitwirkende
Marco Gabrecht
