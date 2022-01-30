--
-- Base de données: `WISMAS_2018`
--

CREATE DATABASE IF NOT EXISTS `WISMAS_2018`;

USE `WISMAS_2018`;

-- --------------------------------------------------------

--
-- Structure de la table `sites`
--

CREATE TABLE IF NOT EXISTS `sites` (
  `idSite` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `nom` varchar(255) NOT NULL, 
  `etat` int(11) NOT NULL, 
  PRIMARY KEY (`idSite`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `sites` (`idSite`,`type`,`nom`,`etat`) VALUES('101','1','Vallée de la mort', 1);
INSERT INTO `sites` (`idSite`,`type`,`nom`,`etat`) VALUES('201','2','Mont Blanc', 1);
INSERT INTO `sites` (`idSite`,`type`,`nom`,`etat`) VALUES('301','3','Mont noir', 1);

-- --------------------------------------------------------

--
-- Structure de la table `mesures`
--

CREATE TABLE IF NOT EXISTS `mesures` (
  `idMesure` int(11) NOT NULL AUTO_INCREMENT,
  `idSite` int(11) NOT NULL,
  `dateMesure` date NOT NULL,
  `heure` int(4),
  `temperatureAir` FLOAT, `temperatureNeige` FLOAT,
  `hauteurNeige` int(4), `humidite` int(4), `pressionAir` int(4), 
  `vitesseVent` FLOAT, `directionVent` VARCHAR(3),    
  PRIMARY KEY (`idMesure`),
  CONSTRAINT mesures_fk_1 FOREIGN KEY (`idSite`) REFERENCES sites(`idSite`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Structure de la table `informations`
--

CREATE TABLE IF NOT EXISTS `informations` (
  `idInfo` int(11) NOT NULL AUTO_INCREMENT,
  `idSite` int(11) NOT NULL,  
  `tarifs` varchar(255) NOT NULL, 
  `horaire` varchar(255) NOT NULL, 
  `nbPisteOuverte` int(4),
  PRIMARY KEY (`idInfo`),
  CONSTRAINT informations_fk_1 FOREIGN KEY (`idSite`) REFERENCES sites(`idSite`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `informations` (`idSite`,`horaire`,`tarifs`,`nbPisteOuverte`) VALUES('101','De 9h à 17h','Adulte 14 euros / journée, Jeune/Etudiant 11 euros / journée', 10);
INSERT INTO `informations` (`idSite`,`horaire`,`tarifs`,`nbPisteOuverte`) VALUES('201','De 8h30 à 16h30','Adulte 13 euros / journée, Jeune/Etudiant 10 euros / journée', 8);
INSERT INTO `informations` (`idSite`,`horaire`,`tarifs`,`nbPisteOuverte`) VALUES('301','De 10h à 17h30','Adulte 10 euros / journée, Jeune/Etudiant 8 euros / journée', 2);
