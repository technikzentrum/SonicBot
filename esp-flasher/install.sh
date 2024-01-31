#!/bin/bash

# Prüfen, ob Python 3 installiert ist
if ! command -v python3 &> /dev/null
then
    echo "Python 3 ist nicht installiert. Installation wird gestartet..."
    # Installieren von Python 3 über Homebrew (muss installiert sein)
    brew install python
else
    echo "Python 3 ist bereits installiert."
fi

# Erstellen einer virtuellen Umgebung im aktuellen Verzeichnis
echo "Erstelle eine virtuelle Umgebung für das Projekt..."
python3 -m venv venv

# Aktivieren der virtuellen Umgebung
source venv/bin/activate

# Installieren der benötigten Bibliotheken
echo "Installiere benötigte Bibliotheken..."
pip install pyserial requests tqdm esptool

echo "Installation abgeschlossen. Virtuelle Umgebung ist bereit."

# Anweisung zum Aktivieren der Umgebung bei zukünftigen Sitzungen
echo "Um die virtuelle Umgebung zu aktivieren, verwenden Sie 'source venv/bin/activate' im Projektverzeichnis."
