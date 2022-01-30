# Le projet WISMAS 2018

- [Le projet WISMAS 2018](#le-projet-wismas-2018)
  - [Présentation](#présentation)
  - [Auteurs](#auteurs)
  - [Productions](#productions)
  - [Vidéos](#vidéos)
  - [Licence GPL](#licence-gpl)

## Présentation

Le système d’information météo pour station multi activités WISMAS (Weather Information System Multi Activity Station) devra :

- Faire des mesures météorologiques sur plusieurs sites
- Prendre des séquences vidéos à partir de plusieurs caméras IP
- Afficher l’ensemble de ces renseignements sur les sites d’achat des forfaits

La station sur laquelle s’appuie l’étude est une station de moyenne montagne multi-activités dans laquelle on peut faire du ski de fond, du ski de piste et également du Snow-Kite sur 3 sites voisins.

L’objectif est de moderniser l’ensemble en y ajoutant des fonctionnalités.

Il s’agit de développer :

- sur un système embarqué (carte ATMEL SAM4S-EK par exemple) les applications nécessaires à la mise en oeuvre des différents capteurs et à la transmission sans fil des données (Module de météorologie)
- sur un PC d’acquisition, un logiciel de gestion des caméras implantés sur les différents sites (Module d’acquisition « vidéo »)
- sur un poste de diffusion, un logicel de publication d’informations sur la station (Module de diffusion d’informations) : gestion du panneaux lumineux et d’un écran

On distinguera les modules suivants :

- Module de météorologie (ET1 EC)
- Module d’acquisition « vidéo » (ET2 IR)
- Module de diffusion d’informations (ET3 IR)

Le système sera construit à partir de carte(s) microcontrôleur qui seront mises en place sur les sites de mesures et d’un poste PC de contrôle.

L’application logicielle « Acquisition » doit permettre :

- D’acquérir les images vidéos des caméras IP installées sur les différents sites
- D’enregistrer les vidéos sur un serveur pour une diffusion ultérieure
- De commander les déplacements d’une caméra pour une acquisition panoramique

L’administrateur pourra paramétrer :

- Les adresses IP et numéros de port des différentes caméras
- La position d’origine de la prise de vue en pouvant déplacer la caméra manuellement
- Le type (fixe ou panoramique) et la durée d’acquisition vidéo
- De relever les mesures des stations météorologiques installées sur les différents sites
- D’enregistrer les mesures dans une base données installée sur le serveur
- De publier des bulletins météorologiques à partir des mesures relevées

Le poste de diffusion (une Raspberry Pi par exemple) devra gérer un panneau lumineux et un écran afin de publier des informations sur la station :

- Conditions météorologiques suivant le site (Alpin, Fond ou Snow kite) avec la possibilité de visualiser une vidéo du lieu
- Informations sur la station (accès, horaires, tarifs, …)
- Conseils de fartage en fonction des conditions météorologiques (en option)

Pour cela, l’application logicielle « Diffusion » doit permettre :

- De relever les mesures des stations météorologiques installées sur les différents sites
- D’enregistrer les mesures dans une base données
- De diffuser des informations

## Auteurs

- Olivier Petrella
- Pierre Grelet

## Productions

- [dossier-wismas-ir-2018.pdf](dossier-wismas-ir-2018.pdf)
- [presentation-wismas-grelet-ir-2018.pdf](presentation-wismas-grelet-ir-2018.pdf)
- [presentation-wismas-petrella-ir-2018.pdf](presentation-wismas-petrella-ir-2018.pdf)

## Vidéos

- https://www.youtube.com/watch?v=9SHUa4WcGPs
- https://www.youtube.com/watch?v=le1ccMHW768

## Licence GPL

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
