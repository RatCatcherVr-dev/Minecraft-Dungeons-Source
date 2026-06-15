# Entitlements handling 
To test Dungeons, you need your account to be set up with the appropriate 
entitlements in the Mojang Services backend,
This is explained on the entitlements wiki page here: https://dev-mc.visualstudio.com/Lovika/_wiki/wikis/Lovika.wiki/5089/Entitlements

This python app automates some of the steps needed to modify your local account's requirements
including:
- Fetching an admin access token
- Finding the userId hash from a Dungeons.log file
- built-in printout of the available entitlements (subject to change)
- Adding/Removing entitlements with a command line toggle

Note: the app will only send requests for entitlements checked against the list (in source/EntitlementsEditor.py)
When new entitlements are added, this file needs to be modified as it is currently written.


## How-to guide
Make sure you have python3 installed and on your PATH
Go to a command line and cd into this directory:
```
cd /Dungeons/root/Tools/entitlements
```

To make sure the python requirements are installed run:
```
pip install -r requirements.txt
```

### Run the script
The default use case is to _add missing entitlements_;
Running the app with no args will print the [last signed in] user's current entitlements AND prompt you to list entitlements to add.

You may want to point the app to a Dungeons.log in a downloaded build, which is done with the `-p` flag like so:
```
py main.py --list -p=f:/builds/dlc_4290914_test
```

You can run 
```
py main.py --help
```
to display other options, like `--list` to only list current entitlements


### credentials
To be allowed to access https://backoffice-staging.minecraftservices.com/backoffice
you need to be signed on to Mojang VPN
The app will query you for a username and password. If you enter nothing, it will fall back on the 
staging environment credentials


## to do:
1. add production environment URL as an option...
2. robuster handling of added entitlements without changing the source of this app, but still retaining some checks