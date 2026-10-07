# Prosit 5 — Pile et mémoire sur STM32

Petit projet PlatformIO pour la **NUCLEO-L476RG**. Il illustre une traduction du pseudo-code du Prosit 5 avec trois capteurs simulés, dix mesures récentes par capteur, un compteur d'erreurs et une fenêtre circulaire.

## Lancer le programme

1. Ouvrir ce dossier dans VS Code avec l'extension PlatformIO.
2. Brancher la carte NUCLEO-L476RG par USB.
3. Compiler avec **PlatformIO: Build**, puis téléverser avec **PlatformIO: Upload**.
4. Lancer **PIO Debug** et placer un point d'arrêt dans `lecture` ou juste après son appel dans `main`.

En terminal, les deux premières actions correspondent à :

```sh
pio run
pio run -t upload
```

La LED PA5 change d'état à chaque cycle. Les valeurs sont **simulées** : ce code ne lit pas de vrais capteurs. Il n'écrit rien sur l'UART, donc un moniteur série vide est normal.

## Où sont les tableaux ?

Dans [`src/main.c`](src/main.c), `static Capteur capteurs[NB_CAPTEURS]` est le tableau des **trois capteurs**. Chaque `Capteur` contient son propre `int16_t valeurs[NB_VAL]`, soit **dix mesures**. Dans le débogueur, déplier `capteurs`, puis `capteurs[0]`, puis `valeurs`.

`ajouter_valeur` écrit dans la prochaine case et revient à la case 0 après la dixième. `lecture` passe l'adresse des structures : l'historique n'est pas copié. Le troisième capteur simule une erreur tous les trois cycles et incrémente son compteur sans ajouter de mesure.

## Ce que montre ce prosit sur la mémoire

Chaque structure `Capteur` occupe **24 octets** dans cette compilation ARM ; le tableau de trois capteurs occupe **72 octets**. Le tableau est `static` : il garde son contenu entre les cycles et se trouve dans les données permanentes (`.bss`), pas dans la pile d'appels.

Pour comparer, la même source compilée en mode débogage (`-O0 -fstack-usage`) avec un tableau **automatique** dans `main` donne une trame de **112 octets**, contre **40 octets** avec le tableau `static`. Les **72 octets libérés sur la pile restent utilisés en SRAM** dans `.bss` : déplacer un objet ne réduit pas à lui seul la consommation totale de mémoire.

Dans la console de débogage GDB, `p/x $msp` affiche la position actuelle de la pile. Un arrêt donne une valeur **instantanée**, pas son maximum sur tous les chemins du programme ; les fonctions HAL et les interruptions doivent aussi être prises en compte pour dimensionner la pile du vrai projet.

Le choix de `int16_t` pour les mesures n'est valable pour de vrais capteurs que si leur plage et leur précision le permettent. Le code est une démonstration pédagogique du pseudo-code, pas le firmware complet du projet WWW.
