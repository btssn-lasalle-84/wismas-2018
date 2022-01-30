#!/bin/bash
# Script d'enregistrement video
# \author Thierry Vaira <tvaira@free.fr>
# 2018

# la fonction qui arrête l'enregistrement
arreter()
{
   # journalise l'evenement
   echo "Arret du PID vlc $PID_VLC" >> ./.cvlc.log 
   
   # termine le processus vlc lance en arriere plan
   kill $PID_VLC
}

# executera la fonction arreter à la reception des signaux de terminaison 0 1 2 3 9 15 (cf. kill -l)
trap arreter 0 1 2 3 9 15

#echo $#
if [[ $# = 5 ]]; then 
   # lance en arriere plan la commande d'enregistrement vlc sans interface graphique
   /usr/bin/vlc -I dummy "http://$1:$2/videostream.cgi?user=$3&pwd=&resolution=32&rate=0" --sout "#transcode{vcodec=h264} :file{mux=ts,dst=$4}" &

   # recupere le PID du processus vlc lance
   PID_VLC=$!
   
   # journalise l'evenement
   echo "Demarrage du PID vlc $PID_VLC par le script $$ pour une dure $5 s" >> ./.cvlc.log 

   # attend la duree d'enregistrement programme
   sleep $5
elif [[ $# = 6 ]]; then
   # lance en arriere plan la commande d'enregistrement vlc sans interface graphique
   /usr/bin/vlc -I dummy "http://$1:$2/videostream.cgi?user=$3&pwd=$4&resolution=32&rate=0" --sout "#transcode{vcodec=h264} :file{mux=ts,dst=$5}" &

   # recupere le PID du processus vlc lance
   PID_VLC=$!

   # journalise l'evenement
   echo "Demarrage du PID vlc $PID_VLC par le script $$ pour une dure $5 s" >> ./.cvlc.log 

   # attend la duree d'enregistrement programme
   sleep $6
else
   echo "Utilisation : $0 [adresseIP] [numeroDePort] [identifiant] [cheminVideo] [duree]"	
   echo "Utilisation : $0 [adresseIP] [numeroDePort] [identifiant] [motDePasse] [cheminVideo] [duree]"
fi

# journalise l'evenement
echo "Fin du script $$" >> ./.cvlc.log 
exit 0

