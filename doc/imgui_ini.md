# `imgui.ini` — Guide

`imgui.ini` stocke la disposition des fenêtres ImGui (position, taille, état, docking) entre deux lancements. ImGui le **lit au démarrage** et le **réécrit automatiquement** (toutes les ~5 s et à la fermeture).

> En pratique, on ne le remplit presque jamais à la main : on arrange les fenêtres dans l'éditeur, ImGui écrit le fichier. Cette doc sert à comprendre le format et à corriger/préparer un layout par défaut.

---

## 1. Format général

Fichier texte, une section par élément, séparées par une ligne vide :

```ini
[Type][Nom]
Clé=Valeur
Clé=Valeur
```

- `Type` : `Window`, `Table`, `Docking`, `Layout`…
- `Nom` : le **titre exact** passé à `ImGui::Begin("...")` (voir §3).

---

## 2. Section `[Window]`

```ini
[Window][Viewport]
Pos=-5,67
Size=794,594
Collapsed=0
DockId=0x00000001,0
```

| Clé | Signification |
|-----|---------------|
| `Pos=x,y` | Position du coin haut-gauche, en pixels (coordonnées écran/viewport). Peut être négative. |
| `Size=w,h` | Taille en pixels. |
| `Collapsed=0\|1` | `1` = fenêtre repliée (seule la barre de titre visible). |
| `DockId=0xID,n` | Fenêtre dockée dans le nœud `0xID` ; `n` = index de l'onglet dans ce nœud. Si présent, `Pos`/`Size` ne servent plus que de valeurs de secours quand on la détache. |
| `ViewportId=0x...` | Viewport natif associé (multi-viewport). Laisser ImGui le gérer. |
| `ViewportPos=x,y` | Position de la fenêtre détachée dans le bureau (multi-viewport). |

Les fenêtres absentes du fichier prennent les valeurs par défaut du code (`SetNextWindowPos/Size` avec `ImGuiCond_FirstUseEver`).

---

## 3. Le nom de section = le titre de la fenêtre

ImGui identifie une fenêtre par le **hash de son titre**. Donc :

- Renommer une fenêtre dans le code → l'ancienne section devient orpheline (ignorée) et la fenêtre repart des valeurs par défaut.
- `##` sépare le label affiché de l'ID : `"Debug##Default"` s'affiche « Debug » mais l'ID est `Debug##Default`.
- Une partie après `###` est le seul ID (le label peut changer sans perdre le layout).

Fenêtres actuelles de ProutEngine (voir [editorLayer.cpp](../src/editor/editorLayer.cpp) et [imguiLayer.cpp](../src/debug/imguiLayer.cpp)) :

| Section | Origine |
|---------|---------|
| `[Window][Viewport]` | Rendu de la scène dans l'éditeur |
| `[Window][Inspector]` | Panneau d'inspection |
| `[Window][ProutEngine Debug]` | Fenêtre debug (FPS, liste d'entités) |
| `[Window][Debug##Default]` | Fenêtre ImGui par défaut quand aucune `Begin` n'est appelée — ignorable |

> Les sections `[Window][ProutEngine]` actuelles du fichier datent d'un ancien titre : elles ne correspondent plus à rien et peuvent être supprimées.

---

## 4. Docking (branche `docking`)

La lib dans `lib/imgui` est la branche **docking** (`IMGUI_HAS_DOCK`). Si `ImGuiConfigFlags_DockingEnable` est activé, le fichier contient en plus :

```ini
[Window][Viewport]
Pos=0,24
Size=900,700
Collapsed=0
DockId=0x00000003,0

[Docking][Data]
DockSpace   ID=0x4647B76E Window=0x3BC79352 Pos=0,24 Size=1280,696 Split=X
  DockNode  ID=0x00000001 Parent=0x4647B76E SizeRef=900,696 CentralNode=1 Selected=0x...
  DockNode  ID=0x00000002 Parent=0x4647B76E SizeRef=380,696 Selected=0x...
```

- `DockSpace` : racine ; `Split=X` (côte à côte) ou `Split=Y` (empilé).
- `DockNode` : nœud enfant ; `SizeRef` = taille de référence, proportions conservées au redimensionnement.
- `CentralNode=1` : zone centrale (typiquement le viewport).
- Les `DockId` des fenêtres pointent vers les `ID=` des nœuds.

**Conseil** : ne jamais écrire cette partie à la main (les IDs sont des hash). Construire le layout une fois dans l'UI, puis copier le fichier.

---

## 5. Section `[Table]`

Écrite pour les `ImGui::BeginTable` (largeur/ordre/visibilité des colonnes) :

```ini
[Table][0xA1B2C3D4,3]
RefScale=13
Column 0  Weight=1.0000
Column 1  Weight=2.0000 Hidden=1
```

Entièrement géré par ImGui.

---

## 6. Workflow recommandé

1. Lancer l'éditeur, arranger les fenêtres à la souris (déplacer, redimensionner, docker).
2. Fermer proprement l'application (force l'écriture du fichier).
3. Vérifier le diff de `imgui.ini`, committer si c'est le layout par défaut voulu.
4. Pour **réinitialiser** : supprimer `imgui.ini` (ou les sections concernées) → valeurs par défaut du code.

### Layout par défaut vs. layout utilisateur

Le fichier est versionné ici (il est modifié dans `git status`), donc chaque dev l'écrase à chaque session. Options :

- **Garder un layout par défaut commité** et ajouter `imgui.ini` au `.gitignore` après avoir copié le fichier en `imgui.default.ini` (à charger si `imgui.ini` est absent).
- **Changer l'emplacement** : `ImGui::GetIO().IniFilename = "build/imgui.ini";` (ou `nullptr` pour désactiver la sauvegarde).
- **Charger/sauver à la main** : `ImGui::LoadIniSettingsFromDisk(path)` / `ImGui::SaveIniSettingsToDisk(path)`.

> Le chemin est **relatif au répertoire de travail** de l'exécutable : lancer depuis un autre dossier crée un autre `imgui.ini`.

---

## 7. Pièges courants

- **Fenêtre hors écran** (`Pos` négatif ou trop grand après un changement d'écran) : corriger `Pos` ou supprimer la section.
- **Layout non pris en compte** : le titre de la fenêtre a changé (§3), ou le fichier est lu depuis un autre working directory.
- **Modifications manuelles écrasées** : ImGui réécrit le fichier ; éditer l'ini **application fermée**.
- **`Size` ignorée** : si la fenêtre est dockée, c'est le nœud de dock qui impose la taille.
