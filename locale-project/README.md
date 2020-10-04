# Locale

This section of the project is used to compile a ModText.dll which contains
localisation strings for the mod.

## Modifying strings

Modify the strings directly within ModText.rc in your favourite text editor.
Do not use visual studio to open the resource file in it's own editor. That will
cause the file to become automatically generated and difficult to modify in
other editors.

## Getting the dll

Once you're satisfied with your changes, you can run the following command
to create and install the dll: `./scripts/build-locale.ps1`
