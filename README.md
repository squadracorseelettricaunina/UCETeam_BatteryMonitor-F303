# UCETeam_BatteryMonitor-F303
Porting su F303K8 del BatteryMonitor creato originariamente su F249ZI.

Questo è un progetto per catturare e trasmettere le temperature di 7 sensori di temperatura dal Pacco Batteria Low Voltage.

## Schema dei collegamenti elettrici

| Pin  | Canale ADC | Descrizione Segnale |
|------|------------|---------------------|
| PA0  | ADC1 CH1   | Batteria 1          |
| PA1  | ADC1 CH2   | Batteria 2          |
| PA3  | ADC1 CH4   | Batteria 3          |
| PA4  | ADC3 CH1   | Batteria 4          |
| PA5  | ADC3 CH2   | Batteria 5          |
| PA6  | ADC3 CH3   | Batteria 6          |
| PA7  | ADC1 CH4   | Batteria 7          |

## Formato dei dati

Il programma trasmette i risultati delle conversioni sulla porta seriale ST-LINK alla frequenza di cifra di 115.2kbps nel formato Comma Separated Values (CSV).

Viene trasmessa una riga ogni mezzo secondo, ogni colonna corrisponde alla temperatura convertita di una Batteria. L'unità di misura è Grado Celsius.

### Esempio

```
27.5,27.5,27.5,27.5,27.5,27.5,27.5
```

Progetto originale a cura di [Mariano Baruffini](https://github.com/MarianoBaruffini)
Porting su F303K8 a cura di [Federico Ciardi](https://github.com/tuberbacon)
