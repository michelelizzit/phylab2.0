# [PhyLab 2.0] (https://lizzit.it/phylab)

More details available at https://lizzit.it/phylab

##General description
PhyLab 2.0 è un sistema modulare e open source per l’acquisizione e l’elaborazione di dati; rende l'esperienza di laboratorio accattivante, facilita l'apprendimento della fisica laboratoriale.  
Acquisisce dati in maniera automatica, anche a frequenza elevata. È composto da unità di acquisizione-trasmissione (basate su Arduino) collegate a uno o più sensori; esse inviano i dati via wireless all’unità centrale (RaspberryPI) che li elabora in grafici. I dati e i grafici sono visualizzabili in tempo reale anche su smartphone. Può acquisire diversi tipi di dati (temperatura, umidità, forza…).  

##Hardware
L’unità centrale è basata su RaspberryPI versione 1, 2 o 3.
Ogni modulo trasmittente invia i dati all’unità centrale tramite un nRF24L01+ oppure tramite connessione USB.  
I dati vengono salvati sull’unità centrale e inviati a tutti dispositivi connessi.
L’unità centrale funziona anche da server web e come hotspot WiFi, questo consente al sistema un trasporto agevole e rapido.  
Il sistema è stato ideato per l’utilizzo nei laboratori scolastici di fisica, da questo deriva il nome PhyLab 2.0  
Le unità trasmittenti sono solitamente basate su Arduino, connesso a un modulo nRF24L01+ e a un sensore, ma qualunque microcontrollore può essere impiegato.  
###PhyLab PCBs
Abbiamo progettato due PhyLab PCB, uno per collegare facilmente un modulo nRF24L01+ al RaspberryPI, e uno che permette di costruire facilmente una unità trasmittente; ulteriori dettagli sono disponibili su https://lizzit.it/phylab  
<img src="http://i.imgur.com/BDOgZFu.jpg" width=400px/>
<img src="http://i.imgur.com/aqlaFSg.jpg" width=400px/>
<img src="http://i.imgur.com/2D74W3K.jpg" width=400px/>
<img src="http://i.imgur.com/K00j45U.jpg" width=400px/>

##Software
Un immagine SD per RaspberryPI è disponibile al download; include tutto il software necessario al funzionamento di PhyLab 2.0 e inoltre crea una rete WiFi a cui collegarsi per visualizzare il grafico.  
L'immagine è basata su PhyLab OS, sistema operativo Linux che include un kernel compilato con i soli moduli necessari a far funzionare PhyLab 2.0  
Questo consente a PhyLab 2.0 di avviarsi in appena 5 secondi ed essere subito operativo, senza alcuna configurazione richiesta.  
È particolarmente utile durante le lezioni di fisica perchè consente a ogni studente di ricevere i dati sul proprio smartphone, visualizzarli sul grafico e scaricarli in formato CSV.  
Il software per alcune unità trasmittenti che abbiamo sviluppato (umidità, temperatura, accelerazione, distanza, tensione, corrente...) verrà rilasciato a breve.  
L’immagine SD include inoltre un server DNS e un server DHCP (dnsmasq) che permette a ogni dispositivo di collegarsi facilmente e visualizzare il grafico andando su http://phy.lab/  

##Demo online

Una versione dimostrativa online è disponibile su: https://lizzit.it/phylab/demo/  
La versione demo mostra solamente l’interfaccia web, con dati casuali, senza essere connessa ad alcun sensore.  
<img src="http://i.imgur.com/mhHIL1G.png" width=200px/>
<img src="http://i.imgur.com/1rWJHnS.png" width=200px/>
<img src="http://i.imgur.com/liXiX2X.png" width=200px/>
<img src="http://i.imgur.com/78djOSZ.png" width=200px/>
<img src="http://i.imgur.com/eIriPYH.png" width=816px/>
