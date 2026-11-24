# esphome-cc1101

Odbiornik wM-Bus dla ESPHome z jednym modułem CC1101, który przełącza się między
częstotliwościami w trakcie pracy. Przykład w `wmbus-xiao.yaml` odczytuje:

- licznik prądu Sagemcom XT211 (Enea): 868,3 MHz, tryb T, szyfrowanie OMS tryb 7, sterownik `amiplus`,
- ciepłomierz Techem vario 4.5.1: 868,95 MHz, otwarta ramka firmowa (CI A2),
- wodomierz Techem radio 4: 868,95 MHz, sterownik `mkradio4`.

CC1101 domyślnie słucha 868,3 MHz. Co `techem_interval` przestraja się na
868,95 MHz i wraca po odebraniu ramek Techem (najpóźniej po `techem_window`).

**Status:** prototyp. Kompiluje się (ESPHome 2026.10.1), nie był jeszcze testowany na sprzęcie.

## Użycie

1. W ESPHome utwórz urządzenie i wklej `wmbus-xiao.yaml`.
2. Wpisz ID liczników w `substitutions` (`00000000` = brak licznika).
3. Dodaj do `secrets.yaml` ESPHome:

   ```yaml
   wifi_ssid: "..."
   wifi_password: "..."
   ap_password: "..."
   api_encryption_key: "..."   # 32 bajty base64
   xt211_key: "..."            # klucz AES licznika XT211 (32 znaki hex)
   ```

Komponent pobiera się sam z tego repozytorium (`external_components`).

## Podłączenie (XIAO ESP32C3)

| CC1101 | XIAO ESP32C3 |
|---|---|
| GND | GND |
| VCC | 3V3 (nie 5V) |
| GDO0 | D1 (GPIO3) |
| CSN | D2 (GPIO4) |
| SCK | D3 (GPIO5) |
| MOSI | D4 (GPIO6) |
| MISO/GDO1 | D5 (GPIO7) |
| GDO2 | - |

Moduł CC1101 musi być w wersji 868 MHz (np. Ebyte E07-900M10S), a nie 433 MHz
(np. E07-M1101D).

## Pochodzenie i licencja

Komponenty w `components/` to kopia
[SzczepanLeon/esphome-components](https://github.com/SzczepanLeon/esphome-components)
(commit `03484db`, autorzy: @SzczepanLeon, @kubasaw i współtwórcy). Kod
w `wmbus_common` pochodzi z [wmbusmeters](https://github.com/wmbusmeters/wmbusmeters)
(Fredrik Öhrström i współtwórcy) i jest objęty licencją GPL-3.0-or-later,
zgodnie z nagłówkami plików. Całość rozpowszechniana jest na licencji GPL-3.0
(plik `LICENSE`).

### Zmiany względem oryginału

Wyłącznie w `components/wmbus_radio` (pełny diff: `patches/cc1101-retune.patch`):

- `RadioTransceiver::retune(hz)`, zaimplementowane dla CC1101: IDLE, zapis
  rejestrów FREQ2..0, powrót do RX z automatyczną kalibracją syntezatora,
- `Radio::set_frequency(hz)` / `Radio::get_frequency()`: zmiana częstotliwości
  z lambdy. Przestrojenie wykonuje wątek odbiornika, więc nie ma równoległego
  dostępu do SPI.
