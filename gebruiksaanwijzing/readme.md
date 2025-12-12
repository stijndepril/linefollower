# Gebruiksaanwijzing

### opladen / vervangen batterijen
De batterijen kunnen van de wagen worden gehaald door de rekker rond de arduino los te maken. Indien de batterijen te stevig vast zitten kan men steeds een schroevendraaier gebruiken om ze los te krijgen. Het is aangeraden steeds de spanning van deze batterijen te controleren met een multimeter. Als deze geen 8V meer leveren laad je ze best op. Dit kan door ze in liitokala oplader te steken. Vanaf de lampjes groen zijn, hebben de batterijen weer voldoende spanning en kan de wagen zonder problemen even door rijden.

### draadloze communicatie
#### verbinding maken
Om verbinding te maken met de smartphone zorg je eerst dat de wagen voorzien is van de correcte spanning (de twee batterijen hebben samen >8V). Als tweede neem je de smarthpone en download je de app "Serial Bluetooth Terminal" (android). Deze app zal het mogelijk te communiceren met onze wagen. Dit gebeurt aan de hand van verschillende commando's. De meest gebruikte commando's kunnen eenvoudig in 4 drukknoppen worden verwerkt. Deze 4 knoppen zijn op de foto in het geel aangeduid. Om deze net zoals mij te programmeren druk je lang op de knop en stel je de volgende zaken in: Knop 1 -> Name: "on", Value: "run on" Knop 2 -> Name: "off", Value: "run off" Knop 3 -> Name: "debug", Value: "debug" Knop 4 -> Name: "?", "cmd"

<img width="921" height="2048" alt="image" src="https://github.com/user-attachments/assets/f3b73003-392a-4bbd-9886-e5e1852e25ba" />


#### commando's
debug [on/off]  
start  
stop  
set cycle [µs]  
set power [0..255]  
set diff [0..1]  
set kp [0..]  
set ki [0..]  
set kd [0..]  
calibrate black  
calibrate white  

### kalibratie
Wanneer er voor de eerste keer wordt gereden op een bepaald parkour is het belangrijk om de sensoren te calibreren. Dit zal er voor zorgen dat er een duidelijk verschil tussen wit en zwart wordt herkent.

### settings
De robot rijdt stabiel met volgende parameters:  
<img width="921" height="2048" alt="image" src="https://github.com/user-attachments/assets/a1c598a0-1ccd-4a4e-ab0b-a88a748d48f5" />
