# DAVIS - Data Visulaization and Integration Shell

Willkommen zu DAVIS, einer benutzerdefinierten Shell, die im Rahmen der Vorlesung "Operating Systems" entwickelt wurde. DAVIS unterstützt grundlegende Shell-Befehle sowie spezielle Funktionen zum Plotten von Graphen und zur Erstellung von LaTeX-Vorlagen.

## Inhaltsverzeichnis

- [Installation und Start](#installation-und-start)
- [Grundlegende Befehle](#grundlegende-befehle)
- [Zusätzliche Befehle Befehle](#zusätzliche-befehle)

## Installation und Start

1. Klone das Repository:
    ```sh
    git clone https://github.com/schn-lars/Project
    ```
2. Wechsle in das Verzeichnis:
    ```sh
    cd DAVIS/Project/src
    ```
3. Compile den Code, installiere Gnuplot und starte DAVIS:
    ```sh
    ./DAVIS
    ```

## Grundlegende Befehle
DAVIS bietet folgende Shell-commands mit unserer eigenen Implementation an: __clear, echo, help, hist, ls__ und __quit__.
Einige commands können mit flags und argumenten benutzt werden. Wie und welche das sind, kann man entweder im Manual (DAVIS/DAVIS_Manual.pdf) oder mit dem command 'help' herausfinden.

Alle anderen commands die eine UNIX-Shell bietet werden auch von DAVIS unterstützt mit der üblichen Implementation vom Operating System.

## Zusätzliche Befehle
Zusätzlich zu den bekannten commands bietet DAVIS auch noch folgende Befehle: __latex, plot__ und __wordle__.
Der __'latex'__ Befehl erstellt mit den richtigen Argumenten ein template für das Programm LaTeX.
Mit dem __'plot'__ Befehl kann aus einer Datei mit Daten Graphen plotten und speichern. Auch hierfür werden Argumente und flags verwendet.
Der __'wordle'__ Befehl dient nur zum Spass und startet das Spiel Wordle.
