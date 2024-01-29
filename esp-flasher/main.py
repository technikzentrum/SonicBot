import serial.tools.list_ports
import subprocess
import time
import os
import tkinter as tk
from tkinter import messagebox
import traceback
import esptool
import sys

# Pfad zu den Dateien, die auf den ESP8266 geladen werden sollen
SPIFFS_IMAGE_PATH = "spiffs_image.bin"
CODE_HEX_PATH = "code.bin"


def program_esp8266(com_port):
    global mystdout
    esptool.main(['--port', com_port, 'read_mac'])
    mac = mystdout.getMac()
    print(f"MAC: {mac}")
    #wenn mac in blacklist.txt ist, dann nicht flashen
    if mac in open("blacklist.txt").read():
        print("MAC in blacklist.txt gefunden. Flashen wird abgebrochen.")
        input("Drücke Enter um fortzufahren...")
        print("Danke, ich mache weiter")
        return False
    esptool.main(['--port', com_port, 'erase_flash'])

    print(f"Programmierung von {com_port}...")

    # SPIFFS Image hochladen
    esptool.main(['--port', com_port, '--baud', '460800', 'write_flash', '0x200000', SPIFFS_IMAGE_PATH])
    print("SPIFFS Image wurde erfolgreich hochgeladen.")

    # Code hochladen
    esptool.main(['--port', com_port, '--baud', '460800', '--after', 'hard_reset', 'write_flash', '0x00000',
                  CODE_HEX_PATH])
    print("Code wurde erfolgreich hochgeladen.")

    print(f"{com_port} wurde erfolgreich programmiert.\n")
    #schreibe mac in datei
    with open("blacklist.txt", "a") as f:
        f.write(mac + "\n")
        print(f"MAC {mac} wurde in blacklist.txt geschrieben.")
        #eingabe auffordern um weiter zu machen
        input("Drücke Enter um fortzufahren...")
    return True


def scan_ports():


    print("Scanne Ports...")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"Port: {port.device} - {port.description}")
        if 'USB-SERIAL CH340' in port.description or "Silicon Labs CP210x USB to UART Bridge" in port.description:
            print(f"ESP8266 erkannt auf: {port.device}")
            try:
                if program_esp8266(port.device):
                    pass
                    #messagebox.showinfo("Erfolg", "Flashen erfolgreich!")
            except Exception as e:
                print(f"Fehler beim Flashen: {e}")
                #print stacktrace
                #traceback.print_exc()
                #messagebox.showinfo("Fehler", "Flashen fehlgeschlagen!")

def watch_ports():
    print("Beobachte Ports...")
    while True:
        scan_ports()
        time.sleep(1)

#erstelle klasse, die std out beschreibt und alles in eine varriable speichert
class StdoutRedirector():
    def __init__(self):
        self.text = ""
    def write(self, string):
        self.text += string + "\n"
        sys.__stdout__.write(string)

    def flush(self):
        sys.__stdout__.flush()

    def getMac(self):
        mac = ""
        #MAC: a8:48:fa:c1:4f:0b
        tmp =  self.text.split("\n")
        for line in tmp:
            if "MAC:" in line:
                mac = line.split(" ")[1]
        return mac
    def isatty(self):
        return True


def main():
    global mystdout
    mystdout = StdoutRedirector()
    sys.stdout = mystdout
    watch_ports()


if __name__ == "__main__":
    main()
