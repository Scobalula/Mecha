### **NOTE: No support is provided for this, it was updated for a friend who works on a CoD OL Mod and it working for him was all that mattered, it is provided AS IS with absolutely no warranty. Do not ask how to use it, report issues, etc. Updates for game updates may be provided but are not guaranteed at all. Bug reports will not be taken and issues will be closed! Features requests will not be taken! You really need to know what you're doing to use this.**

# Mecha

Mecha is a fork of DTZxPorter's [Wraith Cyborg](https://github.com/dtzxporter/WraithXOL) which is an asset extractor for Call of Duty: Online.

# Using Mecha

Mecha works in close to the same way as the original Wraith Cyborg, however I have made some changes to make exporting much easier.

The initial steps are very much the same, and due to possible protections brought into Windows you may need to restart before using Mecha if it fails to find the game due to the anti-cheat used in Call of Duty: Online. You also may need to add an exception to your anti-virus, etc.

To being follow these steps exactly:

* 1: Close all running programs if possible, to avoid any interference, etc. Especially Tencent's services such as WeGame, etc.
* 2: Launch `Mecha.exe`, at this point it should say `Waiting for Call of Duty: Online (Launch game...)`
* 3: Launch Call of Duty: Online, if successful, `Game process found! Permissions granted.`

If you run into any issues, then it's best to restart your PC and try again.

# Exporting

## Commands

Once in game you can use commands to export assets, basic commands are:

```
-ripmodels // Rips Aodels
-ripanims // Rips Anims
-ripimages // Rips Images
-ripsounds // Rips Sounds
```

You can also pass some flags to define export formats (by default MA and SEAnim are exported):

```
-xanimwaw // Note this disables Bo1 XAnims for obvious reasons, and vice versa
-xanimbo
-seanim

-obj
-xna
-smd
-ma
-xme
-semodel
```

Examples:

```
-ripanims
```

```
-ripanims -seanim -xanimbo
```
```
-ripanims -ripmodels -semodel
```

## Filter

You can also filter assets so you don't export every single thing loaded by just passing without a dash, for example:

```
-ripanims -ripmodels _vmgun viewmodel_
```

## IFS

You can also drag and drop IFS files, and files within them will be exported to their original full paths.

## Credits/Contributors

Wraith Cyborg was originally developed by DTZxPorter, updated by me to provide new memory addresses and some slight new features. It would be appreciated if credit could be provided for its use.
